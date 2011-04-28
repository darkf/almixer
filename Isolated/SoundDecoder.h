
/*
 * This is a wrapper interface that tries to provide a similar
 * front-end interface to SDL_sound.
 */

#ifndef SOUNDDECODER_H 
#define SOUNDDECODER_H

#ifdef __cplusplus
extern "C" {
#endif
	

#include <stdint.h>
#include <stddef.h>

#include "al.h" /* OpenAL */

/* Compatibility defines for SDL */
#define AUDIO_U8 0x0008
#define AUDIO_S8 0x8008
#define AUDIO_U16LSB 0x0010	
#define AUDIO_S16LSB 0x8010
#define AUDIO_U16MSB 0x1010
#define AUDIO_S16MSB 0x9010
#define AUDIO_U16 AUDIO_U16LSB
#define AUDIO_S16 AUDIO_S16LSB

#ifdef ANDROID_NDK
	#include <endian.h>
	#if _BYTE_ORDER == _BIG_ENDIAN
		#define __BIG_ENDIAN__ 1
	#elif _BYTE_ORDER == _LITTLE_ENDIAN
		#define __LITTLE_ENDIAN__ 1
	#else
		#warning "Android falling back to __LITTLE_ENDIAN__"
		#define __LITTLE_ENDIAN__ 1
	#endif
#endif

#if __BIG_ENDIAN__
#warning "Using __BIG_ENDIAN__"
#define AUDIO_U16SYS AUDIO_U16MSB
#define AUDIO_S16SYS AUDIO_S16MSB
#elif __LITTLE_ENDIAN__
#define AUDIO_U16SYS	AUDIO_U16LSB
#define AUDIO_S16SYS	AUDIO_S16LSB
#else
#warning "Using __LITTLE_ENDIAN__ as fallback"
#define AUDIO_U16SYS	AUDIO_U16LSB
#define AUDIO_S16SYS	AUDIO_S16LSB
#endif

struct ALmixer_RWops;

typedef enum
{
	SOUND_SAMPLEFLAG_NONE = 0,
	SOUND_SAMPLEFLAG_CANSEEK = 1,
	SOUND_SAMPLEFLAG_EOF     = 1 << 29,
	SOUND_SAMPLEFLAG_ERROR   = 1 << 30,
	SOUND_SAMPLEFLAG_EAGAIN  = 1 << 31 
} SoundDecoder_SampleFlags;

#define Sound_SampleFlags SoundDecoder_SampleFlags;

typedef struct SoundDecoder_AudioInfo
{
    //uint16_t format;  /**< Equivalent of SDL_AudioSpec.format. */
    ALushort format;  /**< Equivalent of SDL_AudioSpec.format. */
    ALubyte channels;
    // uint8_t channels;
    //uint32_t rate; 
    ALuint rate; 
} SoundDecoder_AudioInfo;

//#define Sound_AudioInfo SoundDecoder_AudioInfo;
typedef struct SoundDecoder_AudioInfo Sound_AudioInfo;



typedef struct SoundDecoder_DecoderInfo
{
    const char** extensions;
    const char* description;
    const char* author;
    const char* url;
} SoundDecoder_DecoderInfo;

//#define Sound_DecoderInfo SoundDecoder_DecoderInfo;
typedef struct SoundDecoder_DecoderInfo Sound_DecoderInfo;



typedef struct SoundDecoder_Sample
{
    void* opaque;
    const SoundDecoder_DecoderInfo* decoder;
    SoundDecoder_AudioInfo desired;
    SoundDecoder_AudioInfo actual;
    void *buffer;
    size_t buffer_size;
    SoundDecoder_SampleFlags flags;
} SoundDecoder_Sample;

//#define Sound_Sample SoundDecoder_Sample;
typedef struct SoundDecoder_Sample Sound_Sample;


typedef struct SoundDecoder_Version
{
    int major;
    int minor;
    int patch;
} SoundDecoder_Version;

//#define Sound_Version SoundDecoder_Version;
typedef struct SoundDecoder_Version Sound_Version;


#define SOUNDDECODER_VER_MAJOR 0
#define SOUNDDECODER_VER_MINOR 0
#define SOUNDDECODER_VER_PATCH 1

#define SOUNDDECODER_VERSION(x) \
{ \
    (x)->major = SOUNDDECODER_VER_MAJOR; \
    (x)->minor = SOUNDDECODER_VER_MINOR; \
    (x)->patch = SOUNDDECODER_VER_PATCH; \
}

#define SOUND_VERSION SOUNDDECODER_VERSION

void SoundDecoder_GetLinkedVersion(SoundDecoder_Version *ver);
#define Sound_GetLinkedVersion SoundDecoder_GetLinkedVersion

int SoundDecoder_Init(void);
#define Sound_Init SoundDecoder_Init

void SoundDecoder_Quit(void);
#define Sound_Quit SoundDecoder_Quit


const SoundDecoder_DecoderInfo** SoundDecoder_AvailableDecoders(void);
#define Sound_AvailableDecoders SoundDecoder_AvailableDecoders


const char* SoundDecoder_GetError(void);
#define Sound_GetError SoundDecoder_GetError


void SoundDecoder_ClearError(void);
#define Sound_ClearError SoundDecoder_ClearError



SoundDecoder_Sample* SoundDecoder_NewSample(
	struct ALmixer_RWops* rw_ops,
	const char* ext,
	SoundDecoder_AudioInfo* desired,
	size_t buffer_size);
#define Sound_NewSample SoundDecoder_NewSample

SoundDecoder_Sample* SoundDecoder_NewSampleFromFile(const char* file_name,
	SoundDecoder_AudioInfo* desired,
	size_t bufferSize);
#define Sound_NewSampleFromFile SoundDecoder_NewSampleFromFile


void SoundDecoder_FreeSample(SoundDecoder_Sample* sound_sample);
#define Sound_FreeSample SoundDecoder_FreeSample


ptrdiff_t SoundDecoder_GetDuration(SoundDecoder_Sample* sound_sample);
#define Sound_GetDuration SoundDecoder_GetDuration

int SoundDecoder_SetBufferSize(SoundDecoder_Sample* sound_sample, size_t new_buffer_size);
#define Sound_SetBufferSize SoundDecoder_SetBufferSize

size_t SoundDecoder_Decode(SoundDecoder_Sample* sound_sample);
#define Sound_Decode SoundDecoder_Decode

size_t SoundDecoder_DecodeAll(SoundDecoder_Sample* sound_sample);
#define Sound_DecodeAll SoundDecoder_DecodeAll

int SoundDecoder_Rewind(SoundDecoder_Sample* sound_sample);
#define Sound_Rewind SoundDecoder_Rewind

int SoundDecoder_Seek(SoundDecoder_Sample* sound_sample, size_t ms);
#define Sound_Seek SoundDecoder_Seek


/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif

#endif

