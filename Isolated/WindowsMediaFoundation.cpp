

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

/*
#include <iostream>
#include <string.h>
#include <windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mferror.h>
#include <assert.h>
*/

#include <propvarutil.h>

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
	IMFSourceReader* sourceReader;
	IMFMediaType* uncompressedAudioType;
	int nextFrame;
	short* leftoverBuffer;
    UINT32 leftoverBufferSize;
    UINT32 leftoverBufferLength;
    int leftoverBufferPosition;
    long currentPosition;
    bool isDead;
    bool isSeeking;
	unsigned int bitsPerSample;
	short destBufferShort[8192];
} MediaFoundationFileContainer;

extern "C"
{
	static int MediaFoundation_init(void);
	static void MediaFoundation_quit(void);
	static int MediaFoundation_open(Sound_Sample *sample, const char *ext);
	static void MediaFoundation_close(Sound_Sample *sample);
	static size_t MediaFoundation_read(Sound_Sample *sample);
	static int MediaFoundation_rewind(Sound_Sample *sample);
	static int MediaFoundation_seek(Sound_Sample *sample, size_t ms);
}
// http://msdn.microsoft.com/en-us/library/windows/desktop/dd757927(v=vs.85).aspx
static const char* extensions_MediaFoundation[] = 
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
//	"caf",
//	"Sd2f",
//	"Sd2",
//	"au",
//	"snd",
//	"next",
//	"mp2",
//	"mp1",
//	"ac3",
	"3gpp",
	"3gp",
	"3gp2",
	"3g2",
//	"amrf",
//	"amr",
//	"ima4",
//	"ima",
	"asf",
	"wma",
	"sami",
	"sm",
	NULL 
};

//extern Sound_DecoderFunctions __Sound_DecoderFunctions_MediaFoundation;
extern "C" const Sound_DecoderFunctions __Sound_DecoderFunctions_MediaFoundation;

	const Sound_DecoderFunctions __Sound_DecoderFunctions_MediaFoundation =
//	__Sound_DecoderFunctions_MediaFoundation =
	{
		{
			extensions_MediaFoundation,
			"Decode audio through Windows Media Foundation",
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

	/* Initialize the COM library. */
    hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if(FAILED(hr))
	{
//        std::cerr << "SSMF: failed to initialize COM" << std::endl;
        SNDDBG(("WindowsMediaFoundation: Failed to initialize COM"));
        return 0;
    }


	hr = MFStartup(MF_VERSION, 0);
	if(S_OK == hr)
	{
		return 1;
	}
	else
	{
        SNDDBG(("WindowsMediaFoundation: MFStartup failed"));
		return 0;
	}	
} /* MediaFoundation_init */


static void MediaFoundation_quit(void)
{
    MFShutdown();
    CoUninitialize();	
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
	Sound_SampleInternal* internal = (Sound_SampleInternal*)sample->opaque;
	//Float64 estimated_duration;
	//UInt32 format_size;
	HRESULT hresult;
	IMFSourceReader* source_reader = NULL;
//	const WCHAR* source_file = L"C:\\Users\\pinky\\Documents\\Mario_Jumping.wav";
//	const WCHAR* source_file = L"C:\\Users\\pinky\\Documents\\battle_hymn_of_the_republic.mp3";
	const WCHAR* source_file = L"C:\\Users\\pinky\\Documents\\TheDeclarationOfIndependencePreambleJFK.m4a";
	
	media_foundation_file_container = (MediaFoundationFileContainer*)calloc(1, sizeof(MediaFoundationFileContainer));
	BAIL_IF_MACRO(media_foundation_file_container == NULL, ERR_OUT_OF_MEMORY, 0);

//hresult = MFCreateSourceReaderFromByteStream();
	hresult = MFCreateSourceReaderFromURL(source_file, NULL, &source_reader);
	if(FAILED(hresult))
    {
		SNDDBG(("Error opening input file"));
//		fprintf(stderr, "Error opening input file: %S\n", source_file, hresult);
		free(media_foundation_file_container);
		return 0;
	}

	internal->decoder_private = media_foundation_file_container;
	media_foundation_file_container->sourceReader = source_reader;

	{
		HRESULT hr = hresult;
		IMFMediaType *pUncompressedAudioType = NULL;
		IMFMediaType* audio_type = NULL;

		// Select the first audio stream, and deselect all other streams.
		hr = source_reader->SetStreamSelection((DWORD)MF_SOURCE_READER_ALL_STREAMS, false);

		if (SUCCEEDED(hr))
		{
			// select first stream
			hr = source_reader->SetStreamSelection((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, true);
		}

		// Getting format data and debugging
		{
			// Not sure of the difference between GetNativeMediaType/GetNativeMediaType.
			// Search suggests GetCurrentMediaType gets the complete uncompressed format.
			// hr = source_reader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &audio_type);
			// The method returns a copy of the media type, so it is safe to modify the object received in the ppMediaType parameter.
			hr = source_reader->GetNativeMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM,
				0, // Index of the media type to retreive (don't really know what that means)
				&audio_type
			);
			if(FAILED(hr))
			{
				SNDDBG(("GetNativeMediaType failed"));
				free(media_foundation_file_container);
				SafeRelease(&source_reader);		
				return 0;
    		}

			UINT32 all_samples_independent = 0;
			UINT32 fixed_size_samples = 0;
			UINT32 sample_size = 0;
			UINT32 bits_per_sample = 0;
			UINT32 block_alignment = 0;
			UINT32 num_channels = 0;
			UINT32 samples_per_second = 0;
			hr = audio_type->GetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, &all_samples_independent);
			hr = audio_type->GetUINT32(MF_MT_FIXED_SIZE_SAMPLES, &fixed_size_samples);
			hr = audio_type->GetUINT32(MF_MT_SAMPLE_SIZE, &sample_size);
			hr = audio_type->GetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, &bits_per_sample);
			hr = audio_type->GetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, &block_alignment);
			hr = audio_type->GetUINT32(MF_MT_AUDIO_NUM_CHANNELS, &num_channels);
			hr = audio_type->GetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, &samples_per_second);

			SNDDBG(("WindowsMediaFoundation: all_samples_independent == (%d).\n", all_samples_independent));
			SNDDBG(("WindowsMediaFoundation: fixed_size_samples == (%d).\n", fixed_size_samples));
			SNDDBG(("WindowsMediaFoundation: sample_size == (%d).\n", sample_size));
			SNDDBG(("WindowsMediaFoundation: bits_per_sample == (%d).\n", bits_per_sample));
			SNDDBG(("WindowsMediaFoundation: block_alignment == (%d).\n", block_alignment));
			SNDDBG(("WindowsMediaFoundation: num_channels == (%d).\n", num_channels));
			SNDDBG(("WindowsMediaFoundation: samples_per_second == (%d).\n", samples_per_second));


			// Get the total length of the stream 
			PROPVARIANT prop_variant;
			double duration_in_milliseconds = -1.0;
			// get the duration, which is a 64-bit integer of 100-nanosecond units
			hr = source_reader->GetPresentationAttribute(MF_SOURCE_READER_MEDIASOURCE, MF_PD_DURATION, &prop_variant);
			if(FAILED(hr))
			{
				SNDDBG(("WindowsMediaFoundation: Failed to get duration"));
				duration_in_milliseconds = -1.0;
			}
			else
			{
				LONGLONG file_duration = prop_variant.uhVal.QuadPart;
				//double durationInSeconds = (file_duration / static_cast<double>(10000 * 1000));
				duration_in_milliseconds = (file_duration / static_cast<double>(10000));
			}
			PropVariantClear(&prop_variant);


			sample->flags = SOUND_SAMPLEFLAG_CANSEEK;
			sample->actual.rate = samples_per_second;
			sample->actual.channels = (UINT8)num_channels;
			internal->total_time = (INT32)(duration_in_milliseconds + 0.5);
			/*
			 * I want to use the native system to do conversion and decoding for performance reasons.
			 * This is particularly important on mobile devices like iOS.
			 * Taking from the Ogg Vorbis decode, I pretend the "actual" format is the same 
			 * as the desired format. 
			 */
			if(0 == sample->desired.format)
			{
				sample->actual.format = AUDIO_S16SYS;
			}
			else
			{
				sample->actual.format = sample->desired.format;				
			}

			SNDDBG(("WindowsMediaFoundation: total seconds of sample == (%d).\n", internal->total_time));


			// For compressed files, the bits per sample is undefined
			if(0 == bits_per_sample)
			{
				// hard code to 16
				media_foundation_file_container->bitsPerSample = 16;
			}
			else
			{
				media_foundation_file_container->bitsPerSample = bits_per_sample;
			}

			SafeRelease(&audio_type);		
			

		}


		{
			IMFMediaType* target_audio_type = NULL;
		

			// Create a partial media type that specifies uncompressed PCM audio.
			hr = MFCreateMediaType(&target_audio_type);
			if(FAILED(hr))
			{
				SNDDBG(("WindowsMediaFoundation: Failed to create target MediaType\n"));
				SafeRelease(&source_reader);
				free(media_foundation_file_container);
			}

			hr = target_audio_type->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
			if(FAILED(hr))
			{
				SNDDBG(("WindowsMediaFoundation: Failed to set MFMediaType_Audio\n"));
				SafeRelease(&target_audio_type);
				SafeRelease(&source_reader);
				free(media_foundation_file_container);
				return 0;
			}
			// We want to decode to raw PCM
			hr = target_audio_type->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
			if(FAILED(hr))
			{
				SNDDBG(("WindowsMediaFoundation: Failed to set MFAudioFormat_PCM\n"));
				SafeRelease(&target_audio_type);
				SafeRelease(&source_reader);
				free(media_foundation_file_container);
				return 0;
			}

			// Set this type on the source reader. The source reader will
			// load the necessary decoder.
			hr = source_reader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, NULL, target_audio_type);
			if(FAILED(hr))
			{
				SNDDBG(("WindowsMediaFoundation: Failed to set SetCurrentMediaType\n"));
				SafeRelease(&target_audio_type);
				SafeRelease(&source_reader);
				free(media_foundation_file_container);
				return 0;
			}

			// Don't need this any more
			SafeRelease(&target_audio_type);
		}

		// specify the output type (pcm)
		{
			IMFMediaType* uncompressed_audio_type = NULL;
			

			// Get the complete uncompressed format.
			hr = source_reader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, &uncompressed_audio_type);
			if(FAILED(hr))
			{
				SNDDBG(("WindowsMediaFoundation: Failed to set SetCurrentMediaType\n"));
				SafeRelease(&source_reader);
				free(media_foundation_file_container);
				return 0;
			}
			hr = source_reader->SetStreamSelection((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, true);
			// Ensure the stream is selected.
			if(FAILED(hr))
			{
				SNDDBG(("WindowsMediaFoundation: Failed to set SetCurrentMediaType\n"));
				SafeRelease(&uncompressed_audio_type);
				SafeRelease(&source_reader);
				free(media_foundation_file_container);
				return 0;
			}

			// libaudiodecoder
			UINT32 leftover_buffer_size = 0;
			hr = uncompressed_audio_type->GetUINT32(MF_MT_SAMPLE_SIZE, &leftover_buffer_size);
			if(FAILED(hr))
			{
				SNDDBG(("WindowsMediaFoundation: Failed to get leftover_buffer_size\n"));
				leftover_buffer_size = 32;
			}



			media_foundation_file_container->leftoverBufferSize = leftover_buffer_size;
			media_foundation_file_container->leftoverBufferSize = leftover_buffer_size;
			media_foundation_file_container->leftoverBufferSize /= 2; // convert size in bytes to size in int16s
			media_foundation_file_container->leftoverBuffer = (short*)malloc(media_foundation_file_container->leftoverBufferSize * sizeof(short));

			media_foundation_file_container->uncompressedAudioType = uncompressed_audio_type;
			media_foundation_file_container->uncompressedAudioType->AddRef();
			SafeRelease(&uncompressed_audio_type);

		}

	}
		
	


	return(1);
} /* MediaFoundation_open */


static void MediaFoundation_close(Sound_Sample *sample)
{
	Sound_SampleInternal* internal = (Sound_SampleInternal*)sample->opaque;
	MediaFoundationFileContainer* media_foundation_file_container = (MediaFoundationFileContainer *)internal->decoder_private;

	SafeRelease(&media_foundation_file_container->sourceReader);
	free(media_foundation_file_container->leftoverBuffer);
	free(media_foundation_file_container);
} /* MediaFoundation_close */



// MSDN
static DWORD CalculateMaxAudioDataSize(
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

/**
 * libaudiodecoder
 * Copies min(destFrames, srcFrames) frames to dest from src. Anything leftover
 * is moved to the beginning of m_leftoverBuffer, so empty it first (possibly
 * with this method). If src and dest overlap, I'll hurt you.
 */
static void MediaFoundation_CopyFrames(
    short *dest, size_t *destFrames, const short *src, size_t srcFrames, UINT8 num_channels, MediaFoundationFileContainer* media_foundation_file_container)
{
    if (srcFrames > *destFrames) {
        int samplesToCopy = *destFrames * num_channels;
        memcpy(dest, src, samplesToCopy * sizeof(*src));
        srcFrames -= *destFrames;
        memmove(media_foundation_file_container->leftoverBuffer,
            src + samplesToCopy,
            srcFrames * num_channels * sizeof(*src));
        *destFrames = 0;
        media_foundation_file_container->leftoverBufferLength = srcFrames;
    } else {
        int samplesToCopy = srcFrames * num_channels;
        memcpy(dest, src, samplesToCopy * sizeof(*src));
        *destFrames -= srcFrames;
        if (src == media_foundation_file_container->leftoverBuffer) {
            media_foundation_file_container->leftoverBufferLength = 0;
        }
    }
}

/**
 * Convert a 100ns Media Foundation value to a frame offset.
 */
static __int64 MediaFoundation_FrameFromMF(__int64 mf, int sample_rate)
{
    return static_cast<__int64>(mf) * sample_rate / 1e7;
}
/**
 * Convert a frame offset to a 100ns Media Foundation value.
 */
static __int64 MediaFoundation_MfFromFrame(__int64 frame, int sample_rate)
{
    return static_cast<__int64>(frame) / sample_rate * 1e7;
}


static const MFTIME ONE_SECOND = 10000000; // One second.
static const LONG   ONE_MSEC = 1000;       // One millisecond
// Convert 100-nanosecond units to milliseconds.
static LONG MediaFoundation_MFTimeToMsec(const LONGLONG& time)
{
    return (LONG)(time / (ONE_SECOND / ONE_MSEC));
}

static LONGLONG MediaFoundation_MsecToMFTime(const LONG& time)
{
    return (LONGLONG)((ONE_SECOND / ONE_MSEC) * time);
}



static size_t MediaFoundation_read(Sound_Sample* sample)
{
	size_t total_bytes_read = 0;

	Sound_SampleInternal *internal = (Sound_SampleInternal *) sample->opaque;
	MediaFoundationFileContainer* media_foundation_file_container = (MediaFoundationFileContainer*)internal->decoder_private;
	UINT32 max_buffer_size = internal->buffer_size;
	UINT8 num_channels = sample->actual.channels;
	int sample_rate = sample->actual.rate;

    HRESULT hr = S_OK;

    IMFSample* the_sample = NULL;


	// need to convert bytes to samples
	/*
	max_buffer_size (bytes)   	8 bits 	    
	----------------------- * 	------- * 	--------------
								1 byte 		bitsPerSample
	*/
	UINT32 requested_sample_size = max_buffer_size / media_foundation_file_container->bitsPerSample * 8;
	// Note: internal->buffer is char*, not short*
	short* dest_buffer = (short*)internal->buffer;
	size_t frames_requested = (requested_sample_size /  num_channels);
    size_t frames_needed = frames_requested;




	// first, copy frames from leftover buffer IF the leftover buffer is at
	// the correct frame
	if(media_foundation_file_container->leftoverBufferLength > 0 && media_foundation_file_container->leftoverBufferPosition == media_foundation_file_container->nextFrame)
	{
		MediaFoundation_CopyFrames(dest_buffer, &frames_needed, media_foundation_file_container->leftoverBuffer,
			media_foundation_file_container->leftoverBufferLength, num_channels, media_foundation_file_container);
		if(media_foundation_file_container->leftoverBufferLength > 0)
		{
			if(frames_needed != 0)
			{
				SNDDBG(("WindowsMediaFoundation: Expected frames needed to be 0. Abandoning this file.\n"));
				media_foundation_file_container->isDead = true;
			}
			media_foundation_file_container->leftoverBufferPosition += frames_requested;
		}
	}
	else
	{
		// leftoverBuffer already empty or in the wrong position, clear it
		media_foundation_file_container->leftoverBufferLength = 0;
	}




	while(!media_foundation_file_container->isDead && frames_needed > 0)
	{
		HRESULT hr = S_OK;
		DWORD stream_flags = 0;
		__int64 timestamp = 0;
		IMFSample* current_sample = NULL;
		bool the_error = false; // set to true to break after releasing

		hr = media_foundation_file_container->sourceReader->ReadSample(
			MF_SOURCE_READER_FIRST_AUDIO_STREAM, // [in] DWORD dwStreamIndex,
			0,                                   // [in] DWORD dwControlFlags,
			NULL,                                // [out] DWORD *pdwActualStreamIndex,
			&stream_flags,                            // [out] DWORD *pdwStreamFlags,
			&timestamp,                          // [out] LONGLONG *pllTimestamp,
			&current_sample);                           // [out] IMFSample **ppSample
		if(FAILED(hr))
		{
			SNDDBG(("WindowsMediaFoundation read: ReadSample failed\n"));
			break;
		}

		/*
		   std::cout << "ReadSample timestamp: " << timestamp
		   << "frame: " << frameFromMF(timestamp)
		   << "dwflags: " << stream_flags
		   << std::endl;
		   */
		SNDDBG(("WindowsMediaFoundation: ReadSample timestamp:%ld, frame:%ld, stream_flags:%d\n", 
				timestamp,
				MediaFoundation_FrameFromMF(timestamp, sample_rate),
				stream_flags
		));

		if (stream_flags & MF_SOURCE_READERF_ERROR)
		{
			// our source reader is now dead, according to the docs
			SNDDBG(("WindowsMediaFoundation read: ReadSample set ERROR, SourceReader is now dead\n"));
			media_foundation_file_container->isDead = true;
//			sample->flags |= SOUND_SAMPLEFLAG_EOF;
			// more C++ BS
			sample->flags = (SoundDecoder_SampleFlags)(sample->flags | SOUND_SAMPLEFLAG_ERROR);
			break;
		}
		else if(stream_flags & MF_SOURCE_READERF_ENDOFSTREAM)
		{
			SNDDBG(("WindowsMediaFoundation read: End of input file.\n"));
//			sample->flags |= SOUND_SAMPLEFLAG_EOF;
			// more C++ BS
			sample->flags = (SoundDecoder_SampleFlags)(sample->flags | SOUND_SAMPLEFLAG_EOF);
			break;
		}
		else if (stream_flags & MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED)
		{
			SNDDBG(("WindowsMediaFoundation read: Type change\n"));			
			/* Don't know what to do here. */
//			sample->flags |= SOUND_SAMPLEFLAG_EAGAIN;
			// more C++ BS
			sample->flags = (SoundDecoder_SampleFlags)(sample->flags | SOUND_SAMPLEFLAG_EAGAIN);
			break;
		}
		else if(NULL == current_sample)
		{
			// generally this will happen when stream_flags contains ENDOFSTREAM,
			// so it'll be caught before now -bkgood
			SNDDBG(("WindowsMediaFoundation read: No sample\n"));			
			/* Don't know what to do here. */
//			sample->flags |= SOUND_SAMPLEFLAG_ERROR;
			// more C++ BS
			sample->flags = (SoundDecoder_SampleFlags)(sample->flags | SOUND_SAMPLEFLAG_ERROR);
			continue;
		} // we now own a ref to the instance at current_sample


		IMFMediaBuffer* media_buffer = NULL;
		// I know this does at least a memcopy and maybe a malloc, if we have
		// xrun issues with this we might want to look into using
		// IMFSample::GetBufferByIndex (although MS doesn't recommend this)
		if(FAILED(hr = current_sample->ConvertToContiguousBuffer(&media_buffer)))
		{
			the_error = true;
			goto releaseSample;
		}
		short* the_buffer = NULL;
		size_t buffer_length = 0;
		hr = media_buffer->Lock(reinterpret_cast<unsigned __int8**>(&the_buffer), NULL,
			reinterpret_cast<DWORD*>(&buffer_length));
		if(FAILED(hr))
		{
			the_error = true;
			goto releaseMBuffer;
		}
		buffer_length /= (media_foundation_file_container->bitsPerSample / 8 * num_channels); // now in frames

		if(media_foundation_file_container->isSeeking)
		{
			__int64 buffer_position = MediaFoundation_FrameFromMF(timestamp, sample_rate);
			SNDDBG(("WindowsMediaFoundation read: While seeking to nextFrame:%d, WMF put us at buffer_position:%ld\n", 
				media_foundation_file_container->nextFrame,
				buffer_position,
			));
			/*
			   std::cout << "While seeking to "
			   << m_nextFrame << "WMF put us at " << buffer_position
			   << std::endl;
*/
            if (media_foundation_file_container->nextFrame < buffer_position)
			{
                // Uh oh. We are farther forward than our seek target. Emit
                // silence? We can't seek backwards here.
                short* buffer_current_position = dest_buffer + (requested_sample_size - frames_needed * num_channels);
                __int64 offshoot_frames = buffer_position - media_foundation_file_container->nextFrame;

                // If we can correct this immediately, write zeros and adjust
                // media_foundation_file_container->nextFrame to pretend it never happened.

                if(offshoot_frames <= frames_needed)
				{
					SNDDBG(("WindowsMediaFoundation read: Working around inaccurate seeking. Writing silence for:%ld offshoot frames\n", offshootFrames));				
					/*
					   std::cerr << __FILE__ << __LINE__
					   << "Working around inaccurate seeking. Writing silence for"
					   << offshootFrames << "frames";
					   */
					// Set offshoot_frames * num_channels samples to zero.
					memset(buffer_current_position, 0,
						sizeof(*buffer_current_position) * offshoot_frames *
						num_channels);
					// Now media_foundation_file_container->nextFrame == buffer_position
					media_foundation_file_container->nextFrame += offshoot_frames;
					frames_needed -= offshoot_frames;
				}
				else
				{
					// It's more complicated. The buffer we have just decoded is
					// more than framesNeeded frames away from us. It's too hard
					// for us to handle this correctly currently, so let's just
					// try to get on with our lives.
					media_foundation_file_container->isSeeking = false;
					media_foundation_file_container->nextFrame = buffer_position;
					SNDDBG(("WindowsMediaFoundation read: Seek offshoot is too drastic. Cutting losses and pretending the current decoded audio buffer is the right seek point.\n"));
					/*
					   std::cerr << __FILE__ << __LINE__
					   << "Seek offshoot is too drastic. Cutting losses and pretending the current decoded audio buffer is the right seek point.";
					   */
				}
			}

			if(media_foundation_file_container->nextFrame >= buffer_position &&
				media_foundation_file_container->nextFrame < buffer_position + buffer_length)
			{
				// media_foundation_file_container->nextFrame is in this buffer.
				the_buffer += (media_foundation_file_container->nextFrame - buffer_position) * num_channels;
				buffer_length -= media_foundation_file_container->nextFrame - buffer_position;
				media_foundation_file_container->isSeeking = false;
			}
			else
			{
				// we need to keep going forward
				goto releaseRawBuffer;
			}
		}

		// If the buffer_length is larger than the leftover buffer, re-allocate
		// it with 2x the space.
		if (buffer_length * num_channels > media_foundation_file_container->leftoverBufferSize)
		{
			int new_size = media_foundation_file_container->leftoverBufferSize;

			while(new_size < buffer_length * num_channels)
			{
				new_size *= 2;
			}
			short* new_buffer = (short*)calloc(new_size, sizeof(short));
			memcpy(new_buffer, media_foundation_file_container->leftoverBuffer,
				sizeof(media_foundation_file_container->leftoverBuffer[0]) * media_foundation_file_container->leftoverBufferSize);
			free(media_foundation_file_container->leftoverBuffer);
			media_foundation_file_container->leftoverBuffer = new_buffer;
			media_foundation_file_container->leftoverBufferSize = new_size;
		}
		MediaFoundation_CopyFrames(dest_buffer + (requested_sample_size - frames_needed * num_channels),
			&frames_needed, the_buffer, buffer_length, num_channels, media_foundation_file_container);

releaseRawBuffer:
		hr = media_buffer->Unlock();
		// I'm ignoring this, MSDN for IMFMediaBuffer::Unlock stipulates
		// nothing about the state of the instance if this fails so might as
		// well just let it be released.
		//if (FAILED(hr)) break;
releaseMBuffer:
		SafeRelease(&media_buffer);
releaseSample:
		SafeRelease(&current_sample);
		if(the_error)
		{
			break;
		}
    }

    media_foundation_file_container->nextFrame += frames_requested - frames_needed;
    if(media_foundation_file_container->leftoverBufferLength > 0)
	{
        if(frames_needed != 0)
		{
			SNDDBG(("WindowsMediaFoundation read: Expected frames needed to be 0. Abandoning this file.\n"));
            media_foundation_file_container->isDead = true;
        }
        media_foundation_file_container->leftoverBufferPosition = media_foundation_file_container->nextFrame;
    }
    long samples_read = requested_sample_size - frames_needed * num_channels;
    media_foundation_file_container->currentPosition += samples_read;
	SNDDBG(("WindowsMediaFoundation read for requested_sample_size:%d returning %d samples_read.\n", requested_sample_size, samples_read));
	
	/* // I don't want float samples
	const int sample_max = 1 << (media_foundation_file_container->bitsPerSample-1);
	//Convert to float samples
	if (num_channels == 2)
	{
		SAMPLE *destBufferFloat(const_cast<SAMPLE*>(destination));
		for (unsigned long i = 0; i < samples_read; i++)
		{
			destBufferFloat[i] = destBuffer[i] / (float)sample_max;
		}
	}
	else //Assuming mono, duplicate into stereo frames...
	{
		SAMPLE *destBufferFloat(const_cast<SAMPLE*>(destination));
		for (unsigned long i = 0; i < samples_read; i++)
		{
			destBufferFloat[i] = destBuffer[i] / (float)sample_max;
		}
	}
	*/
	/* convert samples_read to bytes */
	/*
	samples_read (samples)   	bits 				1 byte
	----------------------- * 	-------------- * 	--------------
								sample				8 bits
	*/
	total_bytes_read = samples_read / 8 * media_foundation_file_container->bitsPerSample;
	return total_bytes_read;
} /* MediaFoundation_read */


static int MediaFoundation_seek(Sound_Sample *sample, size_t ms)
{
	Sound_SampleInternal *internal = (Sound_SampleInternal *) sample->opaque;
	MediaFoundationFileContainer* media_foundation_file_container = (MediaFoundationFileContainer*)internal->decoder_private;

	// our seek (ms) is in milliseconds.
	// Windows wants units of 100 nanoseconds
	// 0.001 = millisecond
	// 0.0000001 = nanosecond
	// 1/10,000,000
    PROPVARIANT prop_variant;
    HRESULT hr = S_OK;
    __int64 mf_seek_target = MediaFoundation_MsecToMFTime(ms);

	int sample_rate = sample->actual.rate;
	// libaudiodecoder assumed a sample_index as the input. I have milliseconds. I need to figure out how to convert.
	// Does this work?
	int sample_index = MediaFoundation_FrameFromMF(mf_seek_target, sample_rate);
	// Need the above to work for this to be right
    __int64 seek_target_frame = sample_index / sample->actual.channels;


	if(mf_seek_target >= 1)
	{
		mf_seek_target = mf_seek_target - 1;
	}
    // minus 1 here seems to make our seeking work properly, otherwise we will
    // (more often than not, maybe always) seek a bit too far (although not
    // enough for our calculatedFrameFromMF <= nextFrame assertion in ::read).
    // Has something to do with 100ns MF units being much smaller than most
    // frame offsets (in seconds) -bkgood
    long the_result = media_foundation_file_container->currentPosition;

    if(media_foundation_file_container->isDead)
	{
		SNDDBG(("WindowsMediaFoundation in seek(), isDead\n"));
//		sample->flags |= SOUND_SAMPLEFLAG_ERROR;
		// more C++ BS
		sample->flags = (SoundDecoder_SampleFlags)(sample->flags | SOUND_SAMPLEFLAG_ERROR);
		return 1;
    }

    // this doesn't fail, see MS's implementation
    hr = InitPropVariantFromInt64(mf_seek_target, &prop_variant);


    hr = media_foundation_file_container->sourceReader->Flush(MF_SOURCE_READER_FIRST_AUDIO_STREAM);
    if(FAILED(hr))
	{
		SNDDBG(("WindowsMediaFoundation failed to flush before seek\n"));
//		sample->flags |= SOUND_SAMPLEFLAG_ERROR;
		// more C++ BS
		sample->flags = (SoundDecoder_SampleFlags)(sample->flags | SOUND_SAMPLEFLAG_ERROR);
	}

    // http://msdn.microsoft.com/en-us/library/dd374668(v=VS.85).aspx
    hr = media_foundation_file_container->sourceReader->SetCurrentPosition(GUID_NULL, prop_variant);
    if(FAILED(hr))
	{
        // nothing we can do here as we can't fail (no facility to other than
        // crashing mixxx)
//		sample->flags |= SOUND_SAMPLEFLAG_ERROR;
		// more C++ BS
		sample->flags = (SoundDecoder_SampleFlags)(sample->flags | SOUND_SAMPLEFLAG_ERROR);
		if (hr == MF_E_INVALIDREQUEST)
		{
			SNDDBG(("WindowsMediaFoundation failed to seek: Sample requests still pending\n"));
		}
		else
		{
			SNDDBG(("WindowsMediaFoundation failed to seek\n"));
		}
    }
	else
	{
		the_result = sample_index;
    }
    PropVariantClear(&prop_variant);

    // record the next frame so that we can make sure we're there the next
    // time we get a buffer from MFSourceReader
    media_foundation_file_container->nextFrame = seek_target_frame;
    media_foundation_file_container->isSeeking = true;
    media_foundation_file_container->currentPosition = the_result;
	return(1);
} /* MediaFoundation_seek */

static int MediaFoundation_rewind(Sound_Sample* sample)
{
	MediaFoundation_seek(sample, 0);
	return(1);
} /* MediaFoundation_rewind */



#endif /* _WIN32 */


#endif /* ALMIXER_COMPILE_WITHOUT_SDL */

