#ifdef SWIGJAVASCRIPT
    // Javascript doesn't have an official module system, so currently both SWIG JSCore and v8 
    // implementations put the module name in the global namespace.
    // We are worried about accidental name collisions since it is not obvious a global variable gets taken.
    // So we are going to try to make the name unique.
    #ifdef TITANIUM
        // Our module fuses with a lot of additional Titanium infrastructure which makes it distinct.
        %module co_lanica_almixer
    #else
//        %module net_playcontrol_almixer
        %module ALmixer
    #endif
#else
	%module ALmixer
#endif


%module ALmixer
%{
 /* Includes the header in the wrapper code */
 #include "ALmixer.h"
 #include "ALmixer_RWops.h"
%}

/* This was just a SWIG experiment.
 %extend ALmixer_version { 
    void printversion() {
        printf("Linked version: %d.%d.%d\n", $self->major,$self->minor,$self->patch);
    }
};
*/
// Some systems put things in a module namespace, others do not.
// For modules that put stuff in a namespace, the ALmixer_ C prefix is redundant.
#ifndef SWIGTCL
// Everything below this line will strip the ALmixer_ prefix off the exposed name
%rename("%(strip:[ALmixer_])s") ""; // wxHello -> Hello; FooBar -> FooBar
#endif

%include ALmixer_Callback.i

/* 
 * Typemap for 
 * ALmixer_AudioInfo* desired_format
 * 
 * C  -> desired_format = {format, channels, rate};
 * JS -> desired_format = [format, channels, rate];
 *
 * var AUDIO_S16SYS = 0x9010;
 * var desired_format = [AUDIO_S16SYS, 1, 22050];
 */
#ifdef SWIG_JAVASCRIPT_V8
%fragment("SWIG_JSCGetIntProperty",    "header", fragment=SWIG_AsVal_frag(int)) {}
%fragment("SWIG_JSCGetNumberProperty", "header", fragment=SWIG_AsVal_frag(double)) {}

%typemap(in, fragment="SWIG_JSCGetIntProperty")  ALmixer_AudioInfo* desired_format
    (int length = 0, v8::Local<v8::Array> array, v8::Local<v8::Value> jsvalue0, v8::Local<v8::Value> jsvalue1, v8::Local<v8::Value> jsvalue2, int i = 0, int res, int temp) {
  if ($input->IsArray()) {
    // Convert into Array
    array = v8::Local<v8::Array>::Cast($input);
    length = array->Length();

    $1  = ($*1_ltype *)malloc(sizeof($*1_ltype));

    if (length >= 3) {
        jsvalue0 = array->Get(0);
        jsvalue1 = array->Get(1);
        jsvalue2 = array->Get(2);

        res = SWIG_AsVal(int)(jsvalue0, &temp);
        $1->format = temp;
        res = SWIG_AsVal(int)(jsvalue1, &temp);
        $1->channels = temp;
        res = SWIG_AsVal(int)(jsvalue2, &temp);
        $1->rate = temp;
    } else {
      SWIG_exception_fail(SWIG_ERROR, "$input.length should be >= 3");
    }
  } else {
    SWIG_exception_fail(SWIG_ERROR, "$input is not JSObjectRef");
  }
}

%typemap(freearg) ALmixer_AudioInfo* desired_format {
    free($1);
}
#elif SWIG_JAVASCRIPT_JSC
%include JSCore_fragment.i
%fragment("SWIG_JSObjectGetNumberProperty",  "header", fragment="SWIG_JSObjectGetNumber,SWIG_JSObjectGetNamedProperty") {}
%typemap(in, fragment="SWIG_JSObjectGetNumberProperty") ALmixer_AudioInfo* desired_format (int length, JSObjectRef array, JSValueRef jsvalue0, JSValueRef jsvalue1, JSValueRef jsvalue2, int i, int res) {
  if (JSValueIsObject(context, $input)) {
    // Convert into Array
    array  = JSValueToObject(context, $input, NULL);
    length = (int)SWIG_JSObjectGetNumber(context, 
        SWIG_JSObjectGetNamedProperty(context, array, "length", NULL), NULL);

    $1  = ($*1_ltype *)malloc(sizeof($*1_ltype));

    if (length >= 3) {
      jsvalue0 = JSObjectGetPropertyAtIndex(context, array, 0, NULL);
      jsvalue1 = JSObjectGetPropertyAtIndex(context, array, 1, NULL);
      jsvalue2 = JSObjectGetPropertyAtIndex(context, array, 2, NULL);

      $1->format   = SWIG_JSObjectGetNumber(context, jsvalue0, NULL);
      $1->channels = SWIG_JSObjectGetNumber(context, jsvalue1, NULL);
      $1->rate     = SWIG_JSObjectGetNumber(context, jsvalue2, NULL);
    } else  {
      SWIG_exception_fail(SWIG_ERROR, "$input.length should be >= 3");
    }
  } else {
    SWIG_exception_fail(SWIG_ERROR, "$input is not JSObjectRef");
  }
}
%typemap(freearg) ALmixer_AudioInfo* desired_format {
  free($1);
}
#endif


/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

/** For SWIG, change char to bool for the ALboolean type. */
%apply ALboolean { bool };
typedef bool ALboolean;

#ifndef ALMIXER_SWIG_INCLUDE_OPENAL_BINDINGS
/* These are essential OpenAL types that must be defined because ALmixer uses them directly. */

typedef bool ALboolean;
/** For SWIG, change char to bool for the ALboolean type. */
%apply ALboolean { bool };

/** character */
typedef char ALchar;

/** signed 8-bit 2's complement integer */
/* Apple user char, OpenAL Soft uses signed char which will trigger SWIG warnings by SWIG if OpenAL is directly bound. */
// typedef signed char ALbyte;
typedef char ALbyte;

/** unsigned 8-bit integer */
typedef unsigned char ALubyte;

/** signed 16-bit 2's complement integer */
typedef short ALshort;

/** unsigned 16-bit integer */
typedef unsigned short ALushort;

/** signed 32-bit 2's complement integer */
typedef int ALint;

/** unsigned 32-bit integer */
typedef unsigned int ALuint;

/** non-negative 32-bit binary integer size */
typedef int ALsizei;

/** enumerated 32-bit value */
typedef int ALenum;

/** 32-bit IEEE754 floating-point */
typedef float ALfloat;

/** 64-bit IEEE754 floating-point */
typedef double ALdouble;

/** void type (for opaque pointers only) */
typedef void ALvoid;


/* Enumerant values begin at column 50. No tabs. */

/* "no distance model" or "no buffer" */
#define AL_NONE                                   0
#endif /* ifndef ALMIXER_SWIG_INCLUDE_OPENAL_BINDINGS */

/* Redefine for SWIG native boolean */
/*
/* Boolean False. */
/*
#define AL_FALSE                                  0
*/
/* Boolean True. */
/*
#define AL_TRUE                                   1
*/
typedef false AL_FALSE;
typedef true AL_TRUE;


#if 0  /* OpenAL defines */


/* al.h */
#define OPENAL
#define ALAPI AL_API
#define ALAPIENTRY AL_APIENTRY
#define AL_INVALID                                (-1)
#define AL_ILLEGAL_ENUM                           AL_INVALID_ENUM
#define AL_ILLEGAL_COMMAND                        AL_INVALID_OPERATION

#define AL_VERSION_1_0
#define AL_VERSION_1_1

/** Indicate Source has relative coordinates. */
#define AL_SOURCE_RELATIVE                        0x202

/**
 * Directional source, inner cone angle, in degrees.
 * Range:    [0-360] 
 * Default:  360
 */
#define AL_CONE_INNER_ANGLE                       0x1001

/**
 * Directional source, outer cone angle, in degrees.
 * Range:    [0-360] 
 * Default:  360
 */
#define AL_CONE_OUTER_ANGLE                       0x1002

/**
 * Specify the pitch to be applied at source.
 * Range:   [0.5-2.0]
 * Default: 1.0
 */
#define AL_PITCH                                  0x1003
  
/** 
 * Specify the current location in three dimensional space.
 * OpenAL, like OpenGL, uses a right handed coordinate system,
 *  where in a frontal default view X (thumb) points right, 
 *  Y points up (index finger), and Z points towards the
 *  viewer/camera (middle finger). 
 * To switch from a left handed coordinate system, flip the
 *  sign on the Z coordinate.
 * Listener position is always in the world coordinate system.
 */ 
#define AL_POSITION                               0x1004
  
/** Specify the current direction. */
#define AL_DIRECTION                              0x1005
  
/** Specify the current velocity in three dimensional space. */
#define AL_VELOCITY                               0x1006

/**
 * Indicate whether source is looping.
 * Type: ALboolean?
 * Range:   [AL_TRUE, AL_FALSE]
 * Default: FALSE.
 */
#define AL_LOOPING                                0x1007

/**
 * Indicate the buffer to provide sound samples. 
 * Type: ALuint.
 * Range: any valid Buffer id.
 */
#define AL_BUFFER                                 0x1009
  
/**
 * Indicate the gain (volume amplification) applied. 
 * Type:   ALfloat.
 * Range:  ]0.0-  ]
 * A value of 1.0 means un-attenuated/unchanged.
 * Each division by 2 equals an attenuation of -6dB.
 * Each multiplicaton with 2 equals an amplification of +6dB.
 * A value of 0.0 is meaningless with respect to a logarithmic
 *  scale; it is interpreted as zero volume - the channel
 *  is effectively disabled.
 */
#define AL_GAIN                                   0x100A

/*
 * Indicate minimum source attenuation
 * Type: ALfloat
 * Range:  [0.0 - 1.0]
 *
 * Logarthmic
 */
#define AL_MIN_GAIN                               0x100D

/**
 * Indicate maximum source attenuation
 * Type: ALfloat
 * Range:  [0.0 - 1.0]
 *
 * Logarthmic
 */
#define AL_MAX_GAIN                               0x100E

/**
 * Indicate listener orientation.
 *
 * at/up 
 */
#define AL_ORIENTATION                            0x100F

/**
 * Source state information.
 */
#define AL_SOURCE_STATE                           0x1010
#define AL_INITIAL                                0x1011
#define AL_PLAYING                                0x1012
#define AL_PAUSED                                 0x1013
#define AL_STOPPED                                0x1014

/**
 * Buffer Queue params
 */
#define AL_BUFFERS_QUEUED                         0x1015
#define AL_BUFFERS_PROCESSED                      0x1016

/**
 * Source buffer position information
 */
#define AL_SEC_OFFSET                             0x1024
#define AL_SAMPLE_OFFSET                          0x1025
#define AL_BYTE_OFFSET                            0x1026

/*
 * Source type (Static, Streaming or undetermined)
 * Source is Static if a Buffer has been attached using AL_BUFFER
 * Source is Streaming if one or more Buffers have been attached using alSourceQueueBuffers
 * Source is undetermined when it has the NULL buffer attached
 */
#define AL_SOURCE_TYPE                            0x1027
#define AL_STATIC                                 0x1028
#define AL_STREAMING                              0x1029
#define AL_UNDETERMINED                           0x1030

/** Sound samples: format specifier. */
#define AL_FORMAT_MONO8                           0x1100
#define AL_FORMAT_MONO16                          0x1101
#define AL_FORMAT_STEREO8                         0x1102
#define AL_FORMAT_STEREO16                        0x1103

/**
 * source specific reference distance
 * Type: ALfloat
 * Range:  0.0 - +inf
 *
 * At 0.0, no distance attenuation occurs.  Default is
 * 1.0.
 */
#define AL_REFERENCE_DISTANCE                     0x1020

/**
 * source specific rolloff factor
 * Type: ALfloat
 * Range:  0.0 - +inf
 *
 */
#define AL_ROLLOFF_FACTOR                         0x1021

/**
 * Directional source, outer cone gain.
 *
 * Default:  0.0
 * Range:    [0.0 - 1.0]
 * Logarithmic
 */
#define AL_CONE_OUTER_GAIN                        0x1022

/**
 * Indicate distance above which sources are not
 * attenuated using the inverse clamped distance model.
 *
 * Default: +inf
 * Type: ALfloat
 * Range:  0.0 - +inf
 */
#define AL_MAX_DISTANCE                           0x1023

/** 
 * Sound samples: frequency, in units of Hertz [Hz].
 * This is the number of samples per second. Half of the
 *  sample frequency marks the maximum significant
 *  frequency component.
 */
#define AL_FREQUENCY                              0x2001
#define AL_BITS                                   0x2002
#define AL_CHANNELS                               0x2003
#define AL_SIZE                                   0x2004

/**
 * Buffer state.
 *
 * Not supported for public use (yet).
 */
#define AL_UNUSED                                 0x2010
#define AL_PENDING                                0x2011
#define AL_PROCESSED                              0x2012


/** Errors: No Error. */
#define AL_NO_ERROR                               AL_FALSE

/** 
 * Invalid Name paramater passed to AL call.
 */
#define AL_INVALID_NAME                           0xA001

/** 
 * Invalid parameter passed to AL call.
 */
#define AL_INVALID_ENUM                           0xA002

/** 
 * Invalid enum parameter value.
 */
#define AL_INVALID_VALUE                          0xA003

/** 
 * Illegal call.
 */
#define AL_INVALID_OPERATION                      0xA004

  
/**
 * No mojo.
 */
#define AL_OUT_OF_MEMORY                          0xA005


/** Context strings: Vendor Name. */
#define AL_VENDOR                                 0xB001
#define AL_VERSION                                0xB002
#define AL_RENDERER                               0xB003
#define AL_EXTENSIONS                             0xB004

/** Global tweakage. */

/**
 * Doppler scale.  Default 1.0
 */
#define AL_DOPPLER_FACTOR                         0xC000

/**
 * Tweaks speed of propagation.
 */
#define AL_DOPPLER_VELOCITY                       0xC001

/**
 * Speed of Sound in units per second
 */
#define AL_SPEED_OF_SOUND                         0xC003

/**
 * Distance models
 *
 * used in conjunction with DistanceModel
 *
 * implicit: NONE, which disances distance attenuation.
 */
#define AL_DISTANCE_MODEL                         0xD000
#define AL_INVERSE_DISTANCE                       0xD001
#define AL_INVERSE_DISTANCE_CLAMPED               0xD002
#define AL_LINEAR_DISTANCE                        0xD003
#define AL_LINEAR_DISTANCE_CLAMPED                0xD004
#define AL_EXPONENT_DISTANCE                      0xD005
#define AL_EXPONENT_DISTANCE_CLAMPED              0xD006


/**
 * Priority
 *
 * Apportable Extension.
 * Used to prevent dynamic throttling of this source
 *
 */
#define AL_PRIORITY                               0xE001
#define AL_PRIORITY_SLOTS                         0xE002
#endif /* OpenAL defines */


#ifndef DOXYGEN_SHOULD_IGNORE_THIS

#ifdef SWIG
#define ALMIXER_DECLSPEC
    #define ALMIXER_CALL
#else
	#if defined(_WIN32)
		#if defined(ALMIXER_BUILD_LIBRARY)
			#define ALMIXER_DECLSPEC __declspec(dllexport)
		#else
			#define ALMIXER_DECLSPEC
		#endif
	#else
		#if defined(ALMIXER_BUILD_LIBRARY)
			#if defined (__GNUC__) && __GNUC__ >= 4
				#define ALMIXER_DECLSPEC __attribute__((visibility("default")))
			#else
				#define ALMIXER_DECLSPEC
			#endif
		#else
			#define ALMIXER_DECLSPEC
		#endif
	#endif

	#if defined(_WIN32)
		#define ALMIXER_CALL __cdecl
	#else
		#define ALMIXER_CALL
	#endif
#endif
#endif /* DOXYGEN_SHOULD_IGNORE_THIS */


typedef struct ALmixer_version
{
	ALubyte major;
	ALubyte minor;
	ALubyte patch;
} ALmixer_version;

/* Printable format: "%d.%d.%d", MAJOR, MINOR, PATCHLEVEL
 */
#define ALMIXER_MAJOR_VERSION       0
#define ALMIXER_MINOR_VERSION       2
#define ALMIXER_PATCHLEVEL          0

#define ALMIXER_GET_COMPILED_VERSION(X)                                           \
    {                                                                       \
        (X)->major = ALMIXER_MAJOR_VERSION;                          \
        (X)->minor = ALMIXER_MINOR_VERSION;                          \
        (X)->patch = ALMIXER_PATCHLEVEL;                             \
    }

extern ALMIXER_DECLSPEC const ALmixer_version* ALMIXER_CALL ALmixer_GetLinkedVersion(void);

extern ALMIXER_DECLSPEC const char* ALMIXER_CALL ALmixer_GetError(void);
extern ALMIXER_DECLSPEC void ALMIXER_CALL ALmixer_SetError(const char *fmt, ...);
    
extern ALMIXER_DECLSPEC ALuint ALMIXER_CALL ALmixer_GetTicks(void);
extern ALMIXER_DECLSPEC void ALMIXER_CALL ALmixer_Delay(ALuint milliseconds_delay);

%include "ALmixer_RWops.i"


#define ALMIXER_DEFAULT_FREQUENCY   0
#define ALMIXER_DEFAULT_REFRESH     0
#define ALMIXER_DEFAULT_NUM_CHANNELS    16
#define ALMIXER_DEFAULT_NUM_SOURCES     ALMIXER_DEFAULT_NUM_CHANNELS

extern ALMIXER_DECLSPEC ALboolean ALMIXER_CALL ALmixer_Init(ALuint playback_frequency=0, ALuint num_sources=0, ALuint refresh_rate=0);

extern ALMIXER_DECLSPEC ALboolean ALMIXER_CALL ALmixer_InitContext(ALuint playback_frequency=0, ALuint refresh_rate=0);

extern ALMIXER_DECLSPEC ALboolean ALMIXER_CALL ALmixer_InitMixer(ALuint num_sources=0);

extern ALMIXER_DECLSPEC void ALMIXER_CALL ALmixer_BeginInterruption(void);

extern ALMIXER_DECLSPEC void ALMIXER_CALL ALmixer_EndInterruption(void);

extern ALMIXER_DECLSPEC ALboolean ALmixer_IsInInterruption(void);

extern ALMIXER_DECLSPEC void ALMIXER_CALL ALmixer_SuspendUpdates(void);

extern ALMIXER_DECLSPEC void ALMIXER_CALL ALmixer_ResumeUpdates(void);

extern ALMIXER_DECLSPEC ALboolean ALmixer_AreUpdatesSuspended(void);

extern ALMIXER_DECLSPEC void ALMIXER_CALL ALmixer_SuspendPlayingState(void);
extern ALMIXER_DECLSPEC void ALMIXER_CALL ALmixer_ResumePlayingState(void);
extern ALMIXER_DECLSPEC ALboolean ALmixer_IsPlayingStateSuspended(void);
    


extern ALMIXER_DECLSPEC void ALMIXER_CALL ALmixer_Quit(void);
extern ALMIXER_DECLSPEC void ALMIXER_CALL ALmixer_QuitWithoutFreeData(void);

extern ALMIXER_DECLSPEC ALboolean ALMIXER_CALL ALmixer_IsInitialized(void);

extern ALMIXER_DECLSPEC ALuint ALMIXER_CALL ALmixer_GetFrequency(void);

extern ALMIXER_DECLSPEC ALint ALMIXER_CALL ALmixer_AllocateChannels(ALint num_chans);

extern ALMIXER_DECLSPEC ALint ALMIXER_CALL ALmixer_ReserveChannels(ALint number_of_reserve_channels);

extern ALMIXER_DECLSPEC ALint ALMIXER_CALL ALmixer_Update(void);

/*
#define ALmixer_AudioInfo   Sound_AudioInfo
*/

/*
#define ALMIXER_DEFAULT_BUFFERSIZE 32768
#define ALMIXER_DEFAULT_BUFFERSIZE 16384 
*/
#define ALMIXER_DEFAULT_BUFFERSIZE 8192

/* You probably never need to use these macros directly. */
#ifndef ALMIXER_DISABLE_PREDECODED_PRECOMPUTE_BUFFER_SIZE_OPTIMIZATION
    #define ALMIXER_DEFAULT_PREDECODED_BUFFERSIZE ALMIXER_DEFAULT_BUFFERSIZE * 4
#else
    #define ALMIXER_DEFAULT_PREDECODED_BUFFERSIZE 4096
#endif

/*
#define ALMIXER_DEFAULT_QUEUE_BUFFERS 5
*/
#define ALMIXER_DEFAULT_QUEUE_BUFFERS 12
/**
 * Specifies the number of queue buffers initially filled when first loading a stream.
 * Default startup buffers should be at least 1. */
#define ALMIXER_DEFAULT_STARTUP_BUFFERS 4
/*
#define ALMIXER_DEFAULT_STARTUP_BUFFERS 2 
*/
#define ALMIXER_DEFAULT_BUFFERS_TO_QUEUE_PER_UPDATE_PASS 2

/*
#define ALMIXER_DECODE_STREAM   0
#define ALMIXER_DECODE_ALL      1
*/

%nodefaultctor ALmixer_Data;
struct ALmixer_Data
{
};
typedef struct ALmixer_Data ALmixer_Data;
%extend ALmixer_Data
{
	~ALmixer_Data()
	{
		ALmixer_FreeData($self);
	}
};

typedef struct ALmixer_AudioInfo ALmixer_AudioInfo;

struct ALmixer_AudioInfo
{
    ALushort format;  /**< Equivalent of SDL_AudioSpec.format. */
    ALubyte channels; /**< Number of sound channels. 1 == mono, 2 == stereo. */
    ALuint rate;    /**< Sample rate; frequency of sample points per second. */
};

%newobject ALmixer_LoadSample_RW;
extern ALMIXER_DECLSPEC ALmixer_Data* ALMIXER_CALL ALmixer_LoadSample_RW(ALmixer_RWops* rw_ops, const char* file_ext, ALuint buffer_size, ALboolean decode_mode_is_predecoded, ALuint max_queue_buffers, ALuint num_startup_buffers, ALuint suggested_number_of_buffers_to_queue_per_update_pass, ALuint access_data);

%newobject ALmixer_LoadStream_RW;
extern ALMIXER_DECLSPEC ALmixer_Data* ALMIXER_CALL ALmixer_LoadStream_RW(ALmixer_RWops* rw_ops, const char* file_ext, ALuint buffer_size, ALuint max_queue_buffers, ALuint num_startup_buffers, ALuint suggested_number_of_buffers_to_queue_per_update_pass, ALuint access_data);

%newobject ALmixer_LoadAll_RW;
extern ALMIXER_DECLSPEC ALmixer_Data* ALMIXER_CALL ALmixer_LoadAll_RW(struct ALmixer_RWops* rw_ops, const char* file_ext, ALuint access_data);

%newobject ALmixer_LoadSample;
extern ALMIXER_DECLSPEC ALmixer_Data * ALMIXER_CALL ALmixer_LoadSample(const char* file_name, ALuint buffer_size, ALboolean decode_mode_is_predecoded, ALuint max_queue_buffers, ALuint num_startup_buffers, ALuint suggested_number_of_buffers_to_queue_per_update_pass, ALuint access_data);

%newobject ALmixer_LoadStream;
extern ALMIXER_DECLSPEC ALmixer_Data* ALMIXER_CALL ALmixer_LoadStream(const char* file_name, ALuint buffer_size, ALuint max_queue_buffers, ALuint num_startup_buffers, ALuint suggested_number_of_buffers_to_queue_per_update_pass, ALuint access_data);

%newobject ALmixer_LoadAll;
extern ALMIXER_DECLSPEC ALmixer_Data* ALMIXER_CALL ALmixer_LoadAll(const char* file_name, ALuint access_data);

%newobject ALmixer_LoadSample_RAW_RW;
extern ALMIXER_DECLSPEC ALmixer_Data * ALMIXER_CALL ALmixer_LoadSample_RAW_RW(ALmixer_RWops* rw_ops, const char* file_ext, ALmixer_AudioInfo* desired_format, ALuint buffer_size, ALboolean decode_mode_is_predecoded, ALuint max_queue_buffers, ALuint num_startup_buffers, ALuint suggested_number_of_buffers_to_queue_per_update_pass, ALuint access_data);

%newobject ALmixer_LoadStream_RAW_RW;
extern ALMIXER_DECLSPEC ALmixer_Data* ALMIXER_CALL ALmixer_LoadStream_RAW_RW(struct ALmixer_RWops* rw_ops, const char* file_ext, ALmixer_AudioInfo* desired_format, ALuint buffer_size, ALuint max_queue_buffers, ALuint num_startup_buffers, ALuint suggested_number_of_buffers_to_queue_per_update_pass, ALuint access_data);

%newobject ALmixer_LoadAll_RAW_RW;
extern ALMIXER_DECLSPEC ALmixer_Data* ALMIXER_CALL ALmixer_LoadAll_RAW_RW(struct ALmixer_RWops* rw_ops, const char* file_ext, ALmixer_AudioInfo* desired_format, ALuint access_data);

%newobject ALmixer_LoadSample_RAW;
extern ALMIXER_DECLSPEC ALmixer_Data * ALMIXER_CALL ALmixer_LoadSample_RAW(const char* file_name, ALmixer_AudioInfo* desired_format, ALuint buffer_size, ALboolean decode_mode_is_predecoded, ALuint max_queue_buffers, ALuint num_startup_buffers, ALuint suggested_number_of_buffers_to_queue_per_update_pass, ALuint access_data);

%newobject ALmixer_LoadStream_RAW;
extern ALMIXER_DECLSPEC ALmixer_Data* ALMIXER_CALL ALmixer_LoadStream_RAW(const char* file_name, ALmixer_AudioInfo* desired_format, ALuint buffer_size, ALuint max_queue_buffers, ALuint num_startup_buffers, ALuint suggested_number_of_buffers_to_queue_per_update_pass, ALuint access_data);

%newobject ALmixer_LoadAll_RAW;
extern ALMIXER_DECLSPEC ALmixer_Data* ALMIXER_CALL ALmixer_LoadAll_RAW(const char* file_name, ALmixer_AudioInfo* desired_format, ALuint access_data);

%delobject ALmixer_FreeData;
extern ALMIXER_DECLSPEC void ALMIXER_CALL ALmixer_FreeData(ALmixer_Data* almixer_data);

extern ALMIXER_DECLSPEC ALboolean ALMIXER_CALL ALmixer_IsPredecoded(ALmixer_Data* almixer_data);

typedef void (*playback_finished_callback)(ALint which_channel, ALuint al_source, ALmixer_Data* almixer_data, ALboolean finished_naturally, void* ALmixer_SetPlaybackFinishedCallbackContainer);

extern ALMIXER_DECLSPEC void ALMIXER_CALL ALmixer_SetPlaybackFinishedCallback(playback_finished_callback func, void* ALmixer_SetPlaybackFinishedCallbackContainer);

extern ALMIXER_DECLSPEC void ALMIXER_CALL ALmixer_SetPlaybackDataCallback(void (*playback_data_callback)(ALint which_channel, ALuint al_source, ALbyte* pcm_data, ALuint num_bytes, ALuint frequency, ALubyte num_channels_in_sample, ALubyte bit_depth, ALboolean is_unsigned, ALboolean decode_mode_is_predecoded, ALuint length_in_msec, void* user_data), void* user_data);

extern ALMIXER_DECLSPEC ALint ALMIXER_CALL ALmixer_GetTotalTime(ALmixer_Data* almixer_data);

extern ALMIXER_DECLSPEC ALuint ALMIXER_CALL ALmixer_GetSource(ALint which_channel);

extern ALMIXER_DECLSPEC ALint ALMIXER_CALL ALmixer_GetChannel(ALuint al_source);

extern ALMIXER_DECLSPEC ALint ALMIXER_CALL ALmixer_FindFreeChannel(ALint start_channel);

extern ALMIXER_DECLSPEC ALint ALMIXER_CALL ALmixer_PlayChannelTimed(ALint which_channel, ALmixer_Data* almixer_data, ALint number_of_loops=0, ALint number_of_milliseconds=-1);

extern ALMIXER_DECLSPEC ALint ALMIXER_CALL ALmixer_PlayChannel(ALint which_channel, ALmixer_Data* almixer_data, ALint number_of_loops);

extern ALMIXER_DECLSPEC ALuint ALMIXER_CALL ALmixer_PlaySourceTimed(ALuint al_source, ALmixer_Data* almixer_data, ALint number_of_loops=0, ALint number_of_milliseconds=-1);

extern ALMIXER_DECLSPEC ALuint ALMIXER_CALL ALmixer_PlaySource(ALuint al_source, ALmixer_Data* almixer_data, ALint number_of_loops);

extern ALMIXER_DECLSPEC ALint ALMIXER_CALL ALmixer_HaltChannel(ALint which_channel);

extern ALMIXER_DECLSPEC ALint ALMIXER_CALL ALmixer_HaltSource(ALuint al_source);

extern ALMIXER_DECLSPEC ALboolean ALMIXER_CALL ALmixer_RewindData(ALmixer_Data* almixer_data);

extern ALMIXER_DECLSPEC ALint ALMIXER_CALL ALmixer_RewindChannel(ALint which_channel);
extern ALMIXER_DECLSPEC ALint ALMIXER_CALL ALmixer_RewindSource(ALuint al_source);

extern ALMIXER_DECLSPEC ALboolean ALMIXER_CALL ALmixer_SeekData(ALmixer_Data* almixer_data, ALuint msec_pos);

extern ALMIXER_DECLSPEC ALint ALMIXER_CALL ALmixer_SeekChannel(ALint which_channel, ALuint msec_pos);
extern ALMIXER_DECLSPEC ALint ALMIXER_CALL ALmixer_SeekSource(ALuint al_source, ALuint msec_pos);

extern ALMIXER_DECLSPEC ALint ALMIXER_CALL ALmixer_PauseChannel(ALint which_channel);
extern ALMIXER_DECLSPEC ALint ALMIXER_CALL ALmixer_PauseSource(ALuint al_source);

extern ALMIXER_DECLSPEC ALint ALMIXER_CALL ALmixer_ResumeChannel(ALint which_channel);

 extern ALMIXER_DECLSPEC ALint ALMIXER_CALL ALmixer_ResumeSource(ALuint al_source);

 
extern ALMIXER_DECLSPEC ALint ALMIXER_CALL ALmixer_ExpireChannel(ALint which_channel, ALint number_of_milliseconds);
extern ALMIXER_DECLSPEC ALint ALMIXER_CALL ALmixer_ExpireSource(ALuint al_source, ALint number_of_milliseconds);

extern ALMIXER_DECLSPEC ALint ALMIXER_CALL ALmixer_FadeInChannelTimed(ALint which_channel, ALmixer_Data* almixer_data, ALint number_of_loops, ALuint fade_ticks, ALint expire_ticks);

extern ALMIXER_DECLSPEC ALint ALMIXER_CALL ALmixer_FadeInChannel(ALint which_channel, ALmixer_Data* almixer_data, ALint number_of_loops, ALuint fade_ticks);

extern ALMIXER_DECLSPEC ALuint ALMIXER_CALL ALmixer_FadeInSourceTimed(ALuint al_source, ALmixer_Data* almixer_data, ALint number_of_loops, ALuint fade_ticks, ALint expire_ticks);

extern ALMIXER_DECLSPEC ALuint ALMIXER_CALL ALmixer_FadeInSource(ALuint al_source, ALmixer_Data* almixer_data, ALint number_of_loops, ALuint fade_ticks);

extern ALMIXER_DECLSPEC ALint ALMIXER_CALL ALmixer_FadeOutChannel(ALint which_channel, ALuint fade_ticks);

extern ALMIXER_DECLSPEC ALint ALMIXER_CALL ALmixer_FadeOutSource(ALuint al_source, ALuint fade_ticks);

extern ALMIXER_DECLSPEC ALint ALMIXER_CALL ALmixer_FadeChannel(ALint which_channel, ALuint fade_ticks, ALfloat volume);

extern ALMIXER_DECLSPEC ALint ALMIXER_CALL ALmixer_FadeSource(ALuint al_source, ALuint fade_ticks, ALfloat volume);

extern ALMIXER_DECLSPEC ALboolean ALMIXER_CALL ALmixer_SetVolumeChannel(ALint which_channel, ALfloat volume);

extern ALMIXER_DECLSPEC ALboolean ALMIXER_CALL ALmixer_SetVolumeSource(ALuint al_source, ALfloat volume);

extern ALMIXER_DECLSPEC ALfloat ALMIXER_CALL ALmixer_GetVolumeChannel(ALint which_channel);

extern ALMIXER_DECLSPEC ALfloat ALMIXER_CALL ALmixer_GetVolumeSource(ALuint al_source);

extern ALMIXER_DECLSPEC ALboolean ALMIXER_CALL ALmixer_SetMaxVolumeChannel(ALint which_channel, ALfloat volume);

extern ALMIXER_DECLSPEC ALboolean ALMIXER_CALL ALmixer_SetMaxVolumeSource(ALuint al_source, ALfloat volume);

extern ALMIXER_DECLSPEC ALfloat ALMIXER_CALL ALmixer_GetMaxVolumeChannel(ALint which_channel);

extern ALMIXER_DECLSPEC ALfloat ALMIXER_CALL ALmixer_GetMaxVolumeSource(ALuint al_source);

extern ALMIXER_DECLSPEC ALboolean ALMIXER_CALL ALmixer_SetMinVolumeChannel(ALint which_channel, ALfloat volume);

extern ALMIXER_DECLSPEC ALboolean ALMIXER_CALL ALmixer_SetMinVolumeSource(ALuint al_source, ALfloat volume);

extern ALMIXER_DECLSPEC ALfloat ALMIXER_CALL ALmixer_GetMinVolumeChannel(ALint which_channel);

extern ALMIXER_DECLSPEC ALfloat ALMIXER_CALL ALmixer_GetMinVolumeSource(ALuint al_source);

extern ALMIXER_DECLSPEC ALboolean ALMIXER_CALL ALmixer_SetMasterVolume(ALfloat new_volume);

 extern ALMIXER_DECLSPEC ALfloat ALMIXER_CALL ALmixer_GetMasterVolume(void);

extern ALMIXER_DECLSPEC ALint ALMIXER_CALL ALmixer_IsActiveChannel(ALint which_channel);

extern ALMIXER_DECLSPEC ALint ALMIXER_CALL ALmixer_IsActiveSource(ALuint al_source);

extern ALMIXER_DECLSPEC ALint ALMIXER_CALL ALmixer_IsPlayingChannel(ALint which_channel);

extern ALMIXER_DECLSPEC ALint ALMIXER_CALL ALmixer_IsPlayingSource(ALuint al_source);

extern ALMIXER_DECLSPEC ALint ALMIXER_CALL ALmixer_IsPausedChannel(ALint which_channel);

extern ALMIXER_DECLSPEC ALint ALMIXER_CALL ALmixer_IsPausedSource(ALuint al_source);

extern ALMIXER_DECLSPEC ALuint ALMIXER_CALL ALmixer_CountAllFreeChannels(void);

extern ALMIXER_DECLSPEC ALuint ALMIXER_CALL ALmixer_CountUnreservedFreeChannels(void);

extern ALMIXER_DECLSPEC ALuint ALMIXER_CALL ALmixer_CountAllUsedChannels(void);

extern ALMIXER_DECLSPEC ALuint ALMIXER_CALL ALmixer_CountUnreservedUsedChannels(void);


extern ALMIXER_DECLSPEC ALuint ALMIXER_CALL ALmixer_CountTotalChannels(void);


extern ALMIXER_DECLSPEC ALuint ALMIXER_CALL ALmixer_CountReservedChannels(void);


/* For testing */
#if 0
extern ALMIXER_DECLSPEC void ALMIXER_CALL ALmixer_OutputAttributes(void);
#endif
/** This function may be removed in the future. For debugging. Prints to stderr. Lists the decoders available. */ 
extern ALMIXER_DECLSPEC void ALMIXER_CALL ALmixer_OutputDecoders(void);
/** This function may be removed in the future. For debugging. Prints to stderr. */ 
extern ALMIXER_DECLSPEC void ALMIXER_CALL ALmixer_OutputOpenALInfo(void);

/** This function may be removed in the future. Returns true if compiled with threads, false if not. */ 
/*
extern ALMIXER_DECLSPEC size_t ALMIXER_CALL ALmixer_GetCurrentThreadID(void);
extern ALMIXER_DECLSPEC ALboolean ALMIXER_CALL ALmixer_CompiledWithThreadBackend(void);
extern ALMIXER_DECLSPEC size_t ALMIXER_CALL ALmixer_GetThreadIDForType(int almixer_thread_type);
*/

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif



/**********
* OpenAL
***********/
#ifdef ALMIXER_SWIG_INCLUDE_OPENAL_BINDINGS
%include "al.i"
#endif

