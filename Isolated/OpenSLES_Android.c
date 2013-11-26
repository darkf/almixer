#ifdef ALMIXER_COMPILE_WITHOUT_SDL

/*
 * SDL_sound Core Audio backend
 * Copyright (C) 2010 Eric Wing <ewing . public @ playcontrol.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#ifdef __ANDROID__

#include <stddef.h> /* NULL */
#include <stdio.h> /* printf */
#include <pthread.h>

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include "SoundDecoder.h"
#include "SoundDecoder_Internal.h"

#include <android/log.h>
#include <android/asset_manager.h>

static AAssetManager *asset_manager = NULL;
void ALmixer_OpenSLES_Android_SetAssetManager(AAssetManager *manager) {
    asset_manager = manager;
}

static int OpenSLES_init(void);
static void OpenSLES_quit(void);
static int OpenSLES_open(Sound_Sample* sound_sample, const char* file_ext);
static void OpenSLES_close(Sound_Sample* sound_sample);
static size_t OpenSLES_read(Sound_Sample* sound_sample);
static int OpenSLES_rewind(Sound_Sample* sound_sample);
static int OpenSLES_seek(Sound_Sample* sound_sample, size_t ms);

static const char *extensions_opensles[] = 
{
	"aif",
	"aiff",
	"aifc",
	"wav",
	"wave",
	"mp3",
	"mp4",
	"m4a",
	"aac",
	"adts",
	"caf",
	"Sd2f",
	"Sd2",
	"au",
	"snd",
	"next",
	"mp2",
	"mp1",
	"ac3",
	"3gpp",
	"3gp",
	"3gp2",
	"3g2",
	"amrf",
	"amr",
	"ima4",
	"ima",
	NULL 
};
const Sound_DecoderFunctions __Sound_DecoderFunctions_OpenSLES =
{
    {
        extensions_opensles,
        "Decode audio through Core Audio through",
        "Eric Wing <ewing . public @ playcontrol.net>",
        "http://playcontrol.net"
    },
	
    OpenSLES_init,       /*   init() method */
    OpenSLES_quit,       /*   quit() method */
    OpenSLES_open,       /*   open() method */
    OpenSLES_close,      /*  close() method */
    OpenSLES_read,       /*   read() method */
    OpenSLES_rewind,     /* rewind() method */
    OpenSLES_seek        /*   seek() method */
};


//-----------------------------------------------------------------

/* Explicitly requesting SL_IID_ANDROIDBUFFERQUEUE, SL_IID_PREFETCHSTATUS
 * SL_IID_SEEK and SL_IID_METADATAEXTRACTION */
#define NUM_EXPLICIT_INTERFACES_FOR_PLAYER 4

/* Size of the decode buffer queue */
#define NB_BUFFERS_IN_QUEUE 2

/* Number of decoded samples produced by one source frame; defined by the standard */
#define SAMPLES_PER_SRC_FRAME 1024

/* Size of each PCM buffer in the queue */
#define BUFFER_SIZE_IN_BYTES   (2 * sizeof(short) * SAMPLES_PER_SRC_FRAME)

/* size of the struct to retrieve the PCM format metadata values: the values we're interested in
 * are SLuint32, but it is saved in the data field of a SLMetadataInfo, hence the larger size.
 * Note that this size is queried and displayed at l.452 for demonstration/test purposes.
 *  */
#define PCM_METADATA_VALUE_SIZE 32

//-----------------------------------------------------------------

static SLEngineItf slEngineItf = NULL;
static SLObjectItf slEngine    = NULL;

typedef struct OpenSLESFileContainer {
    SLObjectItf player;
    SLPlayItf   playItf;
    SLAndroidSimpleBufferQueueItf decBuffQueueItf;
    SLSeekItf   seekItf;
    SLMetadataExtractionItf metaItf;

    SLboolean   formatQueried;
    SLMetadataInfo* metadata;
    int sampleRateKeyIndex;
    int channelCountKeyIndex;

    int8_t     *dstDataBase;
    int8_t     *dstData;

    int8_t     *initialBuffer;
    SLboolean   initialBufferDone;
    SLboolean   shouldReturnInitialBuffer;

    // prefetch status, mutex and condition to protect prefetch_status
    SLuint32 prefetch_status;
    pthread_mutex_t prefetch_mutex;
    pthread_cond_t  prefetch_cond;

    SLboolean   available;
    pthread_mutex_t decoder_mutex;
    pthread_cond_t  decoder_cond;

    SLboolean decode_waiting;
    SLboolean eos;

    AAsset* asset;

} OpenSLESFileContainer;

//-----------------------------------------------------------------

// These are extensions to OpenMAX AL 1.0.1 values
#define PREFETCHSTATUS_UNKNOWN ((SLuint32) 0)
#define PREFETCHSTATUS_ERROR   ((SLuint32) (-1))

/* used to detect errors likely to have occured when the OpenSL ES framework fails to open
 * a resource, for instance because a file URI is invalid, or an HTTP server doesn't respond.
 */
#define PREFETCHEVENT_ERROR_CANDIDATE \
        (SL_PREFETCHEVENT_STATUSCHANGE | SL_PREFETCHEVENT_FILLLEVELCHANGE)

/* Callback for "prefetch" events, here used to detect audio resource opening errors */
void prefetchEventCallback(SLPrefetchStatusItf caller, void *context, SLuint32 event)
{
    if (context == NULL) return;
    OpenSLESFileContainer *file_container = (OpenSLESFileContainer *)context;

    SLpermille level = 0;
    SLresult result;
    result = (*caller)->GetFillLevel(caller, &level);
    if (SL_RESULT_SUCCESS != result) {
        SNDDBG("Failed to get FiilLevel from SLPrefetchStatusItf");
        return;
    }

    SLuint32 status;
    result = (*caller)->GetPrefetchStatus(caller, &status);
    if (SL_RESULT_SUCCESS != result) {
        SNDDBG("Failed to get PrefetchStatus from SLPrefetchStatusItf");
        return;
    }

    SNDDBG("prefetch level=%d status=0x%x event=%d", level, status, event);
    SLuint32 new_prefetch_status;
    if ((PREFETCHEVENT_ERROR_CANDIDATE == (event & PREFETCHEVENT_ERROR_CANDIDATE))
            && (level == 0) && (status == SL_PREFETCHSTATUS_UNDERFLOW)) {
        SNDDBG("PrefetchEventCallback: Error while prefetching data, exiting");
        new_prefetch_status = PREFETCHSTATUS_ERROR;
    } else if (event == SL_PREFETCHEVENT_STATUSCHANGE) {
        new_prefetch_status = status;
    } else {
        return;
    }
    pthread_mutex_lock(&file_container->prefetch_mutex);
    file_container->prefetch_status = new_prefetch_status;
    pthread_cond_signal(&file_container->prefetch_cond);
    pthread_mutex_unlock(&file_container->prefetch_mutex);
}

//-----------------------------------------------------------------

void SignalEos(OpenSLESFileContainer *file_container) {
    pthread_mutex_lock(&file_container->decoder_mutex);
    file_container->eos = SL_BOOLEAN_TRUE;
    pthread_cond_signal(&file_container->decoder_cond);
    pthread_mutex_unlock(&file_container->decoder_mutex);
}

static void decodePlayCallback(
        SLAndroidSimpleBufferQueueItf queueItf, void *context) {

    if (context == NULL) return;
    Sound_Sample *sample = (Sound_Sample *)context;
    Sound_SampleInternal *internal = (Sound_SampleInternal *)sample->opaque;
    OpenSLESFileContainer *file_container = (OpenSLESFileContainer *)internal->decoder_private;

    pthread_mutex_lock(&file_container->decoder_mutex);
    file_container->available = SL_BOOLEAN_TRUE;
    pthread_cond_signal(&file_container->decoder_cond);

    file_container->decode_waiting = SL_BOOLEAN_TRUE;
    while(file_container->available == SL_BOOLEAN_TRUE && file_container->eos == SL_BOOLEAN_FALSE) {
        pthread_cond_wait(&file_container->decoder_cond, &file_container->decoder_mutex);
    }
    file_container->decode_waiting = SL_BOOLEAN_FALSE;
    pthread_cond_signal(&file_container->decoder_cond);

    if (file_container->eos == SL_BOOLEAN_FALSE) {
        SLresult result = (*file_container->decBuffQueueItf)->Enqueue(
            file_container->decBuffQueueItf, file_container->dstData, BUFFER_SIZE_IN_BYTES);
        if (SL_RESULT_SUCCESS != result) {
            SNDDBG("Failed to enqueue to decode buffer queue");
        }

        /* Increase data pointer by buffer size */
        file_container->dstData += BUFFER_SIZE_IN_BYTES;
        if (file_container->dstData >= file_container->dstDataBase + (NB_BUFFERS_IN_QUEUE * BUFFER_SIZE_IN_BYTES)) {
            file_container->dstData = file_container->dstDataBase;
        }
        memset(file_container->dstData, 0, BUFFER_SIZE_IN_BYTES);
    }

    if (file_container->formatQueried == SL_BOOLEAN_FALSE) {
        /* retrieve sample rate */
        SLresult result = (*file_container->metaItf)->GetValue(
                                file_container->metaItf, file_container->sampleRateKeyIndex,
                                PCM_METADATA_VALUE_SIZE, file_container->metadata);
        if (SL_RESULT_SUCCESS == result) {
            sample->actual.rate = *((SLuint32*)file_container->metadata->data);
        }
        /* retrieve channel count */
        result = (*file_container->metaItf)->GetValue(file_container->metaItf,
            file_container->channelCountKeyIndex, PCM_METADATA_VALUE_SIZE, file_container->metadata);
        if (SL_RESULT_SUCCESS == result) {
            sample->actual.channels = *((SLuint32*)file_container->metadata->data);
        }

        /* Update sample attributes */
        sample->actual.format = AUDIO_S16SYS;
        sample->flags = SOUND_SAMPLEFLAG_CANSEEK;
        sample->desired = sample->actual;

        SNDDBG("Sound metadata: rate=%d, channels=%d", sample->actual.rate, sample->actual.channels);

        file_container->formatQueried = SL_BOOLEAN_TRUE;
    }

    pthread_mutex_unlock(&file_container->decoder_mutex);
}

/* Callback for "playback" events, i.e. event happening during decoding */
static void decodeProgressCallback(
        SLPlayItf caller, void *context, SLuint32 event) {
    SLresult result;
    SLmillisecond msec;

    if (context == NULL) return;
    OpenSLESFileContainer *file_container = (OpenSLESFileContainer*)context;

    result = (*caller)->GetPosition(caller, &msec);
    if (SL_RESULT_SUCCESS != result) {
        SNDDBG("Failed to get position from OpenSLES progress callback");
        SignalEos(file_container);
        return;
    }

    if (SL_PLAYEVENT_HEADATEND & event) {
        SNDDBG("SL_PLAYEVENT_HEADATEND current position=%u ms", msec);
        SignalEos(file_container);
        return;
    }
}

//-----------------------------------------------------------------

//-----------------------------------------------------------------

static int OpenSLES_init() {
    SLresult result;

    /* SL Engine option for synchronous mode */
    SLEngineOption EngineOption[] = {
        {(SLuint32) SL_ENGINEOPTION_THREADSAFE, (SLuint32) SL_BOOLEAN_TRUE}
    };

    result = slCreateEngine( &slEngine, 1, EngineOption, 0, NULL, NULL);
    if (SL_RESULT_SUCCESS != result) {
        SNDDBG("Failed to create OpenSLES engine object");
        return(0);
    } else {
        SNDDBG("Create OpenSLES engine object - OK");
    }

    result = (*slEngine)->Realize(slEngine, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result) {
        SNDDBG("Failed to realize OpenSLES engine object");
        return(0);
    } else {
        SNDDBG("Realize OpenSLES engine object - OK");
    }

    /* Get the SL Engine Interface which is implicit */
    result = (*slEngine)->GetInterface(slEngine, SL_IID_ENGINE, (void*)&slEngineItf);
    if (SL_RESULT_SUCCESS != result) {
        SNDDBG("Failed to get OpenSLES engine interface");
        return(0);
    } else {
        SNDDBG("Get OpenSLES engine interface - OK");
    }

    return(1);
}

static void OpenSLES_quit() {
    if (slEngine != NULL) {
        SNDDBG("Destroy OpenSLES Engine");
        (*slEngine)->Destroy(slEngine);
        slEngine = NULL;
    }
}

static int OpenSLES_open(Sound_Sample *sample, const char *ext) {

    Sound_SampleInternal *internal = (Sound_SampleInternal *)sample->opaque;

    SLresult result;

    /* Objects this application uses: one audio player */
    SLObjectItf  player;

    /* Interfaces for the audio player */
    SLPlayItf                     playItf;
    /*   to retrieve the PCM samples */
    SLAndroidSimpleBufferQueueItf decBuffQueueItf;
    /*   for prefetch status */
    SLPrefetchStatusItf           prefetchItf;
    /*   for seek */
    SLSeekItf                     seekItf;
    /*   for retrieving meta data */
    SLMetadataExtractionItf       metaItf;

    SLboolean required[NUM_EXPLICIT_INTERFACES_FOR_PLAYER];
    SLInterfaceID iidArray[NUM_EXPLICIT_INTERFACES_FOR_PLAYER];

    /* Initialize arrays required[] and iidArray[] */
    unsigned int i;
    for (i=0 ; i < NUM_EXPLICIT_INTERFACES_FOR_PLAYER ; i++) {
        required[i] = SL_BOOLEAN_FALSE;
        iidArray[i] = SL_IID_NULL;
    }

    /* ------------------------------------------------------ */
    /* Configuration of the player  */

    /* Request the AndroidSimpleBufferQueue interface */
    required[0] = SL_BOOLEAN_TRUE;
    iidArray[0] = SL_IID_ANDROIDSIMPLEBUFFERQUEUE;
    /* Request the PrefetchStatus interface */
    required[1] = SL_BOOLEAN_TRUE;
    iidArray[1] = SL_IID_PREFETCHSTATUS;
    /* Request the Seek interface */
    required[2] = SL_BOOLEAN_TRUE;
    iidArray[2] = SL_IID_SEEK;
    /* Request the MetadataExtraction interface */
    required[3] = SL_BOOLEAN_TRUE;
    iidArray[3] = SL_IID_METADATAEXTRACTION;

    SLDataSource decSource;

    AAsset* asset = NULL;

    SLDataFormat_MIME format_srcMime = { SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED };
    /* Setup the data source for URI if file_name starts with full path "/" */
    if (strncmp(internal->optional_file_name, "/", 1) == 0) {
        SLDataLocator_URI loc_src = {
            SL_DATALOCATOR_URI, ((SLchar *)internal->optional_file_name)
        };
        decSource.pLocator = &loc_src;
        decSource.pFormat   = &format_srcMime;
    } else {
        /* Setup the data source for Asset if file_name is relative path */
        AAssetManager* asmgr = asset_manager;
        if (asmgr == NULL) {
            SNDDBG("ALmixer_GetAssetManager failed");
            return(0);
        }
        asset = AAssetManager_open(asmgr, internal->optional_file_name, AASSET_MODE_UNKNOWN);
        if (asset == NULL) {
            SNDDBG("AAssetManager_open failed");
            return(0);
        }

        off_t start, length;
        int fd = AAsset_openFileDescriptor(asset, &start, &length);
        if (fd < 0) {
            SNDDBG("AAsset_openFileDescriptor failed");
            return(0);
        }

        SLDataLocator_AndroidFD loc_src = {SL_DATALOCATOR_ANDROIDFD, fd, start, length};
        decSource.pLocator = &loc_src;
        decSource.pFormat   = &format_srcMime;
    }


    /* Setup the data sink, a buffer queue for buffers of PCM data */
    SLDataLocator_AndroidSimpleBufferQueue loc_destBq = {
            SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE/*locatorType*/,
            NB_BUFFERS_IN_QUEUE               /*numBuffers*/ };

    /*    declare we're decoding to PCM, the parameters after that need to be valid,
          but are ignored, the decoded format will match the source */
    SLDataFormat_PCM format_destPcm = { /*formatType*/ SL_DATAFORMAT_PCM, /*numChannels*/ 1,
            /*samplesPerSec*/ SL_SAMPLINGRATE_8, /*pcm.bitsPerSample*/ SL_PCMSAMPLEFORMAT_FIXED_16,
            /*/containerSize*/ 16, /*channelMask*/ SL_SPEAKER_FRONT_LEFT,
            /*endianness*/ SL_BYTEORDER_LITTLEENDIAN };
    SLDataSink decDest = {&loc_destBq /*pLocator*/, &format_destPcm /*pFormat*/};

    /* Create the audio player */
    result = (*slEngineItf)->CreateAudioPlayer(slEngineItf, &player, &decSource, &decDest,
            NUM_EXPLICIT_INTERFACES_FOR_PLAYER, iidArray, required);
    if (SL_RESULT_SUCCESS != result) {
        SNDDBG("Failed to create OpenSLES audio player");
        return(0);
    } else {
        SNDDBG("Create OpenSLES audio player - OK");
    }

    /* Realize the player in synchronous mode. */
    result = (*player)->Realize(player, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result) {
        SNDDBG("Failed to realize OpenSLES audio player");
        return(0);
    } else {
        SNDDBG("Realize OpenSLES audio player - OK");
    }

    /* Get the play interface which is implicit */
    result = (*player)->GetInterface(player, SL_IID_PLAY, (void*)&playItf);
    if (SL_RESULT_SUCCESS != result) {
        SNDDBG("Failed to get OpenSLES audio player interface");
        return(0);
    } else {
        SNDDBG("Get OpenSLES audio player interface - OK");
    }

    /* Get the buffer queue interface which was explicitly requested */
    result = (*player)->GetInterface(player, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, (void*)&decBuffQueueItf);
    if (SL_RESULT_SUCCESS != result) {
        SNDDBG("Failed to get OpenSLES decoder buffer queue interface");
        return(0);
    } else {
        SNDDBG("Get OpenSLES decoder buffer queue interface - OK");
    }

    /* Get the prefetch status interface which was explicitly requested */
    result = (*player)->GetInterface(player, SL_IID_PREFETCHSTATUS, (void*)&prefetchItf);
    if (SL_RESULT_SUCCESS != result) {
        SNDDBG("Failed to get OpenSLES prefetch interface");
        return(0);
    } else {
        SNDDBG("Get OpenSLES prefetch interface - OK");
    }

    /* Get the seek interface which was explicitly requested */
    result = (*player)->GetInterface(player, SL_IID_SEEK, (void*)&seekItf);
    if (SL_RESULT_SUCCESS != result) {
        SNDDBG("Failed to get OpenSLES seek interface\n");
        return(0);
    } else {
        SNDDBG("Get OpenSLES seek interface - OK\n");
    }

    /* Get the metadata extraction interface which was explicitly requested */
    result = (*player)->GetInterface(player, SL_IID_METADATAEXTRACTION, (void*)&metaItf);
    if (SL_RESULT_SUCCESS != result) {
        SNDDBG("Failed to get OpenSLES MetadataExtraction interface\n");
        return(0);
    } else {
        SNDDBG("Get OpenSLES MetadataExtraction interface - OK\n");
    }
  
    /* ------------------------------------------------------ */
    /* Initialize the callback and its context for the buffer queue of the decoded PCM */
    OpenSLESFileContainer *file_container = (OpenSLESFileContainer *)malloc(sizeof(OpenSLESFileContainer));
    file_container->player = player;
    file_container->playItf = playItf;
    file_container->seekItf = seekItf;
    file_container->decBuffQueueItf = decBuffQueueItf;
    file_container->available = SL_BOOLEAN_FALSE;
    file_container->eos       = SL_BOOLEAN_FALSE;
    file_container->decode_waiting = SL_BOOLEAN_FALSE;
    file_container->asset = asset;
    file_container->shouldReturnInitialBuffer = SL_BOOLEAN_FALSE;
    file_container->initialBuffer = (int8_t*)malloc(BUFFER_SIZE_IN_BYTES);
    file_container->initialBufferDone = SL_BOOLEAN_FALSE;

    pthread_mutex_init(&file_container->prefetch_mutex, NULL);
    pthread_mutex_init(&file_container->decoder_mutex, NULL);
    pthread_cond_init(&file_container->prefetch_cond, NULL);
    pthread_cond_init(&file_container->decoder_cond, NULL);
    file_container->prefetch_status = PREFETCHSTATUS_UNKNOWN;
    internal->decoder_private = file_container;

    /* ------------------------------------------------------ */
    /* allocate memory to receive the PCM format metadata */
    file_container->metadata = (SLMetadataInfo*)malloc(PCM_METADATA_VALUE_SIZE);
    file_container->formatQueried = SL_BOOLEAN_FALSE;
    file_container->metaItf = metaItf;
    file_container->sampleRateKeyIndex   = -1;
    file_container->channelCountKeyIndex = -1;
    /* ------------------------------------------------------ */

    /* Enqueue buffers to map the region of memory allocated to store the decoded data */
    file_container->dstDataBase = (int8_t*)malloc(BUFFER_SIZE_IN_BYTES * NB_BUFFERS_IN_QUEUE);
    file_container->dstData = file_container->dstDataBase;
    SNDDBG("Enqueueing initial empty buffers to receive decoded PCM data");
    for(i = 0 ; i < NB_BUFFERS_IN_QUEUE ; i++) {
        result = (*decBuffQueueItf)->Enqueue(decBuffQueueItf, file_container->dstData, BUFFER_SIZE_IN_BYTES);
        file_container->dstData += BUFFER_SIZE_IN_BYTES;
    }
    file_container->dstData = file_container->dstDataBase;

    result = (*decBuffQueueItf)->RegisterCallback(decBuffQueueItf, decodePlayCallback, sample);
    if (SL_RESULT_SUCCESS != result) {
        SNDDBG("Failed to register OpenSLES decode buffer callback");
        return(0);
    } else {
        SNDDBG("Register OpenSLES decode buffer callback - OK");
    }

    /* ------------------------------------------------------ */
    /* Set up the player callback to get events during the decoding */
    result = (*playItf)->SetMarkerPosition(playItf, 2000);
    result = (*playItf)->SetPositionUpdatePeriod(playItf, 500);
    result = (*playItf)->SetCallbackEventsMask(playItf, SL_PLAYEVENT_HEADATEND);
    result = (*playItf)->RegisterCallback(playItf, decodeProgressCallback, file_container);

    /* ------------------------------------------------------ */
    /* Initialize the callback for prefetch errors, if we can't open the resource to decode */
    result = (*prefetchItf)->RegisterCallback(prefetchItf, prefetchEventCallback, file_container);
    if (SL_RESULT_SUCCESS != result) {
        SNDDBG("Failed to register OpenSLES prefetch event callback");
        return(0);
    } else {
        SNDDBG("Register OpenSLES prefetch event callback - OK");
    }

    result = (*prefetchItf)->SetCallbackEventsMask(prefetchItf, PREFETCHEVENT_ERROR_CANDIDATE);
    if (SL_RESULT_SUCCESS != result) {
        SNDDBG("Failed to set OpenSLES callback event mask for prefetch");
        return(0);
    } else {
        SNDDBG("Set OpenSLES callback event mask for prefetch - OK");
    }

    // set the player's state to paused, to start prefetching
    SNDDBG("Setting play state to PAUSED");
    result = (*playItf)->SetPlayState(playItf, SL_PLAYSTATE_PAUSED);

    // wait for prefetch status callback to indicate either sufficient data or error
    SNDDBG("Awaiting prefetch complete");
    pthread_mutex_lock(&file_container->prefetch_mutex);
    while (file_container->prefetch_status == PREFETCHSTATUS_UNKNOWN) {
        pthread_cond_wait(&file_container->prefetch_cond, &file_container->prefetch_mutex);
    }
    pthread_mutex_unlock(&file_container->prefetch_mutex);
    if (file_container->prefetch_status == PREFETCHSTATUS_ERROR) {
        SNDDBG("Error during prefetch, exiting");
        return(0);
    }
    SNDDBG("Prefetch is complete");


    /* ------------------------------------------------------ */
    /* Display the metadata obtained from the decoder */
    SLuint32 itemCount;
    result = (*metaItf)->GetItemCount(metaItf, &itemCount);
    SLuint32 keySize, valueSize;
    SLMetadataInfo *keyInfo, *value;
    for(i=0 ; i<itemCount ; i++) {
        keyInfo = NULL; keySize = 0;
        value = NULL;   valueSize = 0;
        result = (*metaItf)->GetKeySize(metaItf, i, &keySize);
        result = (*metaItf)->GetValueSize(metaItf, i, &valueSize);
        keyInfo = (SLMetadataInfo*) malloc(keySize);
        if (NULL != keyInfo) {
            result = (*metaItf)->GetKey(metaItf, i, keySize, keyInfo);
            /* find out the key index of the metadata we're interested in */
            if (!strcmp((char*)keyInfo->data, ANDROID_KEY_PCMFORMAT_NUMCHANNELS)) {
                file_container->channelCountKeyIndex = i;
            } else if (!strcmp((char*)keyInfo->data, ANDROID_KEY_PCMFORMAT_SAMPLERATE)) {
                file_container->sampleRateKeyIndex = i;
            }
            free(keyInfo);
        }
    }
    if (file_container->channelCountKeyIndex == -1) {
        SNDDBG("Unable to find key %s", ANDROID_KEY_PCMFORMAT_NUMCHANNELS);
    }
    if (file_container->sampleRateKeyIndex == -1) {
        SNDDBG("Unable to find key %s", ANDROID_KEY_PCMFORMAT_SAMPLERATE);
    }

    /* ------------------------------------------------------ */
    /* Start decoding */
    SNDDBG("Starting to decode");
    result = (*playItf)->SetPlayState(playItf, SL_PLAYSTATE_PLAYING);
    if (SL_RESULT_SUCCESS != result) {
        SNDDBG("Failed to set OpenSLES play state to SL_PLAYSTATE_PLAYING");
        return(0);
    }

    return(1);
}

static SLresult OpenSLES_updateTotalTime(Sound_Sample *sample) {
    SLmillisecond msec = SL_TIME_UNKNOWN;
    Sound_SampleInternal *internal = (Sound_SampleInternal *)sample->opaque;
    OpenSLESFileContainer *file_container = (OpenSLESFileContainer *)internal->decoder_private;
    SLresult result = (*file_container->playItf)->GetPosition(file_container->playItf, &msec);
    if (result == SL_RESULT_SUCCESS && msec != SL_TIME_UNKNOWN) {
        internal->total_time = msec;
    }
    return result;
}

static size_t OpenSLES_read(Sound_Sample *sample) {
    Sound_SampleInternal *internal = (Sound_SampleInternal *)sample->opaque;
    OpenSLESFileContainer *file_container = (OpenSLESFileContainer *)internal->decoder_private;
    
    if (file_container->eos == SL_BOOLEAN_TRUE) {
        OpenSLES_updateTotalTime(sample);
        SNDDBG("[EOS] total time=%d", internal->total_time);
        sample->flags |= SOUND_SAMPLEFLAG_EOF;
        return  0;
    }

    pthread_mutex_lock(&file_container->decoder_mutex);
    while(file_container->available == SL_BOOLEAN_FALSE && file_container->eos == SL_BOOLEAN_FALSE) {
        pthread_cond_wait(&file_container->decoder_cond, &file_container->decoder_mutex);
    }

    // Update total time
    OpenSLES_updateTotalTime(sample);

    // Update buffer
    if (file_container->initialBufferDone == SL_BOOLEAN_FALSE) {
        memcpy(file_container->initialBuffer, file_container->dstData, BUFFER_SIZE_IN_BYTES);
        file_container->initialBufferDone = SL_BOOLEAN_TRUE;
    }
    if (file_container->shouldReturnInitialBuffer == SL_BOOLEAN_TRUE) {
        memcpy(internal->buffer, file_container->initialBuffer, BUFFER_SIZE_IN_BYTES);
        file_container->shouldReturnInitialBuffer = SL_BOOLEAN_FALSE;
    } else {
        memcpy(internal->buffer, file_container->dstData, BUFFER_SIZE_IN_BYTES);
    }

    file_container->available = SL_BOOLEAN_FALSE;
    pthread_cond_signal(&file_container->decoder_cond);
    pthread_mutex_unlock(&file_container->decoder_mutex);

    SNDDBG("OpenSLES_read %d bytes", BUFFER_SIZE_IN_BYTES);

    return(BUFFER_SIZE_IN_BYTES);
}

static void OpenSLES_close(Sound_Sample *sample) {
    SNDDBG("OpenSLES_close");
    Sound_SampleInternal *internal = (Sound_SampleInternal *)sample->opaque;
    OpenSLESFileContainer *file_container = (OpenSLESFileContainer *)internal->decoder_private;

    SignalEos(file_container);

    pthread_mutex_lock(&file_container->decoder_mutex);
    // Waiting for decoder
    while(file_container->decode_waiting == SL_BOOLEAN_TRUE) {
        pthread_cond_wait(&file_container->decoder_cond, &file_container->decoder_mutex);
    }
    pthread_mutex_unlock(&file_container->decoder_mutex);

    pthread_cond_destroy(&file_container->decoder_cond);
    pthread_mutex_destroy(&file_container->decoder_mutex);

    if (internal->decoder_private != NULL) {
        if (file_container->player != NULL) {
            (*file_container->playItf)->SetPlayState(file_container->playItf, SL_PLAYSTATE_STOPPED);
            (*file_container->player)->Destroy(file_container->player);
        }
        if (file_container->asset != NULL) {
            AAsset_close(file_container->asset);
            file_container->asset = NULL;
        }
        if (file_container->metadata != NULL) {
            free(file_container->metadata);
            file_container->metadata = NULL;
        }
        if (file_container->dstDataBase != NULL) {
            free(file_container->dstDataBase);
            file_container->dstDataBase = NULL;
        }
        if (file_container->initialBuffer != NULL) {
            free(file_container->initialBuffer);
            file_container->initialBuffer = NULL;
        }
        free(file_container);
        internal->decoder_private = NULL;
    }
}

static int OpenSLES_rewind(Sound_Sample *sample) {
    SNDDBG("OpenSLES_rewind");
    Sound_SampleInternal *internal = (Sound_SampleInternal *)sample->opaque;
    OpenSLESFileContainer *file_container = (OpenSLESFileContainer *)internal->decoder_private;

    (*file_container->playItf)->SetPlayState(file_container->playItf, SL_PLAYSTATE_STOPPED);

    file_container->shouldReturnInitialBuffer = SL_BOOLEAN_TRUE;
    file_container->available = SL_BOOLEAN_FALSE;
    file_container->eos       = SL_BOOLEAN_FALSE;
    file_container->decode_waiting = SL_BOOLEAN_FALSE;

    (*file_container->seekItf)->SetPosition(file_container->seekItf, 0, SL_SEEKMODE_ACCURATE);
    (*file_container->playItf)->SetPlayState(file_container->playItf, SL_PLAYSTATE_PLAYING);

    return(1);
}

static int OpenSLES_seek(Sound_Sample *sample, size_t ms) {
    Sound_SampleInternal *internal = (Sound_SampleInternal *)sample->opaque;
    OpenSLESFileContainer *file_container = (OpenSLESFileContainer *)internal->decoder_private;
    SLresult result = (*file_container->seekItf)->SetPosition(file_container->seekItf, ms, SL_SEEKMODE_ACCURATE);

    if (result != SL_RESULT_SUCCESS) {
        sample->flags |= SOUND_SAMPLEFLAG_ERROR;
    }

    return(1);
}

#endif /* __ANDROID__ */


#endif /* ALMIXER_COMPILE_WITHOUT_SDL */

