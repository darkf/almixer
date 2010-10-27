/*
    SDL_ALmixer:  A library to make playing sounds and music easier,
	which uses OpenAL to manage sounds and SDL_Sound (by Ryan C. Gordon)
	to decode files.
    Copyright 2002 Eric Wing 

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/
#ifndef _SDL_ALMIXER_H_
#define _SDL_ALMIXER_H_

#include "SDL_types.h"
/*
#include "SDL_rwops.h"
#include "SDL_audio.h"
#include "SDL_byteorder.h"
*/
#include "SDL_version.h"

/*
#include "begin_code.h"
*/

#include "SDL_sound.h"
#include "altypes.h"

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif
		
/* Printable format: "%d.%d.%d", MAJOR, MINOR, PATCHLEVEL
*/
#define ALMIXER_MAJOR_VERSION		0
#define ALMIXER_MINOR_VERSION		1
#define ALMIXER_PATCHLEVEL			0

/* This macro can be used to fill a version structure with the compile-time
 * version of the SDL_mixer library.
 */
#define ALMIXER_VERSION(X)                                           \
{                                                                       \
        (X)->major = ALMIXER_MAJOR_VERSION;                          \
        (X)->minor = ALMIXER_MINOR_VERSION;                          \
        (X)->patch = ALMIXER_PATCHLEVEL;                             \
}

/* This function gets the version of the dynamically linked SDL_ALmixer library.
   it should NOT be used to fill a version structure, instead you should
   use the ALMIXER_VERSION() macro.
 */
extern DECLSPEC const SDL_version * SDLCALL ALmixer_Linked_Version();

/*
#define ALmixer_AudioInfo 	Sound_AudioInfo
*/

#define ALMIXER_DEFAULT_FREQUENCY 	44100
#define ALMIXER_DEFAULT_REFRESH 	0
#define ALMIXER_DEFAULT_NUM_CHANNELS	16
#define ALMIXER_DEFAULT_NUM_SOURCES		ALMIXER_DEFAULT_NUM_CHANNELS

#define ALMIXER_DEFAULT_BUFFERSIZE 32768
/* #define ALMIXER_DEFAULT_BUFFERSIZE 16384 */

/* Default Queue Buffers must be at least 2 */
#define ALMIXER_DEFAULT_QUEUE_BUFFERS 5
/* Default startup buffers should be at least 1 */
#define ALMIXER_DEFAULT_STARTUP_BUFFERS 2

#define ALMIXER_DECODE_STREAM 	0
#define ALMIXER_DECODE_ALL 		1


#define ALmixer_GetError 	SDL_GetError
#define ALmixer_SetError 	SDL_SetError

typedef struct {
	ALuint albuffer;
	Sint32 index; /* might not need */
	Uint8* data;
	Uint32 num_bytes;
} Buffer_Map;

typedef struct {
	Uint8 decoded_all; /* dictates different behaviors */
	Sint32 total_time; /* total playing time of sample (msec) */
	
	Uint32 in_use; /* needed to prevent sharing for streams */
	Uint8 eof; /* flag for eof, only used for streams  */
	
	Uint32 total_bytes; /* For predecoded */
	Uint32 loaded_bytes; /* For predecoded (for seek) */

	Sound_Sample* sample; /* SDL_Sound provides the data */
	ALuint* buffer; /* array of OpenAL buffers (at least 1 for predecoded) */

	/* Needed for streamed buffers */
	Uint32 max_queue_buffers; /* Max number of queue buffers */
	Uint32 num_startup_buffers; /* Number of ramp-up buffers */
	Uint8 num_buffers_in_use; /* number of buffers in use */
	
	/* This stuff is for streamed buffers that require data access */
	Buffer_Map* buffer_map_list; /* translate ALbuffer to index 
									and holds pointer to copy of data for
									data access */
	ALuint current_buffer; /* The current playing buffer */

	/* Nvidia distribution refuses to recognize a simple buffer query command
	 * unlike all other distributions. It's forcing me to redo the code 
	 * to accomodate this Nvidia flaw by making me maintain a "best guess"
	 * copy of what I think the buffer queue state looks like.
	 * A circular queue would a helpful data structure for this task,
	 * but I wanted to avoid making an additional header requirement,
	 * so I'm making it a void* 
	 */
	void* circular_buffer_queue; 
		
	
} ALmixer_Data;


#if 0
typedef struct {
	Sound_Sample* sample;
	Mix_Chunk** chunk; /* provide two chunks for double buffering */
	Uint8** double_buffer; /* Only used for streaming */
	Uint8 active_buffer; /* used to index the above chunk */
	void (*channel_done_callback)(int channel);
} ALmixer_Chunk;
#endif

/*
extern DECLSPEC int SDLCALL ALmixer_Init(int frequency, Uint16 format, int channels, int chunksize);
*/
/* Frequency == 0 means ALMIXER_DEFAULT_FREQUENCY */
/* This is the recommended Init function. This will initialize the context, SDL_sound,
 * and the mixer system. If you attempt to bypass this function, you do so at 
 * your own risk.
 */
extern DECLSPEC Sint32 SDLCALL ALmixer_Init(Uint32 frequency, Sint32 num_sources, Uint32 refresh);

/* This is a backdoor in case you need to initialize the AL context and 
 * the mixer system separately. I strongly recommend avoiding these two functions
 * and use the normal Init() function.
 */
/* Init_Context will only initialize the OpenAL context (and not the mixer part).
 * Note that SDL_Sound is also initialized here because load order matters
 * because SDL audio will conflict with OpenAL when using SMPEG. This is only 
 * provided as a backdoor and is not recommended.
 */
extern DECLSPEC Sint32 SDLCALL ALmixer_Init_Context(Uint32 frequency, Uint32 refresh);
/* Init_Mixer will only initialize the Mixer system. This is provided in the case 
 * that you need control over the loading of the context. You may load the context 
 * yourself, and then call this function. This is not recommended practice, but is 
 * provided as a backdoor in case you have good reason to 
 * do this. Be warned that if ALmixer_Init_Mixer() fails,
 * it will not clean up the AL context. Also be warned that Quit() still does try to 
 * clean up everything.
 */
extern DECLSPEC Sint32 SDLCALL ALmixer_Init_Mixer(Sint32 num_sources);

extern DECLSPEC void SDLCALL ALmixer_Quit();
extern DECLSPEC Uint8 SDLCALL ALmixer_IsInitialized();

extern DECLSPEC Uint32 SDLCALL ALmixer_GetFrequency();

extern DECLSPEC Sint32 SDLCALL ALmixer_AllocateChannels(Sint32 numchans);
extern DECLSPEC Sint32 SDLCALL ALmixer_ReserveChannels(Sint32 num);

extern DECLSPEC ALmixer_Data * SDLCALL ALmixer_LoadSample_RW(SDL_RWops* rwops, const char* fileext, Uint32 buffersize, Uint8 decode_mode, Uint32 max_queue_buffers, Uint32 num_startup_buffers, Uint8 access_data);


#define ALmixer_LoadStream_RW(rwops,fileext,buffersize,max_queue_buffers,num_startup_buffers,access_data) ALmixer_LoadSample_RW(rwops,fileext,buffersize,ALMIXER_DECODE_STREAM, max_queue_buffers, num_startup_buffers,access_data)

#define ALmixer_LoadAll_RW(rwops,fileext,buffersize,access_data) ALmixer_LoadSample_RW(rwops,fileext,buffersize,ALMIXER_DECODE_ALL, 0, 0,access_data)



extern DECLSPEC ALmixer_Data * SDLCALL ALmixer_LoadSample(const char* filename, Uint32 buffersize, Uint8 decode_mode, Uint32 max_queue_buffers, Uint32 num_startup_buffers, Uint8 access_data);


#define ALmixer_LoadStream(filename,buffersize,max_queue_buffers,num_startup_buffers,access_data) ALmixer_LoadSample(filename,buffersize,ALMIXER_DECODE_STREAM, max_queue_buffers, num_startup_buffers,access_data)

#define ALmixer_LoadAll(filename,buffersize,access_data) ALmixer_LoadSample(filename,buffersize,ALMIXER_DECODE_ALL, 0, 0,access_data)


extern DECLSPEC ALmixer_Data * SDLCALL ALmixer_LoadSample_RAW_RW(SDL_RWops* rwops, const char* fileext, Sound_AudioInfo* desired, Uint32 buffersize, Uint8 decode_mode, Uint32 max_queue_buffers, Uint32 num_startup_buffers, Uint8 access_data);

#define ALmixer_LoadStream_RAW_RW(rwops,fileext,desired,buffersize,max_queue_buffers,num_startup_buffers,access_data) ALmixer_LoadSample_RAW_RW(rwops,fileext,desired,buffersize,ALMIXER_DECODE_STREAM, max_queue_buffers, num_startup_buffers,access_data)

#define ALmixer_LoadAll_RAW_RW(rwops,fileext,desired,buffersize,access_data) ALmixer_LoadSample_RAW_RW(rwops,fileext,desired,buffersize,ALMIXER_DECODE_ALL, 0, 0,access_data)

extern DECLSPEC ALmixer_Data * SDLCALL ALmixer_LoadSample_RAW(const char* filename, Sound_AudioInfo* desired, Uint32 buffersize, Uint8 decode_mode, Uint32 max_queue_buffers, Uint32 num_startup_buffers, Uint8 access_data);



extern DECLSPEC void SDLCALL ALmixer_FreeData(ALmixer_Data* data);

extern DECLSPEC Sint32 SDLCALL ALmixer_GetTotalTime(ALmixer_Data* data);


/* If not using threads, this function must be periodically called
 * to poll ALmixer to force streamed music and other events to
 * take place. If threads are enabled, then this function just
 * returns 0.
 */
extern DECLSPEC Sint32 SDLCALL ALmixer_Update();




/* Play a sound on a channel with a time limit */
extern DECLSPEC Sint32 SDLCALL ALmixer_PlayChannelTimed(Sint32 channel, ALmixer_Data* data, Sint32 loops, Sint32 ticks);

/* The same as above, but the sound is played without time limits */ 
#define ALmixer_PlayChannel(channel,data,loops) ALmixer_PlayChannelTimed(channel,data,loops,-1)
/* These functions are the same as PlayChannel*(), but use sources 
 * instead of channels
 */
extern DECLSPEC ALuint SDLCALL ALmixer_PlaySourceTimed(ALuint source, ALmixer_Data* data, Sint32 loops, Sint32 ticks);

#define ALmixer_PlaySource(source,data,loops) ALmixer_PlaySourceTimed(source,data,loops,-1)

/* This function will look up the source for the corresponding channel.
 * If -1 is supplied, it will try to return a source not in use 
 * Must return 0 on error instead of -1 because of unsigned int 
 */
extern DECLSPEC ALuint SDLCALL ALmixer_GetSource(Sint32 channel);
/* This function will look up the channel for the corresponding source.
 * If -1 is supplied, it will try to return the first channel not in use. 
 * Returns -1 on error, or the channel.
 */
extern DECLSPEC Sint32 SDLCALL ALmixer_GetChannel(ALuint source);

extern DECLSPEC Sint32 SDLCALL ALmixer_FindFreeChannel(Sint32 start_channel);

extern DECLSPEC void SDLCALL ALmixer_ChannelFinished(void (*channel_finished)(Sint32 channel, void* userdata), void* userdata);
extern DECLSPEC void SDLCALL ALmixer_ChannelData(void (*channel_data)(Sint32 which_chan, Uint8* data, Uint32 num_bytes, Uint32 frequency, Uint8 channels, Uint8 bitdepth, Uint16 format, Uint8 decode_mode));


extern DECLSPEC Sint32 SDLCALL ALmixer_HaltChannel(Sint32 channel);
extern DECLSPEC Sint32 SDLCALL ALmixer_HaltSource(ALuint source);


extern DECLSPEC Sint32 SDLCALL ALmixer_RewindData(ALmixer_Data* data);

/* If decoded all, rewind will instantly rewind it. Data is not 
 * affected, so it will start at the "Seek"'ed positiond.
 * Streamed data will rewind the actual data, but the effect
 * will not be noticed until the currently buffered data is played.
 * Use Halt before this call for instantaneous changes
 */
extern DECLSPEC Sint32 SDLCALL ALmixer_RewindChannel(Sint32 channel);
extern DECLSPEC Sint32 SDLCALL ALmixer_RewindSource(ALuint source);

extern DECLSPEC Sint32 SDLCALL ALmixer_PauseChannel(Sint32 channel);
extern DECLSPEC Sint32 SDLCALL ALmixer_PauseSource(ALuint source);

extern DECLSPEC Sint32 SDLCALL ALmixer_ResumeChannel(Sint32 channel);
extern DECLSPEC Sint32 SDLCALL ALmixer_ResumeSource(ALuint source);

extern DECLSPEC Sint32 SDLCALL ALmixer_Seek(ALmixer_Data* data, Uint32 msec);


extern DECLSPEC Sint32 SDLCALL ALmixer_FadeInChannelTimed(Sint32 channel, ALmixer_Data* data, Sint32 loops, Uint32 fade_ticks, Sint32 expire_ticks);

#define ALmixer_FadeInChannel(channel,data,loops,fade_ticks) ALmixer_FadeInChannelTimed(channel,data,loops,fade_ticks,-1)

extern DECLSPEC ALuint SDLCALL ALmixer_FadeInSourceTimed(ALuint source, ALmixer_Data* data, Sint32 loops, Uint32 fade_ticks, Sint32 expire_ticks);

#define ALmixer_FadeInSource(source,data,loops,fade_ticks) ALmixer_FadeInSourceTimed(source,data,loops,fade_ticks,-1)

extern DECLSPEC Sint32 SDLCALL ALmixer_FadeOutChannel(Sint32 channel, Uint32 ticks);
extern DECLSPEC Sint32 SDLCALL ALmixer_FadeOutSource(ALuint source, Uint32 ticks);

extern DECLSPEC Sint32 SDLCALL ALmixer_FadeChannel(Sint32 channel, Uint32 ticks, ALfloat volume);
extern DECLSPEC Sint32 SDLCALL ALmixer_FadeSource(ALuint source, Uint32 ticks, ALfloat volume);

extern DECLSPEC Sint32 SDLCALL ALmixer_SetMaxVolumeChannel(Sint32 channel, ALfloat volume);
extern DECLSPEC Sint32 SDLCALL ALmixer_SetMaxVolumeSource(ALuint source, ALfloat volume);
extern DECLSPEC ALfloat SDLCALL ALmixer_GetMaxVolumeChannel(Sint32 channel);
extern DECLSPEC ALfloat SDLCALL ALmixer_GetMaxVolumeSource(ALuint source);

extern DECLSPEC Sint32 SDLCALL ALmixer_SetMinVolumeChannel(Sint32 channel, ALfloat volume);
extern DECLSPEC Sint32 SDLCALL ALmixer_SetMinVolumeSource(ALuint source, ALfloat volume);
extern DECLSPEC ALfloat SDLCALL ALmixer_GetMinVolumeChannel(Sint32 channel);
extern DECLSPEC ALfloat SDLCALL ALmixer_GetMinVolumeSource(ALuint source);


extern DECLSPEC Sint32 SDLCALL ALmixer_SetMasterVolume(ALfloat volume);
extern DECLSPEC ALfloat SDLCALL ALmixer_GetMasterVolume();


extern DECLSPEC Sint32 SDLCALL ALmixer_ExpireChannel(Sint32 channel, Sint32 ticks);
extern DECLSPEC Sint32 SDLCALL ALmixer_ExpireSource(ALuint source, Sint32 ticks);

extern DECLSPEC Sint32 SDLCALL ALmixer_QueryChannel(Sint32 channel);
extern DECLSPEC Sint32 SDLCALL ALmixer_QuerySource(ALuint source);
extern DECLSPEC Sint32 SDLCALL ALmixer_PlayingChannel(Sint32 channel);
extern DECLSPEC Sint32 SDLCALL ALmixer_PlayingSource(ALuint source);
extern DECLSPEC Sint32 SDLCALL ALmixer_PausedChannel(Sint32 channel);
extern DECLSPEC Sint32 SDLCALL ALmixer_PausedSource(ALuint source);

extern DECLSPEC Sint32 SDLCALL ALmixer_CountAllFreeChannels();
extern DECLSPEC Sint32 SDLCALL ALmixer_CountUnreservedFreeChannels();
extern DECLSPEC Sint32 SDLCALL ALmixer_CountAllUsedChannels();
extern DECLSPEC Sint32 SDLCALL ALmixer_CountUnreservedUsedChannels();
#define ALmixer_CountTotalChannels() ALmixer_AllocateChannels(-1)
#define ALmixer_CountReservedChannels() ALmixer_ReserveChannels(-1)



/* For testing */
#if 0
extern DECLSPEC void SDLCALL ALmixer_Output_Attributes();
#endif
extern DECLSPEC void SDLCALL ALmixer_Output_Decoders();
extern DECLSPEC void SDLCALL ALmixer_Output_OpenAL_Info();

#if 0



extern DECLSPEC Uint32 SDLCALL ALmixer_Volume(Sint32 channel, Sint32 volume);


/* I'm going to blindly throw in the Mixer effects sections and
 * hope they work.
 */
#define ALmixer_EffectFunc_t 			Mix_EffectFunc_t
#define ALmixer_EffectDone_t 			Mix_EffectDone_t
/*
#define ALmixer_RegisterEffect 			Mix_RegisterEffect
#define ALmixer_UnregisterEffect			Mix_UnregisterEffect
#define ALmixer_UnregisterAllEffects		Mix_RegisterEffect
*/

#define ALmixer_SetPostMix				Mix_SetPostMix
#define ALmixer_SetPanning				Mix_SetPanning
#define ALmixer_SetDistance 				Mix_SetDistance
#define ALmixer_SetPosition				Mix_SetPosition
#define ALmixer_SetReverseStereo			Mix_SetReverseStereo

/* Unfortunately, effects have a nasty behavior of unregistering 
 * themselves after the channel finishes. This is incompatible
 * with the streaming system that this library uses.
 * Implementing a proper effects system will take more time.
 * For now, I need to be able to retrieve the playing data
 * for an oscilloscope, so I am hacking together a 1 effect
 * system. You can't have more than one.
 */

extern DECLSPEC Sint32 SDLCALL ALmixer_RegisterEffect(Sint32 chan, ALmixer_EffectFunc_t f, ALmixer_EffectDone_t d, void* arg);

extern DECLSPEC Sint32 SDLCALL ALmixer_UnregisterEffect(Sint32 chan, ALmixer_EffectFunc_t f);

extern DECLSPEC Sint32 SDLCALL ALmixer_UnregisterAllEffects(Sint32 chan);

#endif




/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif
/*
#include "close_code.h"
*/

#endif /* _SDL_ALMIXER_H_ */

/* end of SDL_ALmixer.h ... */


