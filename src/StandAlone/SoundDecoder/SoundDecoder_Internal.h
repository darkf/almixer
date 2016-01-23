#ifndef SOUNDDECODER_INTERNAL_H
#define SOUNDDECODER_INTERNAL_H

#include <stdio.h>
#include "al.h" /* OpenAL */
#include "ALmixer_RWops.h"

#ifdef __cplusplus
extern "C" {
#endif
	

typedef struct SoundDecoder_DecoderFunctions
{
    const SoundDecoder_DecoderInfo info;
    int (*init)(void);
    void (*quit)(void);
    int (*open)(SoundDecoder_Sample *sample, const char *ext);
    void (*close)(SoundDecoder_Sample *sample);
    size_t (*read)(SoundDecoder_Sample *sample);
    int (*rewind)(SoundDecoder_Sample *sample);
    int (*seek)(SoundDecoder_Sample *sample, size_t ms);
} SoundDecoder_DecoderFunctions;

typedef struct SoundDecoder_DecoderFunctions Sound_DecoderFunctions;



typedef struct SoundDecoder_SampleInternal
{
    ALmixer_RWops *rw;
    const SoundDecoder_DecoderFunctions *funcs;
    void *buffer;
    size_t buffer_size;
    void *decoder_private;
    ptrdiff_t total_time;
#if 0
	FILE* optional_file_handle; /* needed for Android OpenSL ES decoder backend */
	const char* optional_file_name;  /* needed for Android OpenSL ES decoder backend */
#endif

} SoundDecoder_SampleInternal;

typedef struct SoundDecoder_SampleInternal Sound_SampleInternal;

#define BAIL_MACRO(e, r) { SoundDecoder_SetError(e); return r; }
#define BAIL_IF_MACRO(c, e, r) if (c) { SoundDecoder_SetError(e); return r; }
#define ERR_OUT_OF_MEMORY "Out of memory"
#define ERR_NOT_INITIALIZED "SoundDecoder not initialized"
#define ERR_UNSUPPORTED_FORMAT "Unsupported codec"
#define ERR_NULL_SAMPLE "Sound sample is NULL"
#define ERR_PREVIOUS_SAMPLE_ERROR "Cannot operate on sample due to previous error"
#define ERR_ALREADY_AT_EOF_ERROR "Cannot operate on sample because already at EOF"

/* Helper APIs for Android OpenSL ES decoder backends. */
FILE* SoundDecoderInternal_GetOptionalFileHandle(SoundDecoder_SampleInternal* sample_internal);
void SoundDecoderInternal_SetOptionalFileHandle(SoundDecoder_SampleInternal* sample_internal, FILE* file_handle);
const char* SoundDecoderInternal_GetOptionalFileName(SoundDecoder_SampleInternal* sample_internal);
void SoundDecoderInternal_SetOptionalFileName(SoundDecoder_SampleInternal* sample_internal, const char* file_name);


/* TODO: Start migrating to varadic macros. 
	Visual Studio has kept everything back for years.
	But they finally support it.
	Both Windows and Android are terribly difficult in printing to console and the 
	current printf evaluation doesn't actually do anything useful.
	SNDDBG2 and SNDERR have been introduced for Visual Studio (because I needed to debug WMF).
	Start moving to these and introduce counterparts for the other modules when the opportunity arises.
	SNDERR is intended to be hard errors that we may want to always keep on.
*/

// Android is calling SNDDBG differently, without the evaluation double parenthesis.
// So special case it. Probably should add a disable case for it.
#ifdef __ANDROID__
#	define SNDDBG(...) ((void)__android_log_print(ANDROID_LOG_DEBUG,  "ALmixer", __VA_ARGS__))
#elif _MSC_VER 
	// drat: can't remember what version VS finally got variadic macros
#	if(_MSC_VER < 1600) // VS2010
		// don't know what to do
#	else
#		ifdef ALMIXER_COMPILE_WITH_SDL
#			include "SDL.h"
#			ifdef ALMIXER_SOUND_DECODER_DEBUG_CHATTER
#				define SNDDBG(x) SDL_Log x
#				define SNDDBG2(...) SDL_Log(__VA_ARGS__)
#			else
#				define SNDDBG(x)
#				define SNDDBG2(...)
#			endif
#			define SNDERR(...) SDL_Log(__VA_ARGS__)
#		else
#			define WIN32_LEAN_AND_MEAN
#			include <Windows.h>
#			// warning C4996: '_snprintf': This function or variable may be unsafe. Consider using _snprintf_s instead.
#			pragma warning(disable : 4996)
#			ifdef ALMIXER_SOUND_DECODER_DEBUG_CHATTER
				// legacy
#				define SNDDBG(x) printf x
				// We're making a better one. Visual Studio has been the hold back for variadic macros, but printf doesn't work either and VS finally got them.
#				define SNDDBG2(...) { char output_dbg_buf[1024]; _snprintf(output_dbg_buf, 1024, __VA_ARGS__); OutputDebugStringA(output_dbg_buf); }
#			else
#				define SNDDBG(x)
#				define SNDDBG2(...)
#			endif
#			// This one we might always leave on
#			define SNDERR(...) { char output_dbg_buf[1024]; _snprintf(output_dbg_buf, 1024, __VA_ARGS__); OutputDebugStringA(output_dbg_buf); }
#		endif
#	endif
#else
#	if (defined ALMIXER_SOUND_DECODER_DEBUG_CHATTER)
#	ifdef ALMIXER_COMPILE_WITH_SDL
#		include "SDL.h"
#		define SNDDBG(x) SDL_Log x
#	else
#		define SNDDBG(x) printf x
#	endif
#else
#	define SNDDBG(x)
#endif

#endif

void SoundDecoder_SetError(const char* err_str, ...);
#define __Sound_SetError SoundDecoder_SetError

int SoundDecoder_strcasecmp(const char *x, const char *y);
#define __Sound_strcasecmp SoundDecoder_strcasecmp

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif


#endif

