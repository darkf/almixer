/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2014 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/**
 *  \file ALmixer_rwops.h
 *
 *  This file provides a general interface for SDL to read and write
 *  data streams.  It can easily be extended to files, memory, etc.
 */

#ifndef _ALmixer_rwops_h
#define _ALmixer_rwops_h

#include "ALmixer.h"
/*
#include "ALmixer_stdinc.h"
*/

/*
	I'm probably asking for trouble, but I don't want to have to ship all the SDL_config headers.
	So I'm going to inline just the parts I need. 
	I need the stdint types, and
	I need the compiler define for ALMIXER_HAVE_STDIO_H.
	As far as I can tell, all the shipping SDL2 systems have stdio.
	I also want the pragma structure packing. My hope is my copied implementation is compatible so you can pointer cast between SDL/ALmixer.
 */
#ifndef ALMIXER_SHOULD_DISABLE_STDIO
#define ALMIXER_HAVE_STDIO_H 1
#include <stdio.h>
/*
#include <stddef.h>
*/
#endif /* ifndef ALMIXER_SHOULD_DISABLE_STDIO */

/* Most everything except Visual Studio 2008 and earlier has stdint.h now */
#if defined(_MSC_VER) && (_MSC_VER < 1600)
#ifndef ALMIXER_SHOULD_DISABLE_STDINT_REPLACEMENT
/* Here are some reasonable defaults */
typedef unsigned int size_t;
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed int int32_t;
typedef unsigned int uint32_t;
typedef signed long long int64_t;
typedef unsigned long long uint64_t;
typedef unsigned long uintptr_t;
#define ALMIXER_NOW_USING_STDINT_REPLACEMENT 1
#endif /* #ifndef ALMIXER_SHOULD_DISABLE_STDINT_REPLACEMENT */
#else
#include <stdint.h>
#endif /* Visual Studio 2008 */
typedef enum
{
    ALMIXER_FALSE = 0,
    ALMIXER_TRUE = 1
} ALmixer_bool;
/*
#include "ALmixer_error.h"
#include "begin_code.h"
*/
/* Force structure packing at 4 byte alignment.
 This is necessary if the header is included in code which has structure
 packing set to an alternate value, say for loading structures from disk.
 The packing is reset to the previous value in close_code.h
 */
#if defined(_MSC_VER) || defined(__MWERKS__) || defined(__BORLANDC__)
#ifdef _MSC_VER
#pragma warning(disable: 4103)
#endif
#ifdef __BORLANDC__
#pragma nopackwarning
#endif
#ifdef _M_X64
/* Use 8-byte alignment on 64-bit architectures, so pointers are aligned */
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif
#endif /* Compiler needs structure packing set */


/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

/* RWops Types */
#define ALMIXER_RWOPS_UNKNOWN   0   /* Unknown stream type */
#define ALMIXER_RWOPS_WINFILE   1   /* Win32 file */
#define ALMIXER_RWOPS_STDFILE   2   /* Stdio file */
#define ALMIXER_RWOPS_JNIFILE   3   /* Android asset */
#define ALMIXER_RWOPS_MEMORY    4   /* Memory stream */
#define ALMIXER_RWOPS_MEMORY_RO 5   /* Read-Only memory stream */

/**
 * This is the read/write operation structure -- very basic.
 */
struct ALmixer_RWops
{
/* SWIG JavaScript is having problems with anonymous unions. Disable for now. */
#if 1

    /**
     *  Return the size of the file in this rwops, or -1 if unknown
     */
    int64_t (ALMIXER_CALL * size) (struct ALmixer_RWops * context);

    /**
     *  Seek to \c offset relative to \c whence, one of stdio's whence values:
     *  RW_SEEK_SET, RW_SEEK_CUR, RW_SEEK_END
     *
     *  \return the final offset in the data stream, or -1 on error.
     */
    int64_t (ALMIXER_CALL * seek) (struct ALmixer_RWops * context, int64_t offset,
                             int whence);

    /**
     *  Read up to \c maxnum objects each of size \c size from the data
     *  stream to the area pointed at by \c ptr.
     *
     *  \return the number of objects read, or 0 at error or end of file.
     */
    size_t (ALMIXER_CALL * read) (struct ALmixer_RWops * context, void *ptr,
                             size_t size, size_t maxnum);

    /**
     *  Write exactly \c num objects each of size \c size from the area
     *  pointed at by \c ptr to data stream.
     *
     *  \return the number of objects written, or 0 at error or end of file.
     */
    size_t (ALMIXER_CALL * write) (struct ALmixer_RWops * context, const void *ptr,
                              size_t size, size_t num);

    /**
     *  Close and free an allocated ALmixer_RWops structure.
     *
     *  \return 0 if successful or -1 on write error when flushing data.
     */
    int (ALMIXER_CALL * close) (struct ALmixer_RWops * context);

    uint32_t type;
    union
    {
//#if defined(__ANDROID__)
        struct
        {
            void *fileNameRef;
            void *inputStreamRef;
            void *readableByteChannelRef;
            void *readMethod;
            void *assetFileDescriptorRef;
            long position;
            long size;
            long offset;
            int fd;
        } androidio;
//#elif defined(__WIN32__)
        struct
        {
            ALmixer_bool append;
            void *h;
            struct
            {
                void *data;
                size_t size;
                size_t left;
            } buffer;
        } windowsio;
//#endif

//#ifdef ALMIXER_HAVE_STDIO_H
        struct
        {
            ALmixer_bool autoclose;
            FILE *fp;
        } stdio;
//#endif
        struct
        {
            uint8_t *base;
            uint8_t *here;
            uint8_t *stop;
        } mem;
        struct
        {
            void *data1;
            void *data2;
        } unknown;
    } hidden;
#endif
};

typedef ALmixer_RWops ALmixer_RWops;
/* FIXME: The autoclose makes the automatic memory management tricky/dangerous.
*/
/*
%extend ALmixer_RWops
{
	~ALmixer_RWops()
	{
		ALmixer_RWclose($self);
	}
};
*/



/**
 *  \name RWFrom functions
 *
 *  Functions to create ALmixer_RWops structures from various data streams.
 */
/* @{ */
// %newobject ALmixer_RWFromFile; // not under memory management because autoclose confuses things
extern ALMIXER_DECLSPEC ALmixer_RWops *ALMIXER_CALL ALmixer_RWFromFile(const char *file,
                                                  const char *mode);

// Assumption: All the platforms we are targeting support stdio.
//#ifdef ALMIXER_HAVE_STDIO_H
// %newobject ALmixer_RWFromFP; // not under memory management because autoclose confuses things
extern ALMIXER_DECLSPEC ALmixer_RWops *ALMIXER_CALL ALmixer_RWFromFP(FILE * fp,
                                                ALmixer_bool autoclose);
//#else
/*
extern ALMIXER_DECLSPEC ALmixer_RWops *ALMIXER_CALL ALmixer_RWFromFP(void * fp,
                                                ALmixer_bool autoclose);
*/
//#endif

// %newobject ALmixer_RWFromMem; // not under memory management because autoclose confuses things
extern ALMIXER_DECLSPEC ALmixer_RWops *ALMIXER_CALL ALmixer_RWFromMem(void *mem, int size);
// %newobject ALmixer_RWFromConstMem; // not under memory management because autoclose confuses things
extern ALMIXER_DECLSPEC ALmixer_RWops *ALMIXER_CALL ALmixer_RWFromConstMem(const void *mem,
                                                      int size);

/* @} *//* RWFrom functions */


// %newobject ALmixer_AllocRW; // not under memory management because autoclose confuses things
extern ALMIXER_DECLSPEC ALmixer_RWops *ALMIXER_CALL ALmixer_AllocRW(void);
// %delobject ALmixer_FreeRW; // not under memory management because autoclose confuses things
extern ALMIXER_DECLSPEC void ALMIXER_CALL ALmixer_FreeRW(ALmixer_RWops * area);

#define RW_SEEK_SET 0       /**< Seek from the beginning of data */
#define RW_SEEK_CUR 1       /**< Seek relative to current read point */
#define RW_SEEK_END 2       /**< Seek relative to the end of data */

/**
 *  \name Read/write macros
 *
 *  Macros to easily read and write from an ALmixer_RWops structure.
 */
/* @{ */
/*
#define ALmixer_RWsize(ctx)         (ctx)->size(ctx)
#define ALmixer_RWseek(ctx, offset, whence) (ctx)->seek(ctx, offset, whence)
#define ALmixer_RWtell(ctx)         (ctx)->seek(ctx, 0, RW_SEEK_CUR)
#define ALmixer_RWread(ctx, ptr, size, n)   (ctx)->read(ctx, ptr, size, n)
#define ALmixer_RWwrite(ctx, ptr, size, n)  (ctx)->write(ctx, ptr, size, n)
#define ALmixer_RWclose(ctx)        (ctx)->close(ctx)
*/
int64_t ALmixer_RWsize(ALmixer_RWops* ctx);
int64_t ALmixer_RWseek(ALmixer_RWops* ctx, int64_t offset, int whence);
int64_t ALmixer_RWtell(ALmixer_RWops* ctx);
// SWIG: Should pretend string buffer.
//size_t ALmixer_RWread(ALmixer_RWops* ctx, void *ptr, size_t size, size_t maxnum);
%include "typemaps.i"
#ifdef SWIG_JAVASCRIPT_JSC
/* The check typemap doesn't work, so I'm implementing the whole thing. */
%native(RWread) JSValueRef _wrap_RWread(JSContextRef context, JSObjectRef function, JSObjectRef thisObject, size_t argc, const JSValueRef argv[], JSValueRef* exception);
%{
static JSValueRef _wrap_RWread(JSContextRef context, JSObjectRef function, JSObjectRef thisObject, size_t argc, const JSValueRef argv[], JSValueRef* exception)
{
  ALmixer_RWops *arg1 = (ALmixer_RWops *) 0 ;
  char *arg2 = (char *) 0 ;
  size_t arg3 ;
  size_t arg4 ;
  void *argp1 = 0 ;
  int res1 = 0 ;
  size_t val3 ;
  int ecode3 = 0 ;
  size_t val4 ;
  int ecode4 = 0 ;
  size_t result;
  
  JSValueRef jsresult;
  
  if(argc != 3) SWIG_exception_fail(SWIG_ERROR, "Illegal number of arguments for _wrap_RWread.");
  
  res1 = SWIG_ConvertPtr(argv[0], &argp1,SWIGTYPE_p_ALmixer_RWops, 0 |  0 );
  if (!SWIG_IsOK(res1)) {
    SWIG_exception_fail(SWIG_ArgError(res1), "in method '" "RWread" "', argument " "1"" of type '" "ALmixer_RWops *""'"); 
  }
  arg1 = (ALmixer_RWops *)(argp1);
  {
    // typemap to disable read_buffer as an input parameter
  }
  ecode3 = SWIG_AsVal_size_t SWIG_JSC_AS_CALL_ARGS(argv[2], &val3);
  if (!SWIG_IsOK(ecode3)) {
    SWIG_exception_fail(SWIG_ArgError(ecode3), "in method '" "RWread" "', argument " "3"" of type '" "size_t""'");
  } 
  arg3 = (size_t)(val3);
  ecode4 = SWIG_AsVal_size_t SWIG_JSC_AS_CALL_ARGS(argv[3], &val4);
  if (!SWIG_IsOK(ecode4)) {
    SWIG_exception_fail(SWIG_ArgError(ecode4), "in method '" "RWread" "', argument " "4"" of type '" "size_t""'");
  } 
  arg4 = (size_t)(val4);

  {
    arg2 = (char*)SDL_malloc(arg3*arg4);
  }

  result = ALmixer_RWread(arg1,arg2,arg3,arg4);
  jsresult = SWIG_From_size_t  SWIG_JSC_FROM_CALL_ARGS((size_t)(result));

  {
    JSValueRef return_string = ((void *)0);
    char* buffer = arg2;
    JSStringRef jsstring_data_buffer;
    /* size is always 1 as far as I know. But to be defensive in case nmemb 
        and size get swapped or they are not what I expect, I will multiply.
    */
    size_t num_size = arg3*arg4; // size is 1 as far as I know.

    JSChar* js_char_buffer = SDL_malloc(num_size*sizeof(JSChar));
    size_t i;
    
    for(i=0; i<num_size; i++)
    {
      if(buffer[i] & 0x80)
      {
        js_char_buffer[i] = 0xF700 | buffer[i];
      }
      else
      {
        js_char_buffer[i] = buffer[i];
      }
    }
    
    jsstring_data_buffer = JSStringCreateWithCharacters(&js_char_buffer[0], num_size);
    free(js_char_buffer);
    
    return_string = JSValueMakeString(context, jsstring_data_buffer);
    JSStringRelease(jsstring_data_buffer);
    
    {
        JSValueRef return_array_values[2];
        JSObjectRef return_js_array;
        return_array_values[0] = jsresult;
        return_array_values[1] = return_string;
        return_js_array = JSObjectMakeArray(context, 2, return_array_values, NULL);

        jsresult = return_js_array;
    }
  }

  {
    SDL_free(arg2);
  }
  
  return jsresult;
  
  goto fail;
fail:
  return JSValueMakeUndefined(context);
}
%}

#elif defined(SWIGLUA)

/* This disables read_buffer as an input parameter. */
%typemap(in, numinputs=0) char* read_buffer
{
    // typemap to disable read_buffer as an input parameter
}
/* We need to malloc after the last two parameters are read in. So we use "check". */
/* $1 is arg2 */
%typemap(check) char* read_buffer
{
   $1 = (char*)SDL_malloc(arg3*arg4);
}
/* This makes read_buffer a return value */
/* $1 is arg2, but I don't know how to use tokens for arg3, arg4 */
%typemap(argout) char* read_buffer
{
  lua_pushlstring(L, $1, arg3*arg4);
  SWIG_arg++;
}
/* free the temporary buffer */
%typemap(freearg) char* read_buffer 
{
   SDL_free($1);
}
/*
local num_bytes, buffer_string = SDL.RWread(rw_ops, 1, 20);
*/
size_t ALmixer_RWread(ALmixer_RWops* ctx, char* read_buffer, size_t size, size_t maxnum);

#else
#warning "Need to map read buffer output for ALmixer_RWread"
// SWIG doesn't apply OUTPUT to char*. Have to do it manually.
size_t ALmixer_RWread(ALmixer_RWops* ctx, void *OUTPUT, size_t size, size_t maxnum);
#endif
// SWIG: Should pretend string buffer.
//size_t ALmixer_RWwrite(ALmixer_RWops* ctx, const void *ptr, size_t size, size_t num);
#ifdef SWIG_JAVASCRIPT_JSC
/* This was originally generated from SWIG, and then I injected the code for the s_listOfCallbackUserDatas */
%native(RWwrite) JSValueRef _wrap_RWwrite(JSContextRef context, JSObjectRef function, JSObjectRef thisObject, size_t argc, const JSValueRef argv[], JSValueRef* exception);
%{
static JSValueRef _wrap_RWwrite(JSContextRef context, JSObjectRef function, JSObjectRef thisObject, size_t argc, const JSValueRef argv[], JSValueRef* exception)
{
  ALmixer_RWops *arg1 = (ALmixer_RWops *) 0 ;
  char *arg2 = (char *) 0 ;
  size_t arg3 ;
  size_t arg4 ;
  void *argp1 = 0 ;
  int res1 = 0 ;
  int res2 ;
  size_t val3 ;
  size_t val4 ;
  int ecode3 = 0 ;
  int ecode4 = 0 ;
  size_t result;
  
  JSValueRef jsresult;
  
  if(argc != 4) SWIG_exception_fail(SWIG_ERROR, "Illegal number of arguments for _wrap_RWwrite.");
  
  res1 = SWIG_ConvertPtr(argv[0], &argp1,SWIGTYPE_p_ALmixer_RWops, 0 |  0 );
  if (!SWIG_IsOK(res1)) {
    SWIG_exception_fail(SWIG_ArgError(res1), "in method '" "RWwrite" "', argument " "1"" of type '" "ALmixer_RWops *""'"); 
  }
  arg1 = (ALmixer_RWops *)(argp1);
/*
  res2 = SWIG_AsCharPtrAndSize(argv[1], &buf2, &size2, &alloc2);
  if (!SWIG_IsOK(res2)) {
    SWIG_exception_fail(SWIG_ArgError(res2), "in method '" "RWwrite" "', argument " "2"" of type '" "char const *""'");
  }
*/
  if(!JSValueIsString(context, argv[1]))
  {
    SWIG_exception_fail(SWIG_ArgError(res2), "in method '" "RWwrite" "', argument " "2"" of type '" "string""'");
  }
  ecode3 = SWIG_AsVal_size_t SWIG_JSC_AS_CALL_ARGS(argv[2], &val3);
  if (!SWIG_IsOK(ecode4)) {
    SWIG_exception_fail(SWIG_ArgError(ecode4), "in method '" "RWwrite" "', argument " "4"" of type '" "size_t""'");
  } 
  arg3 = (size_t)(val3);

  ecode4 = SWIG_AsVal_size_t SWIG_JSC_AS_CALL_ARGS(argv[3], &val4);
  if (!SWIG_IsOK(ecode4)) {
    SWIG_exception_fail(SWIG_ArgError(ecode4), "in method '" "RWwrite" "', argument " "4"" of type '" "size_t""'");
  } 
  arg4 = (size_t)(val4);



    {
        JSStringRef js_str = JSValueToStringCopy(context, argv[1], NULL);
//        size_t num_chars = JSStringGetLength(js_str);
        size_t num_chars = arg3 * arg4;
        const JSChar* jschar_buffer = JSStringGetCharactersPtr(js_str);
        size_t i;
//        char* char_buffer = (char*)malloc(unicode_chars * sizeof(char)); // Not null terminated
        char* char_buffer = (char*)malloc(arg3 * arg4); // Not null terminated
        for(i=0; i<num_chars; i++)
        {
            if(jschar_buffer[i] & 0xF700)
            {
                char_buffer[i] = 0x0080 | jschar_buffer[i];
            }
            else
            {
                char_buffer[i] = 0x00FF & jschar_buffer[i];
            }
        }

        arg2 = char_buffer;

        result = ALmixer_RWwrite(arg1,(char const *)arg2,arg3,arg4);

        free(char_buffer);
        JSStringRelease(js_str);

    }


  jsresult = SWIG_From_size_t  SWIG_JSC_FROM_CALL_ARGS((size_t)(result));
  
//  if (alloc2 == SWIG_NEWOBJ) free((char*)buf2);
  
  return jsresult;
  
  goto fail;
fail:
  return JSValueMakeUndefined(context);
}
%}

#else
size_t ALmixer_RWwrite(ALmixer_RWops* ctx, const char *ptr, size_t size, size_t num);
#endif

int ALmixer_RWclose(ALmixer_RWops* ctx);

/* @} *//* Read/write macros */


/**
 *  \name Read endian functions
 *
 *  Read an item of the specified endianness and return in native format.
 */
/* @{ */
extern ALMIXER_DECLSPEC uint8_t ALMIXER_CALL ALmixer_ReadU8(ALmixer_RWops * src);
extern ALMIXER_DECLSPEC uint16_t ALMIXER_CALL ALmixer_ReadLE16(ALmixer_RWops * src);
extern ALMIXER_DECLSPEC uint16_t ALMIXER_CALL ALmixer_ReadBE16(ALmixer_RWops * src);
extern ALMIXER_DECLSPEC uint32_t ALMIXER_CALL ALmixer_ReadLE32(ALmixer_RWops * src);
extern ALMIXER_DECLSPEC uint32_t ALMIXER_CALL ALmixer_ReadBE32(ALmixer_RWops * src);
extern ALMIXER_DECLSPEC uint64_t ALMIXER_CALL ALmixer_ReadLE64(ALmixer_RWops * src);
extern ALMIXER_DECLSPEC uint64_t ALMIXER_CALL ALmixer_ReadBE64(ALmixer_RWops * src);
/* @} *//* Read endian functions */

/**
 *  \name Write endian functions
 *
 *  Write an item of native format to the specified endianness.
 */
/* @{ */
extern ALMIXER_DECLSPEC size_t ALMIXER_CALL ALmixer_WriteU8(ALmixer_RWops * dst, uint8_t value);
extern ALMIXER_DECLSPEC size_t ALMIXER_CALL ALmixer_WriteLE16(ALmixer_RWops * dst, uint16_t value);
extern ALMIXER_DECLSPEC size_t ALMIXER_CALL ALmixer_WriteBE16(ALmixer_RWops * dst, uint16_t value);
extern ALMIXER_DECLSPEC size_t ALMIXER_CALL ALmixer_WriteLE32(ALmixer_RWops * dst, uint32_t value);
extern ALMIXER_DECLSPEC size_t ALMIXER_CALL ALmixer_WriteBE32(ALmixer_RWops * dst, uint32_t value);
extern ALMIXER_DECLSPEC size_t ALMIXER_CALL ALmixer_WriteLE64(ALmixer_RWops * dst, uint64_t value);
extern ALMIXER_DECLSPEC size_t ALMIXER_CALL ALmixer_WriteBE64(ALmixer_RWops * dst, uint64_t value);
/* @} *//* Write endian functions */


/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif
/*
#include "close_code.h"
*/
#if defined(_MSC_VER) || defined(__MWERKS__) || defined(__WATCOMC__)  || defined(__BORLANDC__)
#ifdef __BORLANDC__
#pragma nopackwarning
#endif
#pragma pack(pop)
#endif /* Compiler needs structure packing set */
#endif /* _ALmixer_rwops_h */

/* vi: set ts=4 sw=4 expandtab: */

