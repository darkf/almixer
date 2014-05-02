

#ifndef ALMIXER_COMPILE_WITH_SDLSOUND

/*
 * Windows Media Foundation backend
 * Copyright (C) 2014 Eric Wing <ewing . public @ playcontrol.net>
 *
 * The read and seek functions come from libaudiodecoder under the MIT license:
 Copyright (c) 2010-2012 Albert Santoni, Bill Good, RJ Ryan

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 The text above constitutes the entire libaudiodecoder license; however, the Oscillicious community also makes the following non-binding requests:

 Any person wishing to distribute modifications to the Software is requested to send the modifications to the original developer so that they can be incorporated into the canonical version. It is also requested that these non-binding requests be included along with the license above.

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

#include <propvarutil.h>

#include <stddef.h> /* NULL */
#include <stdio.h> /* printf */


#include "SoundDecoder.h"

#include "SoundDecoder_Internal.h"
#include "WindowsMediaFoundation_IMFByteStreamRWops.hpp"

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
	IMFByteStreamRWops* byteStreamRWops;
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

// This BS dance is to get around the C++ name mangling which leads to linking problems.
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

// This BS dance is to get around the C++ name mangling which leads to linking problems.
extern "C" const Sound_DecoderFunctions __Sound_DecoderFunctions_MediaFoundation;
const Sound_DecoderFunctions __Sound_DecoderFunctions_MediaFoundation =
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


static int MediaFoundation_open(Sound_Sample *sample, const char *ext)
{
	MediaFoundationFileContainer* media_foundation_file_container;
	Sound_SampleInternal* internal = (Sound_SampleInternal*)sample->opaque;
	HRESULT hresult;
	IMFSourceReader* source_reader = NULL;
	// Since the byte stream stuff is so complicated, if you need to test without it, 
	// you can hard code loading a file and use MFCreateSourceReaderFromURL.
//	const WCHAR* source_file = L"C:\\Users\\username\\Documents\\crystal.wav";
//	const WCHAR* source_file = L"C:\\Users\\username\\Documents\\battle_hymn_of_the_republic.mp3";
//	const WCHAR* source_file = L"C:\\Users\\username\\Documents\\TheDeclarationOfIndependencePreambleJFK.m4a";
	

	IMFByteStreamRWops* byte_stream = new IMFByteStreamRWops(internal->rw, sample);
//	hresult = MFCreateSourceReaderFromURL(source_file, NULL, &source_reader);
	hresult = MFCreateSourceReaderFromByteStream(byte_stream, NULL, &source_reader);
	if (FAILED(hresult))
    {
		SNDDBG(("Error opening input file"));
		return 0;
	}

	media_foundation_file_container = (MediaFoundationFileContainer*)calloc(1, sizeof(MediaFoundationFileContainer));
	BAIL_IF_MACRO(media_foundation_file_container == NULL, ERR_OUT_OF_MEMORY, 0);

	internal->decoder_private = media_foundation_file_container;
	media_foundation_file_container->sourceReader = source_reader;
	media_foundation_file_container->byteStreamRWops = byte_stream;

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
//	delete media_foundation_file_container->byteStreamRWops;
	media_foundation_file_container->byteStreamRWops->Release();
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
		SNDDBG(("WindowsMediaFoundation: ReadSample timestamp:%ld, frame:%ld, stream_flags:%d\n", 
				timestamp,
				MediaFoundation_FrameFromMF(timestamp, sample_rate),
				stream_flags
		));
		*/
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
				buffer_position
			));

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
					SNDDBG(("WindowsMediaFoundation read: Working around inaccurate seeking. Writing silence for:%ld offshoot frames\n", offshoot_frames));

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


#endif /* ALMIXER_COMPILE_WITH_SDLSOUND */

