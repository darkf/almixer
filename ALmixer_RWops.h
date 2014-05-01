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
#include "ALmixer_stdinc.h"


/*
	I'm probably asking for trouble, but I don't want to have to ship all the SDL_config headers.
	So I'm going to inline just the parts I need. 
	I need the stdint types, and
	I need the compiler define for HAVE_STDIO_H.
	As far as I can tell, all the shipping SDL2 systems have stdio.
	I also want the pragma structure packing. My hope is my copied implementation is compatible so you can pointer cast between SDL/ALmixer.
 */
#define HAVE_STDIO_H 1
#include <stdio.h>
//#include <stddef.h>

/* Most everything except Visual Studio 2008 and earlier has stdint.h now */
#if defined(_MSC_VER) && (_MSC_VER < 1600)
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
#else
#include <stdint.h>
#endif /* Visual Studio 2008 */
typedef enum
{
    ALMIXER_FALSE = 0,
    ALMIXER_TRUE = 1
} ALmixer_bool;

//#include "ALmixer_error.h"

//#include "begin_code.h"
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
typedef struct ALmixer_RWops
{
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
#if defined(__ANDROID__)
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
#elif defined(__WIN32__)
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
#endif

#ifdef HAVE_STDIO_H
        struct
        {
            ALmixer_bool autoclose;
            FILE *fp;
        } stdio;
#endif
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

} ALmixer_RWops;


/**
 *  \name RWFrom functions
 *
 *  Functions to create ALmixer_RWops structures from various data streams.
 */
/* @{ */

extern ALMIXER_DECLSPEC ALmixer_RWops *ALMIXER_CALL ALmixer_RWFromFile(const char *file,
                                                  const char *mode);

#ifdef HAVE_STDIO_H
extern ALMIXER_DECLSPEC ALmixer_RWops *ALMIXER_CALL ALmixer_RWFromFP(FILE * fp,
                                                ALmixer_bool autoclose);
#else
extern ALMIXER_DECLSPEC ALmixer_RWops *ALMIXER_CALL ALmixer_RWFromFP(void * fp,
                                                ALmixer_bool autoclose);
#endif

extern ALMIXER_DECLSPEC ALmixer_RWops *ALMIXER_CALL ALmixer_RWFromMem(void *mem, int size);
extern ALMIXER_DECLSPEC ALmixer_RWops *ALMIXER_CALL ALmixer_RWFromConstMem(const void *mem,
                                                      int size);

/* @} *//* RWFrom functions */


extern ALMIXER_DECLSPEC ALmixer_RWops *ALMIXER_CALL ALmixer_AllocRW(void);
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
#define ALmixer_RWsize(ctx)         (ctx)->size(ctx)
#define ALmixer_RWseek(ctx, offset, whence) (ctx)->seek(ctx, offset, whence)
#define ALmixer_RWtell(ctx)         (ctx)->seek(ctx, 0, RW_SEEK_CUR)
#define ALmixer_RWread(ctx, ptr, size, n)   (ctx)->read(ctx, ptr, size, n)
#define ALmixer_RWwrite(ctx, ptr, size, n)  (ctx)->write(ctx, ptr, size, n)
#define ALmixer_RWclose(ctx)        (ctx)->close(ctx)
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
//#include "close_code.h"
#if defined(_MSC_VER) || defined(__MWERKS__) || defined(__WATCOMC__)  || defined(__BORLANDC__)
#ifdef __BORLANDC__
#pragma nopackwarning
#endif
#pragma pack(pop)
#endif /* Compiler needs structure packing set */
#endif /* _ALmixer_rwops_h */

/* vi: set ts=4 sw=4 expandtab: */
