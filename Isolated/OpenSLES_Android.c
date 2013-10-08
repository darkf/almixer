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
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include "SoundDecoder.h"
#include "SoundDecoder_Internal.h"

/*
typedef struct OpenSLESFileContainer
{
	AudioFileID* audioFileID;
	ExtAudioFileRef extAudioFileRef;
	AudioStreamBasicDescription* outputFormat;
} OpenSLESFileContainer;
*/

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


static int OpenSLES_init(void)
{
    return(1);  /* always succeeds. */
} /* OpenSLES_init */


static void OpenSLES_quit(void)
{
    /* it's a no-op. */
} /* OpenSLES_quit */

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

static int OpenSLES_open(Sound_Sample* sound_sample, const char* file_ext)
{
	Sound_SampleInternal* internal_sample = (Sound_SampleInternal*) sound_sample->opaque;

	return(1);
} /* OpenSLES_open */


static void OpenSLES_close(Sound_Sample* sound_sample)
{
	Sound_SampleInternal* internal_sample = (Sound_SampleInternal*) sound_sample->opaque;

} /* OpenSLES_close */

static size_t OpenSLES_read(Sound_Sample* sound_sample)
{
	Sound_SampleInternal* internal_sample = (Sound_SampleInternal*) sound_sample->opaque;
	
	return(1);
} /* OpenSLES_read */

static int OpenSLES_rewind(Sound_Sample* sound_sample)
{
	Sound_SampleInternal* internal_sample = (Sound_SampleInternal*) sound_sample->opaque;
	
	return(1);
} /* OpenSLES_rewind */

/* Note: I found this tech note:
 http://developer.apple.com/library/mac/#qa/qa2009/qa1678.html
 I don't know if this applies to us. So far, I haven't noticed the problem,
 so I haven't applied any of the techniques.
 */
static int OpenSLES_seek(Sound_Sample* sound_sample, size_t ms)
{
	Sound_SampleInternal* internal_sample = (Sound_SampleInternal*) sound_sample->opaque;

	return(1);
} /* OpenSLES_seek */

#endif /* __ANDROID__ */


#endif /* ALMIXER_COMPILE_WITHOUT_SDL */

