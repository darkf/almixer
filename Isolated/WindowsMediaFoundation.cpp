

#ifdef ALMIXER_COMPILE_WITHOUT_SDL

/*
 * Windows Media Foundation backend
 * Copyright (C) 2012 Eric Wing <ewing . public @ playcontrol.net>
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

#ifdef _WIN32

#define WINVER _WIN32_WINNT_WIN7

#include <windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <stdio.h>
#include <mferror.h>



#include <stddef.h> /* NULL */
#include <stdio.h> /* printf */


#include "SoundDecoder.h"

#include "SoundDecoder_Internal.h"

template <class T> void SafeRelease(T **ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

typedef struct MediaFoundationFileContainer
{
	IMFSourceReader* pReader;
//	AudioFileID* audioFileID;
	//ExtAudioFileRef extAudioFileRef;
	//AudioStreamBasicDescription* outputFormat;
			IMFMediaType *pUncompressedAudioType;
		IMFMediaType *pPartialType;
		IMFMediaType *ppPCMAudio;

} MediaFoundationFileContainer;

static int MediaFoundation_init(void);
static void MediaFoundation_quit(void);
static int MediaFoundation_open(Sound_Sample *sample, const char *ext);
static void MediaFoundation_close(Sound_Sample *sample);
static size_t MediaFoundation_read(Sound_Sample *sample);
static int MediaFoundation_rewind(Sound_Sample *sample);
static int MediaFoundation_seek(Sound_Sample *sample, size_t ms);

static const char *extensions_MediaFoundation[] = 
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
const Sound_DecoderFunctions __Sound_DecoderFunctions_MediaFoundation =
{
    {
        extensions_MediaFoundation,
        "Decode audio through Core Audio through",
        "Eric Wing <ewing . public @ playcontrol.net>",
        "http://playcontrol.net"
    },
	
    MediaFoundation_init,       /*   init() method */
    MediaFoundation_quit,       /*   quit() method */
    MediaFoundation_open,       /*   open() method */
    MediaFoundation_close,      /*  close() method */
    MediaFoundation_read,       /*   read() method */
    MediaFoundation_rewind,     /* rewind() method */
    MediaFoundation_seek        /*   seek() method */
};


static int MediaFoundation_init(void)
{
    HRESULT hr = S_OK;
	hr = MFStartup(MF_VERSION, 0);
	if(S_OK == hr)
	{
		return 1;
	}
	else
	{
		return 0;
	}	
} /* MediaFoundation_init */


static void MediaFoundation_quit(void)
{
    MFShutdown();
} /* MediaFoundation_quit */

/*
   http://developer.apple.com/library/ios/#documentation/MusicAudio/Reference/AudioFileConvertRef/Reference/reference.html
   kAudioFileAIFFType = 'AIFF',
   kAudioFileAIFCType            = 'AIFC',
   kAudioFileWAVEType            = 'WAVE',
   kAudioFileSoundDesigner2Type  = 'Sd2f',
   kAudioFileNextType            = 'NeXT',
   kAudioFileMP3Type             = 'MPG3',
   kAudioFileMP2Type             = 'MPG2',
   kAudioFileMP1Type             = 'MPG1',
   kAudioFileAC3Type             = 'ac-3',
   kAudioFileAAC_ADTSType        = 'adts',
   kAudioFileMPEG4Type           = 'mp4f',
   kAudioFileM4AType             = 'm4af',
   kAudioFileCAFType             = 'caff',
   kAudioFile3GPType             = '3gpp',
   kAudioFile3GP2Type            = '3gp2',
   kAudioFileAMRType             = 'amrf'
*/
#if 0
static AudioFileTypeID MediaFoundation_GetAudioTypeForExtension(const char* file_extension)
{
	if( (__Sound_strcasecmp(file_extension, "aif") == 0)
		|| (__Sound_strcasecmp(file_extension, "aiff") == 0)
		|| (__Sound_strcasecmp(file_extension, "aifc") == 0)
	)
	{
		return kAudioFileAIFCType;
	}
	else if( (__Sound_strcasecmp(file_extension, "wav") == 0)
		|| (__Sound_strcasecmp(file_extension, "wave") == 0)
	)
	{
		return kAudioFileWAVEType;
	}
	else if( (__Sound_strcasecmp(file_extension, "mp3") == 0)
	)
	{
		return kAudioFileMP3Type;
	}
	else if( (__Sound_strcasecmp(file_extension, "mp4") == 0)
	)
	{
		return kAudioFileMPEG4Type;
	}
	else if( (__Sound_strcasecmp(file_extension, "m4a") == 0)
	)
	{
		return kAudioFileM4AType;
	}
	else if( (__Sound_strcasecmp(file_extension, "aac") == 0)
	)
	{
		return kAudioFileAAC_ADTSType;
	}
	else if( (__Sound_strcasecmp(file_extension, "adts") == 0)
	)
	{
		return kAudioFileAAC_ADTSType;
	}
	else if( (__Sound_strcasecmp(file_extension, "caf") == 0)
		|| (__Sound_strcasecmp(file_extension, "caff") == 0)
	)
	{
		return kAudioFileCAFType;
	}
	else if( (__Sound_strcasecmp(file_extension, "Sd2f") == 0)
		|| (__Sound_strcasecmp(file_extension, "sd2") == 0)
	)
	{
		return kAudioFileSoundDesigner2Type;
	}
	else if( (__Sound_strcasecmp(file_extension, "au") == 0)
		|| (__Sound_strcasecmp(file_extension, "snd") == 0)
		|| (__Sound_strcasecmp(file_extension, "next") == 0)
	)
	{
		return kAudioFileNextType;
	}
	else if( (__Sound_strcasecmp(file_extension, "mp2") == 0)
	)
	{
		return kAudioFileMP2Type;
	}
	else if( (__Sound_strcasecmp(file_extension, "mp1") == 0)
	)
	{
		return kAudioFileMP1Type;
	}
	else if( (__Sound_strcasecmp(file_extension, "ac3") == 0)
	)
	{
		return kAudioFileAC3Type;
	}
	else if( (__Sound_strcasecmp(file_extension, "3gpp") == 0)
			|| (__Sound_strcasecmp(file_extension, "3gp") == 0)
	)
	{
		return kAudioFile3GPType;
	}
	else if( (__Sound_strcasecmp(file_extension, "3gp2") == 0)
			|| (__Sound_strcasecmp(file_extension, "3g2") == 0)
	)
	{
		return kAudioFile3GP2Type;
	}
	else if( (__Sound_strcasecmp(file_extension, "amrf") == 0)
		|| (__Sound_strcasecmp(file_extension, "amr") == 0)
	)
	{
		return kAudioFileAMRType;
	}
	else if( (__Sound_strcasecmp(file_extension, "ima4") == 0)
		|| (__Sound_strcasecmp(file_extension, "ima") == 0)
	)
	{
		/* not sure about this one */
		return kAudioFileCAFType;
	}
	else
	{
		return 0;
	}

}
#endif


/*

SInt64 MediaFoundation_SizeCallback(void* inClientData)
{
	ALmixer_RWops* rw_ops = (ALmixer_RWops*)inClientData;
	SInt64 current_position = ALmixer_RWtell(rw_ops);
	SInt64 end_position = ALmixer_RWseek(rw_ops, 0, SEEK_END);
	ALmixer_RWseek(rw_ops, current_position, SEEK_SET);
//	fprintf(stderr, "MediaFoundation_SizeCallback:%d\n", end_position);

	return end_position;
}
*/
/*
OSStatus MediaFoundation_ReadCallback(
	void* inClientData,
	SInt64 inPosition,
	UInt32 requestCount,
	void* data_buffer,
	UInt32* actualCount
)
{
	ALmixer_RWops* rw_ops = (ALmixer_RWops*)inClientData;
	ALmixer_RWseek(rw_ops, inPosition, SEEK_SET);
	size_t bytes_actually_read = ALmixer_RWread(rw_ops, data_buffer, 1, requestCount);
	// Not sure how to test for a read error with ALmixer_RWops
//	fprintf(stderr, "MediaFoundation_ReadCallback:%d, %d\n", requestCount, bytes_actually_read);

	*actualCount = bytes_actually_read;
	return noErr;
}
*/

static int MediaFoundation_open(Sound_Sample *sample, const char *ext)
{
	MediaFoundationFileContainer* media_foundation_file_container;
	Sound_SampleInternal *internal = (Sound_SampleInternal *) sample->opaque;
	//Float64 estimated_duration;
	//UInt32 format_size;
	HRESULT hresult;
	IMFSourceReader* source_reader = NULL;
    const WCHAR* source_file = L"C:\\Users\\pinky\\Documents\\Source\\HG\\BUILD_ALMIXER\\Debug\\pcm1644m.wav";
	
	media_foundation_file_container = (MediaFoundationFileContainer*)malloc(sizeof(MediaFoundationFileContainer));
	BAIL_IF_MACRO(media_foundation_file_container == NULL, ERR_OUT_OF_MEMORY, 0);

//hresult = MFCreateSourceReaderFromByteStream();
	hresult = MFCreateSourceReaderFromURL(source_file, NULL, &source_reader);
	if (FAILED(hresult))
    {
		fprintf(stderr, "Error opening input file: %S\n", source_file, hresult);
	}
	
	
	{
		IMFSourceReader* pReader = source_reader;
//	HRESULT hr = hresult;
		IMFMediaType *pUncompressedAudioType = NULL;
		IMFMediaType *pPartialType = NULL;
		IMFMediaType *ppPCMAudio;

		// Select the first audio stream, and deselect all other streams.
		HRESULT hr = pReader->SetStreamSelection(
			(DWORD)MF_SOURCE_READER_ALL_STREAMS, FALSE);

		if (SUCCEEDED(hr))
		{
			hr = pReader->SetStreamSelection(
				(DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, TRUE);
		}

		// Create a partial media type that specifies uncompressed PCM audio.
		hr = MFCreateMediaType(&pPartialType);

		if (SUCCEEDED(hr))
		{
			hr = pPartialType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
		}

		if (SUCCEEDED(hr))
		{
			hr = pPartialType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
		}

		// Set this type on the source reader. The source reader will
		// load the necessary decoder.
		if (SUCCEEDED(hr))
		{
			hr = pReader->SetCurrentMediaType(
				(DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM,
				NULL, pPartialType);
		}

		// Get the complete uncompressed format.
		if (SUCCEEDED(hr))
		{
			hr = pReader->GetCurrentMediaType(
				(DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM,
				&pUncompressedAudioType);
		}

		// Ensure the stream is selected.
		if (SUCCEEDED(hr))
		{
			hr = pReader->SetStreamSelection(
				(DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM,
				TRUE);
		}

		// Return the PCM format to the caller.
		if (SUCCEEDED(hr))
		{
			ppPCMAudio = pUncompressedAudioType;
			ppPCMAudio->AddRef();
		}
			media_foundation_file_container->ppPCMAudio = ppPCMAudio;

		SafeRelease(&pUncompressedAudioType);
		SafeRelease(&pPartialType);
	}
		
	
#if 0	
	audio_file_id = (AudioFileID*)malloc(sizeof(AudioFileID));
	BAIL_IF_MACRO(audio_file_id == NULL, ERR_OUT_OF_MEMORY, 0);

	error_result = AudioFileOpenWithCallbacks(
		internal->rw,
		MediaFoundation_ReadCallback,
		NULL,
		MediaFoundation_SizeCallback,
		NULL,
		MediaFoundation_GetAudioTypeForExtension(ext),
		audio_file_id
	);
	if (error_result != noErr)
	{
		AudioFileClose(*audio_file_id);
		free(audio_file_id);
		free(media_foundation_file_container);
		SNDDBG(("Core Audio: can't grok data. reason: [%s].\n", MediaFoundation_FourCCToString(error_result)));
		BAIL_MACRO("Core Audio: Not valid audio data.", 0);
	} /* if */
	
    format_size = sizeof(actual_format);
    error_result = AudioFileGetProperty(
		*audio_file_id,
		kAudioFilePropertyDataFormat,
		&format_size,
		&actual_format
	);
    if (error_result != noErr)
	{
		AudioFileClose(*audio_file_id);
		free(audio_file_id);
		free(media_foundation_file_container);
		SNDDBG(("Core Audio: AudioFileGetProperty failed. reason: [%s]", MediaFoundation_FourCCToString(error_result)));
		BAIL_MACRO("Core Audio: Not valid audio data.", 0);
	} /* if */

    format_size = sizeof(estimated_duration);
    error_result = AudioFileGetProperty(
		*audio_file_id,
		kAudioFilePropertyEstimatedDuration,
		&format_size,
		&estimated_duration
	);
    if (error_result != noErr)
	{
		AudioFileClose(*audio_file_id);
		free(audio_file_id);
		free(media_foundation_file_container);
		SNDDBG(("Core Audio: AudioFileGetProperty failed. reason: [%s].\n", MediaFoundation_FourCCToString(error_result)));
		BAIL_MACRO("Core Audio: Not valid audio data.", 0);
	} /* if */


	media_foundation_file_container->audioFileID = audio_file_id;
	
	
#endif
	internal->decoder_private = media_foundation_file_container;
/*	
	sample->flags = SOUND_SAMPLEFLAG_CANSEEK;
	sample->actual.rate = (UInt32) actual_format.mSampleRate;
	sample->actual.channels = (UInt8)actual_format.mChannelsPerFrame;
	internal->total_time = (SInt32)(estimated_duration * 1000.0 + 0.5);
*/
	sample->flags = SOUND_SAMPLEFLAG_CANSEEK;
	sample->actual.rate = (uint32_t) 44100;
	sample->actual.channels = (uint8_t)1;
	internal->total_time = (int32_t)(7 * 1000.0 + 0.5);

#if 0

#else
	
	
	
    /*
     * I want to use Core Audio to do conversion and decoding for performance reasons.
	 * This is particularly important on mobile devices like iOS.
	 * Taking from the Ogg Vorbis decode, I pretend the "actual" format is the same 
	 * as the desired format. 
     */
    sample->actual.format = (sample->desired.format == 0) ?
	AUDIO_S16SYS : sample->desired.format;
#endif	


	SNDDBG(("MediaFoundation: channels == (%d).\n", sample->actual.channels));
	SNDDBG(("MediaFoundation: sampling rate == (%d).\n",sample->actual.rate));
	SNDDBG(("MediaFoundation: total seconds of sample == (%d).\n", internal->total_time));
	SNDDBG(("MediaFoundation: sample->actual.format == (%d).\n", sample->actual.format));


#if 0	
	error_result = ExtAudioFileWrapAudioFileID(*audio_file_id,
		false, // set to false for read-only
		&media_foundation_file_container->extAudioFileRef
	);
	if(error_result != noErr)
	{
		AudioFileClose(*audio_file_id);
		free(audio_file_id);
		free(media_foundation_file_container);
		SNDDBG(("Core Audio: can't wrap data. reason: [%s].\n", MediaFoundation_FourCCToString(error_result)));
		BAIL_MACRO("Core Audio: Failed to wrap data.", 0);
	} /* if */


	/* The output format must be linear PCM because that's the only type OpenAL knows how to deal with.
	 * Set the client format to 16 bit signed integer (native-endian) data because that is the most
	 * optimal format on iPhone/iPod Touch hardware.
	 * Maintain the channel count and sample rate of the original source format.
	 */
	output_format.mSampleRate = actual_format.mSampleRate; // preserve the original sample rate
	output_format.mChannelsPerFrame = actual_format.mChannelsPerFrame; // preserve the number of channels
	output_format.mFormatID = kAudioFormatLinearPCM; // We want linear PCM data
	output_format.mFramesPerPacket = 1; // We know for linear PCM, the definition is 1 frame per packet

	if(sample->desired.format == 0)
	{
		// do AUDIO_S16SYS
		output_format.mFormatFlags = kAudioFormatFlagsNativeEndian | kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked; // I seem to read failures problems without kAudioFormatFlagIsPacked. From a mailing list post, this seems to be a Core Audio bug.
		output_format.mBitsPerChannel = 16; // We know we want 16-bit
	}
	else
	{
		output_format.mFormatFlags = 0; // clear flags
		output_format.mFormatFlags |= kAudioFormatFlagIsPacked; // I seem to read failures problems without kAudioFormatFlagIsPacked. From a mailing list post, this seems to be a Core Audio bug.
		// Mask against bitsize
		if(0xFF & sample->desired.format)
		{
			output_format.mBitsPerChannel = 16; /* 16-bit */
		}
		else
		{
			output_format.mBitsPerChannel = 8; /* 8-bit */
		}

		// Mask for signed/unsigned
		if((1<<15) & sample->desired.format)
		{
			output_format.mFormatFlags = output_format.mFormatFlags | kAudioFormatFlagIsSignedInteger;

		}
		else
		{
			// no flag set for unsigned
		}
		// Mask for big/little endian
		if((1<<12) & sample->desired.format)
		{
			output_format.mFormatFlags = output_format.mFormatFlags | kAudioFormatFlagIsBigEndian;
		}
		else
		{
			// no flag set for little endian 
		}
	}

	output_format.mBytesPerPacket = output_format.mBitsPerChannel/8 * output_format.mChannelsPerFrame; // e.g. 16-bits/8 * channels => so 2-bytes per channel per frame
	output_format.mBytesPerFrame = output_format.mBitsPerChannel/8 * output_format.mChannelsPerFrame; // For PCM, since 1 frame is 1 packet, it is the same as mBytesPerPacket

	
/*
	output_format.mSampleRate = actual_format.mSampleRate; // preserve the original sample rate
	output_format.mChannelsPerFrame = actual_format.mChannelsPerFrame; // preserve the number of channels
	output_format.mFormatID = kAudioFormatLinearPCM; // We want linear PCM data
//	output_format.mFormatFlags = kAudioFormatFlagsNativeEndian | kAudioFormatFlagIsPacked | kAudioFormatFlagIsSignedInteger;
	output_format.mFormatFlags = kAudioFormatFlagsNativeEndian |  kAudioFormatFlagIsSignedInteger;
	output_format.mFramesPerPacket = 1; // We know for linear PCM, the definition is 1 frame per packet
	output_format.mBitsPerChannel = 16; // We know we want 16-bit
	output_format.mBytesPerPacket = 2 * output_format.mChannelsPerFrame; // We know we are using 16-bit, so 2-bytes per channel per frame
	output_format.mBytesPerFrame = 2 * output_format.mChannelsPerFrame; // For PCM, since 1 frame is 1 packet, it is the same as mBytesPerPacket
*/
	SNDDBG(("output_format: mSampleRate: %lf\n", output_format.mSampleRate)); 
	SNDDBG(("output_format: mChannelsPerFrame: %d\n", output_format.mChannelsPerFrame)); 
	SNDDBG(("output_format: mFormatID: %d\n", output_format.mFormatID)); 
	SNDDBG(("output_format: mFormatFlags: %d\n", output_format.mFormatFlags)); 
	SNDDBG(("output_format: mFramesPerPacket: %d\n", output_format.mFramesPerPacket)); 
	SNDDBG(("output_format: mBitsPerChannel: %d\n", output_format.mBitsPerChannel)); 
	SNDDBG(("output_format: mBytesPerPacket: %d\n", output_format.mBytesPerPacket)); 
	SNDDBG(("output_format: mBytesPerFrame: %d\n", output_format.mBytesPerFrame)); 
	
	
	/* Set the desired client (output) data format */
	error_result = ExtAudioFileSetProperty(media_foundation_file_container->extAudioFileRef, kExtAudioFileProperty_ClientDataFormat, sizeof(output_format), &output_format);
	if(noErr != error_result)
	{
		ExtAudioFileDispose(media_foundation_file_container->extAudioFileRef);
		AudioFileClose(*audio_file_id);
		free(audio_file_id);
		free(media_foundation_file_container);
		SNDDBG(("Core Audio: ExtAudioFileSetProperty(kExtAudioFileProperty_ClientDataFormat) failed, reason: [%s].\n", MediaFoundation_FourCCToString(error_result)));
		BAIL_MACRO("Core Audio: Not valid audio data.", 0);
	}	


	media_foundation_file_container->outputFormat = (AudioStreamBasicDescription*)malloc(sizeof(AudioStreamBasicDescription));
	BAIL_IF_MACRO(media_foundation_file_container->outputFormat == NULL, ERR_OUT_OF_MEMORY, 0);


	
	/* Copy the output format to the audio_description that was passed in so the 
	 * info will be returned to the user.
	 */
	memcpy(media_foundation_file_container->outputFormat, &output_format, sizeof(AudioStreamBasicDescription));

#endif	

	return(1);
} /* MediaFoundation_open */


static void MediaFoundation_close(Sound_Sample *sample)
{
	Sound_SampleInternal *internal = (Sound_SampleInternal *) sample->opaque;
	MediaFoundationFileContainer* media_foundation_file_container = (MediaFoundationFileContainer *) internal->decoder_private;

//	free(media_foundation_file_container->outputFormat);
/*	
	ExtAudioFileDispose(media_foundation_file_container->extAudioFileRef);
	AudioFileClose(*media_foundation_file_container->audioFileID);
	*/
//	free(media_foundation_file_container->audioFileID);
	free(media_foundation_file_container);
		
} /* MediaFoundation_close */



DWORD CalculateMaxAudioDataSize(
    IMFMediaType *pAudioType,    // The PCM audio format.
    DWORD cbHeader,              // The size of the WAVE file header.
    DWORD msecAudioData          // Maximum duration, in milliseconds.
    )
{
    UINT32 cbBlockSize = 0;         // Audio frame size, in bytes.
    UINT32 cbBytesPerSecond = 0;    // Bytes per second.

    // Get the audio block size and number of bytes/second from the audio format.

    cbBlockSize = MFGetAttributeUINT32(pAudioType, MF_MT_AUDIO_BLOCK_ALIGNMENT, 0);
    cbBytesPerSecond = MFGetAttributeUINT32(pAudioType, MF_MT_AUDIO_AVG_BYTES_PER_SECOND, 0);

    // Calculate the maximum amount of audio data to write.
    // This value equals (duration in seconds x bytes/second), but cannot
    // exceed the maximum size of the data chunk in the WAVE file.

        // Size of the desired audio clip in bytes:
    DWORD cbAudioClipSize = (DWORD)MulDiv(cbBytesPerSecond, msecAudioData, 1000);

    // Largest possible size of the data chunk:
    DWORD cbMaxSize = MAXDWORD - cbHeader;

    // Maximum size altogether.
    cbAudioClipSize = min(cbAudioClipSize, cbMaxSize);

    // Round to the audio block size, so that we do not write a partial audio frame.
    cbAudioClipSize = (cbAudioClipSize / cbBlockSize) * cbBlockSize;

    return cbAudioClipSize;
}


static size_t MediaFoundation_read(Sound_Sample *sample)
{
//	OSStatus error_result = noErr;	
	/* Documentation/example shows SInt64, but is problematic for big endian
	 * on 32-bit cast for ExtAudioFileRead() because it takes the upper
	 * bits which turn to 0.
	 */
	uint32_t buffer_size_in_frames = 0;
	uint32_t buffer_size_in_frames_remaining = 0;
	uint32_t total_frames_read = 0;
	uint32_t data_buffer_size = 0;
	uint32_t bytes_remaining = 0;
	size_t total_bytes_read = 0;
	Sound_SampleInternal *internal = (Sound_SampleInternal *) sample->opaque;
	MediaFoundationFileContainer* media_foundation_file_container = (MediaFoundationFileContainer *) internal->decoder_private;
	uint32_t max_buffer_size = internal->buffer_size;

    HRESULT hr = S_OK;
    DWORD cbAudioData = 0;
    DWORD cbBuffer = 0;
    BYTE *pAudioData = NULL;

    IMFSample *pSample = NULL;
    IMFMediaBuffer *pBuffer = NULL;

    // Get audio samples from the source reader.
    while (true)
    {
        DWORD dwFlags = 0;
		IMFSourceReader* pReader = media_foundation_file_container->pReader;
		      DWORD  cbMaxAudioData = CalculateMaxAudioDataSize(media_foundation_file_container->ppPCMAudio, 44, 6000);
        // Read the next sample.
        hr = pReader->ReadSample(
            (DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM,
            0, NULL, &dwFlags, NULL, &pSample );

        if (FAILED(hr)) { break; }

        if (dwFlags & MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED)
        {
            printf("Type change - not supported by WAVE file format.\n");
            break;
        }
        if (dwFlags & MF_SOURCE_READERF_ENDOFSTREAM)
        {
            printf("End of input file.\n");
            break;
        }

        if (pSample == NULL)
        {
            printf("No sample\n");
            continue;
        }

        // Get a pointer to the audio data in the sample.

        hr = pSample->ConvertToContiguousBuffer(&pBuffer);

        if (FAILED(hr)) { break; }


        hr = pBuffer->Lock(&pAudioData, NULL, &cbBuffer);

        if (FAILED(hr)) { break; }


        // Make sure not to exceed the specified maximum size.
        if (cbMaxAudioData - cbAudioData < cbBuffer)
        {
            cbBuffer = cbMaxAudioData - cbAudioData;
        }

        // Write this data to the output file.
      //  hr = WriteToFile(hFile, pAudioData, cbBuffer);

		{
			HRESULT hr = S_OK;
			DWORD cbAudioData = 0;
			DWORD cbBuffer = 0;
			BYTE *pAudioData = NULL;

			IMFSample *pSample = NULL;
			IMFMediaBuffer *pBuffer = NULL;

			// Get audio samples from the source reader.
			while (true)
			{
				DWORD dwFlags = 0;

				// Read the next sample.
				hr = pReader->ReadSample(
					(DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM,
					0, NULL, &dwFlags, NULL, &pSample );

				if (FAILED(hr)) { break; }

				if (dwFlags & MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED)
				{
					printf("Type change - not supported by WAVE file format.\n");
					break;
				}
				if (dwFlags & MF_SOURCE_READERF_ENDOFSTREAM)
				{
					printf("End of input file.\n");
					break;
				}

				if (pSample == NULL)
				{
					printf("No sample\n");
					continue;
				}

				// Get a pointer to the audio data in the sample.

				hr = pSample->ConvertToContiguousBuffer(&pBuffer);

				if (FAILED(hr)) { break; }


				hr = pBuffer->Lock(&pAudioData, NULL, &cbBuffer);

				if (FAILED(hr)) { break; }


				// Make sure not to exceed the specified maximum size.
				if (cbMaxAudioData - cbAudioData < cbBuffer)
				{
					cbBuffer = cbMaxAudioData - cbAudioData;
				}

				// Write this data to the output file.
//				hr = WriteToFile(hFile, pAudioData, cbBuffer);

				if (FAILED(hr)) { break; }

				// Unlock the buffer.
				hr = pBuffer->Unlock();
				pAudioData = NULL;

				if (FAILED(hr)) { break; }

				// Update running total of audio data.
				cbAudioData += cbBuffer;

				if (cbAudioData >= cbMaxAudioData)
				{
					break;
				}

				SafeRelease(&pSample);
				SafeRelease(&pBuffer);
			}

			if (SUCCEEDED(hr))
			{
				printf("Wrote %d bytes of audio data.\n", cbAudioData);

				*pcbDataWritten = cbAudioData;
			}

			if (pAudioData)
			{
				pBuffer->Unlock();
			}

			SafeRelease(&pBuffer);
			SafeRelease(&pSample);
		}

        if (FAILED(hr)) { break; }

        // Unlock the buffer.
        hr = pBuffer->Unlock();
        pAudioData = NULL;

        if (FAILED(hr)) { break; }

        // Update running total of audio data.
        cbAudioData += cbBuffer;

        if (cbAudioData >= cbMaxAudioData)
        {
            break;
        }

        SafeRelease(&pSample);
        SafeRelease(&pBuffer);
    }

    if (SUCCEEDED(hr))
    {
        printf("Wrote %d bytes of audio data.\n", cbAudioData);

        *pcbDataWritten = cbAudioData;
    }

    if (pAudioData)
    {
        pBuffer->Unlock();
    }

    SafeRelease(&pBuffer);
    SafeRelease(&pSample);
	
#if 0	
	
//	printf("internal->buffer_size=%d, internal->buffer=0x%x, sample->buffer_size=%d\n", internal->buffer_size, internal->buffer, sample->buffer_size); 
//	printf("internal->max_buffer_size=%d\n", max_buffer_size); 

	/* Compute how many frames will fit into our max buffer size */
	/* Warning: If this is not evenly divisible, the buffer will not be completely filled which violates the SDL_sound assumption. */
	buffer_size_in_frames = max_buffer_size / media_foundation_file_container->outputFormat->mBytesPerFrame;
//	printf("buffer_size_in_frames=%ld, internal->buffer_size=%d, internal->buffer=0x%x outputFormat->mBytesPerFrame=%d, sample->buffer_size=%d\n", buffer_size_in_frames, internal->buffer_size, internal->buffer, media_foundation_file_container->outputFormat->mBytesPerFrame, sample->buffer_size); 


//	void* temp_buffer = malloc(max_buffer_size);
	
	AudioBufferList audio_buffer_list;
	audio_buffer_list.mNumberBuffers = 1;
	audio_buffer_list.mBuffers[0].mDataByteSize = max_buffer_size;
	audio_buffer_list.mBuffers[0].mNumberChannels = media_foundation_file_container->outputFormat->mChannelsPerFrame;
	audio_buffer_list.mBuffers[0].mData = internal->buffer;


	bytes_remaining = max_buffer_size;
	buffer_size_in_frames_remaining = buffer_size_in_frames;
	
	// oops. Due to the kAudioFormatFlagIsPacked bug, 
	// I was misled to believe that Core Audio
	// was not always filling my entire requested buffer. 
	// So this while-loop might be unnecessary.
	// However, I have not exhaustively tested all formats, 
	// so maybe it is possible this loop is useful.
	// It might also handle the not-evenly disvisible case above.
	while(buffer_size_in_frames_remaining > 0 && !(sample->flags & SOUND_SAMPLEFLAG_EOF))
	{
		
		data_buffer_size = (UInt32)(buffer_size_in_frames * media_foundation_file_container->outputFormat->mBytesPerFrame);
//		printf("data_buffer_size=%d\n", data_buffer_size); 

		buffer_size_in_frames = buffer_size_in_frames_remaining;
		
//		printf("reading buffer_size_in_frames=%"PRId64"\n", buffer_size_in_frames); 


		audio_buffer_list.mBuffers[0].mDataByteSize = bytes_remaining;
		audio_buffer_list.mBuffers[0].mData = &(((UInt8*)internal->buffer)[total_bytes_read]);

		
		/* Read the data into an AudioBufferList */
		error_result = ExtAudioFileRead(media_foundation_file_container->extAudioFileRef, &buffer_size_in_frames, &audio_buffer_list);
		if(error_result == noErr)
		{
		
		
			/* Success */
			
			total_frames_read += buffer_size_in_frames;
			buffer_size_in_frames_remaining = buffer_size_in_frames_remaining - buffer_size_in_frames;
			
//			printf("read buffer_size_in_frames=%"PRId64", buffer_size_in_frames_remaining=%"PRId64"\n", buffer_size_in_frames, buffer_size_in_frames_remaining); 

			/* ExtAudioFileRead returns the number of frames actually read. Need to convert back to bytes. */
			data_buffer_size = (UInt32)(buffer_size_in_frames * media_foundation_file_container->outputFormat->mBytesPerFrame);
//			printf("data_buffer_size=%d\n", data_buffer_size); 

			total_bytes_read += data_buffer_size;
			bytes_remaining = bytes_remaining - data_buffer_size;

			/* Note: 0 == buffer_size_in_frames is a legitimate value meaning we are EOF. */
			if(0 == buffer_size_in_frames)
			{
				sample->flags |= SOUND_SAMPLEFLAG_EOF;			
			}

		}
		else 
		{
			SNDDBG(("Core Audio: ExtAudioFileReadfailed, reason: [%s].\n", MediaFoundation_FourCCToString(error_result)));

			sample->flags |= SOUND_SAMPLEFLAG_ERROR;
			break;
			
		}
	}
	
	if( (!(sample->flags & SOUND_SAMPLEFLAG_EOF)) && (total_bytes_read < max_buffer_size))
	{
		SNDDBG(("Core Audio: ExtAudioFileReadfailed SOUND_SAMPLEFLAG_EAGAIN, reason: [total_bytes_read < max_buffer_size], %d, %d.\n", total_bytes_read , max_buffer_size));
		
		/* Don't know what to do here. */
		sample->flags |= SOUND_SAMPLEFLAG_EAGAIN;
	}
#endif
	return total_bytes_read;
} /* MediaFoundation_read */


static int MediaFoundation_rewind(Sound_Sample *sample)
{
/*
	OSStatus error_result = noErr;	
	Sound_SampleInternal *internal = (Sound_SampleInternal *) sample->opaque;
	MediaFoundationFileContainer* media_foundation_file_container = (MediaFoundationFileContainer *) internal->decoder_private;
	
	error_result = ExtAudioFileSeek(media_foundation_file_container->extAudioFileRef, 0);
	if(error_result != noErr)
	{
		sample->flags |= SOUND_SAMPLEFLAG_ERROR;
	}
	*/
	return(1);
} /* MediaFoundation_rewind */

/* Note: I found this tech note:
 http://developer.apple.com/library/mac/#qa/qa2009/qa1678.html
 I don't know if this applies to us. So far, I haven't noticed the problem,
 so I haven't applied any of the techniques.
 */
static int MediaFoundation_seek(Sound_Sample *sample, size_t ms)
{
#if 0
	OSStatus error_result = noErr;	
	Sound_SampleInternal *internal = (Sound_SampleInternal *) sample->opaque;
	MediaFoundationFileContainer* media_foundation_file_container = (MediaFoundationFileContainer *) internal->decoder_private;
	SInt64 frame_offset = 0;

	/* I'm confused. The Apple documentation says this:
	 "Seek position is specified in the sample rate and frame count of the file’s audio data format
	 — not your application’s audio data format."
	 My interpretation is that I want to get the "actual format of the file and compute the frame offset.
	 But when I try that, seeking goes to the wrong place.
	 When I use outputFormat, things seem to work correctly.
	 I must be misinterpreting the documentation or doing something wrong.
	 */
#if 0 /* not working */
	AudioStreamBasicDescription	actual_format;
	UInt32 format_size;
    format_size = sizeof(AudioStreamBasicDescription);
    error_result = AudioFileGetProperty(
		*media_foundation_file_container->audioFileID,
		kAudioFilePropertyDataFormat,
		&format_size,
		&actual_format
	);
    if(error_result != noErr)
	{
		sample->flags |= SOUND_SAMPLEFLAG_ERROR;
		BAIL_MACRO("Core Audio: Could not GetProperty for kAudioFilePropertyDataFormat.", 0);
	} /* if */

	// packetIndex = (pos * sampleRate) / framesPerPacket
	frame_offset = (SInt64)((ms/1000.0 * actual_format.mSampleRate) / actual_format.mFramesPerPacket);
	// computed against actual format and not the client format

	// packetIndex = (pos * sampleRate) / framesPerPacket
	//	frame_offset = (SInt64)((ms/1000.0 * actual_format.mSampleRate) / actual_format.mFramesPerPacket);
#else /* seems to work, but I'm confused */
	// packetIndex = (pos * sampleRate) / framesPerPacket
	frame_offset = (SInt64)((ms/1000.0 * media_foundation_file_container->outputFormat->mSampleRate) / media_foundation_file_container->outputFormat->mFramesPerPacket);	
#endif
	
	// computed against actual format and not the client format
	error_result = ExtAudioFileSeek(media_foundation_file_container->extAudioFileRef, frame_offset);
	if(error_result != noErr)
	{
		sample->flags |= SOUND_SAMPLEFLAG_ERROR;
	}
#endif
	return(1);
} /* MediaFoundation_seek */

#endif /* __APPLE__ */


#endif /* ALMIXER_COMPILE_WITHOUT_SDL */

