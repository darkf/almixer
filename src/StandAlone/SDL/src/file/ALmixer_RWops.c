

#ifdef ALMIXER_COMPILE_WITH_SDL

/* In this case, we just past through to the real SDL implementation.
 The ALmixer_RWops interface in this case is actually just a facade and everything is actually an SDL_RWops* that has a pointer casted.
 The ALmixer_RWops struct was copied directly from SDL and tries very hard to have the identical size and alignment packing so the two are interchangeable.
 */

#include "SDL.h"
#include "SDL_stdinc.h"
#include "SDL_rwops.h"
#include "ALmixer_RWops.h"

ALmixer_RWops *
ALmixer_RWFromFile(const char *file, const char *mode)
{
	return SDL_reinterpret_cast(ALmixer_RWops*, SDL_RWFromFile(file, mode));
}

#ifdef ALMIXER_HAVE_STDIO_H
ALmixer_RWops *
ALmixer_RWFromFP(FILE * fp, ALmixer_bool autoclose)
{
 	return SDL_reinterpret_cast(ALmixer_RWops*, SDL_RWFromFP(fp, autoclose));
}
#else
ALmixer_RWops *
ALmixer_RWFromFP(void * fp, ALmixer_bool autoclose)
{
 	return SDL_reinterpret_cast(ALmixer_RWops*, SDL_RWFromFP(fp, autoclose));
}
#endif /* ALMIXER_HAVE_STDIO_H */

ALmixer_RWops *
ALmixer_RWFromMem(void *mem, int size)
{
 	return SDL_reinterpret_cast(ALmixer_RWops*, SDL_RWFromMem(mem, size));
}

ALmixer_RWops *
ALmixer_RWFromConstMem(const void *mem, int size)
{
  	return SDL_reinterpret_cast(ALmixer_RWops*, SDL_RWFromConstMem(mem, size));
}

ALmixer_RWops *
ALmixer_AllocRW(void)
{
  	return SDL_reinterpret_cast(ALmixer_RWops*, SDL_AllocRW());
}

void
ALmixer_FreeRW(ALmixer_RWops * area)
{
	SDL_FreeRW(SDL_reinterpret_cast(SDL_RWops*, area));
}

/* Functions for dynamically reading and writing endian-specific values */

uint8_t
ALmixer_ReadU8(ALmixer_RWops * src)
{
    return ALmixer_ReadU8(SDL_reinterpret_cast(SDL_RWops*, src));
}

uint16_t
ALmixer_ReadLE16(ALmixer_RWops * src)
{
    return ALmixer_ReadLE16(SDL_reinterpret_cast(SDL_RWops*, src));
}

uint16_t
ALmixer_ReadBE16(ALmixer_RWops * src)
{
    return ALmixer_ReadBE16(SDL_reinterpret_cast(SDL_RWops*, src));
}

uint32_t
ALmixer_ReadLE32(ALmixer_RWops * src)
{
    return ALmixer_ReadLE32(SDL_reinterpret_cast(SDL_RWops*, src));
}

uint32_t
ALmixer_ReadBE32(ALmixer_RWops * src)
{
    return ALmixer_ReadBE32(SDL_reinterpret_cast(SDL_RWops*, src));
}

uint64_t
ALmixer_ReadLE64(ALmixer_RWops * src)
{
    return ALmixer_ReadLE64(SDL_reinterpret_cast(SDL_RWops*, src));
}

uint64_t
ALmixer_ReadBE64(ALmixer_RWops * src)
{
    return ALmixer_ReadBE64(SDL_reinterpret_cast(SDL_RWops*, src));
}

size_t
ALmixer_WriteU8(ALmixer_RWops * dst, uint8_t value)
{
    return ALmixer_WriteU8(SDL_reinterpret_cast(SDL_RWops*, dst), value);
}

size_t
ALmixer_WriteLE16(ALmixer_RWops * dst, uint16_t value)
{
    return ALmixer_WriteLE16(SDL_reinterpret_cast(SDL_RWops*, dst), value);
}

size_t
ALmixer_WriteBE16(ALmixer_RWops * dst, uint16_t value)
{
    return ALmixer_WriteBE16(SDL_reinterpret_cast(SDL_RWops*, dst), value);
}

size_t
ALmixer_WriteLE32(ALmixer_RWops * dst, uint32_t value)
{
    return ALmixer_WriteLE32(SDL_reinterpret_cast(SDL_RWops*, dst), value);
}

size_t
ALmixer_WriteBE32(ALmixer_RWops * dst, uint32_t value)
{
    return ALmixer_WriteBE32(SDL_reinterpret_cast(SDL_RWops*, dst), value);
}

size_t
ALmixer_WriteLE64(ALmixer_RWops * dst, uint64_t value)
{
    return ALmixer_WriteLE64(SDL_reinterpret_cast(SDL_RWops*, dst), value);
}

size_t
ALmixer_WriteBE64(ALmixer_RWops * dst, uint64_t value)
{
    return ALmixer_WriteBE64(SDL_reinterpret_cast(SDL_RWops*, dst), value);
}

#else /* #if ALMIXER_COMPILE_WITH_SDL */

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


/* Need this so Linux systems define fseek64o, ftell64o and off64_t */
#define _LARGEFILE64_SOURCE
#include "../ALmixer_internal.h"

#if 1
#if defined(__WIN32__)
#include "../core/windows/ALmixer_windows.h"
#endif
#else
#define WIN32_LEAN_AND_MEAN
#define STRICT
#ifndef UNICODE
#define UNICODE
#endif
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x501 /* Need 0x410 for AlphaBlend() and 0x500 for EnumDisplayDevices(), 0x501 for raw input */
#include <windows.h>
#endif /* #if 0 */

/* This file provides a general interface for SDL to read and write
   data sources.  It can easily be extended to files, memory, etc.
*/

#include "../../include/ALmixer_endian.h"


//#include "ALmixer.h"
#include "ALmixer_RWops.h"

typedef enum
{
    ALMIXER_ENOMEM,
    ALMIXER_EFREAD,
    ALMIXER_EFWRITE,
    ALMIXER_EFSEEK,
    ALMIXER_UNSUPPORTED,
    ALMIXER_LASTERROR
} ALmixer_errorcode;
/* Very common errors go here */
static int
ALmixer_Error(ALmixer_errorcode code)
{
    switch (code) {
		case ALMIXER_ENOMEM:
			ALmixer_SetError("Out of memory");
			return -1;
		case ALMIXER_EFREAD:
			ALmixer_SetError("Error reading from datastream");
			return -1;
		case ALMIXER_EFWRITE:
			ALmixer_SetError("Error writing to datastream");
			return -1;
		case ALMIXER_EFSEEK:
			ALmixer_SetError("Error seeking in datastream");
			return -1;
		case ALMIXER_UNSUPPORTED:
			ALmixer_SetError("That operation is not supported");
			return -1;
		default:
			ALmixer_SetError("Unknown ALmixer_RWops error");
			return -1;
    }
}

#define ALmixer_OutOfMemory()   ALmixer_Error(ALMIXER_ENOMEM)
#define ALmixer_Unsupported()   ALmixer_Error(ALMIXER_UNSUPPORTED)
#define ALmixer_InvalidParamError(param)    ALmixer_SetError("Parameter '%s' is invalid", (param))


#ifdef __APPLE__
#include "cocoa/ALmixer_rwopsbundlesupport.h"
#endif /* __APPLE__ */

#ifdef __ANDROID__
#include "../core/android/ALmixer_android.h"
#include "ALmixer_system.h"
#endif

#ifdef __WIN32__

/* Functions to read/write Win32 API file pointers */

#ifndef INVALID_SET_FILE_POINTER
#define INVALID_SET_FILE_POINTER 0xFFFFFFFF
#endif

#define READAHEAD_BUFFER_SIZE   1024

static int ALMIXER_CALL
windows_file_open(ALmixer_RWops * context, const char *filename, const char *mode)
{
    UINT old_error_mode;
    HANDLE h;
    DWORD r_right, w_right;
    DWORD must_exist, truncate;
    int a_mode;

    if (!context)
        return -1;              /* failed (invalid call) */

    context->hidden.windowsio.h = INVALID_HANDLE_VALUE;   /* mark this as unusable */
    context->hidden.windowsio.buffer.data = NULL;
    context->hidden.windowsio.buffer.size = 0;
    context->hidden.windowsio.buffer.left = 0;

    /* "r" = reading, file must exist */
    /* "w" = writing, truncate existing, file may not exist */
    /* "r+"= reading or writing, file must exist            */
    /* "a" = writing, append file may not exist             */
    /* "a+"= append + read, file may not exist              */
    /* "w+" = read, write, truncate. file may not exist    */

    must_exist = (ALmixer_strchr(mode, 'r') != NULL) ? OPEN_EXISTING : 0;
    truncate = (ALmixer_strchr(mode, 'w') != NULL) ? CREATE_ALWAYS : 0;
    r_right = (ALmixer_strchr(mode, '+') != NULL
               || must_exist) ? GENERIC_READ : 0;
    a_mode = (ALmixer_strchr(mode, 'a') != NULL) ? OPEN_ALWAYS : 0;
    w_right = (a_mode || ALmixer_strchr(mode, '+')
               || truncate) ? GENERIC_WRITE : 0;

    if (!r_right && !w_right)   /* inconsistent mode */
        return -1;              /* failed (invalid call) */

    context->hidden.windowsio.buffer.data =
        (char *) ALmixer_malloc(READAHEAD_BUFFER_SIZE);
    if (!context->hidden.windowsio.buffer.data) {
        return ALmixer_OutOfMemory();
    }
    /* Do not open a dialog box if failure */
    old_error_mode =
        SetErrorMode(SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);

    {
        LPTSTR tstr = WIN_UTF8ToString(filename);
        h = CreateFile(tstr, (w_right | r_right),
                       (w_right) ? 0 : FILE_SHARE_READ, NULL,
                       (must_exist | truncate | a_mode),
                       FILE_ATTRIBUTE_NORMAL, NULL);
        ALmixer_free(tstr);
    }

    /* restore old behavior */
    SetErrorMode(old_error_mode);

    if (h == INVALID_HANDLE_VALUE) {
        ALmixer_free(context->hidden.windowsio.buffer.data);
        context->hidden.windowsio.buffer.data = NULL;
        ALmixer_SetError("Couldn't open %s", filename);
        return -2;              /* failed (CreateFile) */
    }
    context->hidden.windowsio.h = h;
    context->hidden.windowsio.append = a_mode ? ALMIXER_TRUE : ALMIXER_FALSE;

    return 0;                   /* ok */
}

static int64_t ALMIXER_CALL
windows_file_size(ALmixer_RWops * context)
{
    LARGE_INTEGER size;

    if (!context || context->hidden.windowsio.h == INVALID_HANDLE_VALUE) {
        ALmixer_SetError("windows_file_size: invalid context/file not opened");
		return -1;
	}

    if (!GetFileSizeEx(context->hidden.windowsio.h, &size)) {
#if 1
        return WIN_SetError("windows_file_size");
#else
		/* There are a lot of dependencies on iconv/UTF16 stuff just for WIN_SetError to print the HRESULT. I'd rather avoid it */
		ALmixer_SetError("windows_file_size failed calling GetFileSizeEx()");
		return -1;
#endif
    }

    return size.QuadPart;
}

static int64_t ALMIXER_CALL
windows_file_seek(ALmixer_RWops * context, int64_t offset, int whence)
{
    DWORD windowswhence;
    LARGE_INTEGER windowsoffset;

    if (!context || context->hidden.windowsio.h == INVALID_HANDLE_VALUE) {
        ALmixer_SetError("windows_file_seek: invalid context/file not opened");
		return -1;
	}

    /* FIXME: We may be able to satisfy the seek within buffered data */
    if (whence == RW_SEEK_CUR && context->hidden.windowsio.buffer.left) {
        offset -= (long)context->hidden.windowsio.buffer.left;
    }
    context->hidden.windowsio.buffer.left = 0;

    switch (whence) {
    case RW_SEEK_SET:
        windowswhence = FILE_BEGIN;
        break;
    case RW_SEEK_CUR:
        windowswhence = FILE_CURRENT;
        break;
    case RW_SEEK_END:
        windowswhence = FILE_END;
        break;
    default:
        ALmixer_SetError("windows_file_seek: Unknown value for 'whence'");
		return -1;
	}

    windowsoffset.QuadPart = offset;
    if (!SetFilePointerEx(context->hidden.windowsio.h, windowsoffset, &windowsoffset, windowswhence)) {
#if 1
        return WIN_SetError("windows_file_seek");
#else
		/* There are a lot of dependencies on iconv/UTF16 stuff just for WIN_SetError to print the HRESULT. I'd rather avoid it */
		ALmixer_SetError("windows_file_seek failed calling SetFilePointerEx()");
		return -1;
#endif
    }
    return windowsoffset.QuadPart;
}

static size_t ALMIXER_CALL
windows_file_read(ALmixer_RWops * context, void *ptr, size_t size, size_t maxnum)
{
    size_t total_need;
    size_t total_read = 0;
    size_t read_ahead;
    DWORD byte_read;

    total_need = size * maxnum;

    if (!context || context->hidden.windowsio.h == INVALID_HANDLE_VALUE
        || !total_need)
        return 0;

    if (context->hidden.windowsio.buffer.left > 0) {
        void *data = (char *) context->hidden.windowsio.buffer.data +
            context->hidden.windowsio.buffer.size -
            context->hidden.windowsio.buffer.left;
        read_ahead =
            ALmixer_min(total_need, context->hidden.windowsio.buffer.left);
        ALmixer_memcpy(ptr, data, read_ahead);
        context->hidden.windowsio.buffer.left -= read_ahead;

        if (read_ahead == total_need) {
            return maxnum;
        }
        ptr = (char *) ptr + read_ahead;
        total_need -= read_ahead;
        total_read += read_ahead;
    }

    if (total_need < READAHEAD_BUFFER_SIZE) {
        if (!ReadFile
            (context->hidden.windowsio.h, context->hidden.windowsio.buffer.data,
             READAHEAD_BUFFER_SIZE, &byte_read, NULL)) {
            ALmixer_Error(ALMIXER_EFREAD);
            return 0;
        }
        read_ahead = ALmixer_min(total_need, (int) byte_read);
        ALmixer_memcpy(ptr, context->hidden.windowsio.buffer.data, read_ahead);
        context->hidden.windowsio.buffer.size = byte_read;
        context->hidden.windowsio.buffer.left = byte_read - read_ahead;
        total_read += read_ahead;
    } else {
        if (!ReadFile
            (context->hidden.windowsio.h, ptr, (DWORD)total_need, &byte_read, NULL)) {
            ALmixer_Error(ALMIXER_EFREAD);
            return 0;
        }
        total_read += byte_read;
    }
    return (total_read / size);
}

static size_t ALMIXER_CALL
windows_file_write(ALmixer_RWops * context, const void *ptr, size_t size,
                 size_t num)
{

    size_t total_bytes;
    DWORD byte_written;
    size_t nwritten;

    total_bytes = size * num;

    if (!context || context->hidden.windowsio.h == INVALID_HANDLE_VALUE
        || total_bytes <= 0 || !size)
        return 0;

    if (context->hidden.windowsio.buffer.left) {
        SetFilePointer(context->hidden.windowsio.h,
                       -(LONG)context->hidden.windowsio.buffer.left, NULL,
                       FILE_CURRENT);
        context->hidden.windowsio.buffer.left = 0;
    }

    /* if in append mode, we must go to the EOF before write */
    if (context->hidden.windowsio.append) {
        if (SetFilePointer(context->hidden.windowsio.h, 0L, NULL, FILE_END) ==
            INVALID_SET_FILE_POINTER) {
            ALmixer_Error(ALMIXER_EFWRITE);
            return 0;
        }
    }

    if (!WriteFile
        (context->hidden.windowsio.h, ptr, (DWORD)total_bytes, &byte_written, NULL)) {
		ALmixer_Error(ALMIXER_EFWRITE);
        return 0;
    }

    nwritten = byte_written / size;
    return nwritten;
}

static int ALMIXER_CALL
windows_file_close(ALmixer_RWops * context)
{

    if (context) {
        if (context->hidden.windowsio.h != INVALID_HANDLE_VALUE) {
            CloseHandle(context->hidden.windowsio.h);
            context->hidden.windowsio.h = INVALID_HANDLE_VALUE;   /* to be sure */
        }
        ALmixer_free(context->hidden.windowsio.buffer.data);
        context->hidden.windowsio.buffer.data = NULL;
        ALmixer_FreeRW(context);
    }
    return 0;
}
#endif /* __WIN32__ */

#ifdef ALMIXER_HAVE_STDIO_H

/* Functions to read/write stdio file pointers */

static int64_t ALMIXER_CALL
stdio_size(ALmixer_RWops * context)
{
    int64_t pos, size;

    pos = ALmixer_RWseek(context, 0, RW_SEEK_CUR);
    if (pos < 0) {
        return -1;
    }
    size = ALmixer_RWseek(context, 0, RW_SEEK_END);

    ALmixer_RWseek(context, pos, RW_SEEK_SET);
    return size;
}

static int64_t ALMIXER_CALL
stdio_seek(ALmixer_RWops * context, int64_t offset, int whence)
{
#ifdef HAVE_FSEEKO64
    if (fseeko64(context->hidden.stdio.fp, (off64_t)offset, whence) == 0) {
        return ftello64(context->hidden.stdio.fp);
    }
#elif defined(HAVE_FSEEKO)
    if (fseeko(context->hidden.stdio.fp, (off_t)offset, whence) == 0) {
        return ftello(context->hidden.stdio.fp);
    }
#elif defined(HAVE__FSEEKI64)
    if (_fseeki64(context->hidden.stdio.fp, offset, whence) == 0) {
        return _ftelli64(context->hidden.stdio.fp);
    }
#else
    if (fseek(context->hidden.stdio.fp, offset, whence) == 0) {
        return ftell(context->hidden.stdio.fp);
    }
#endif
    return ALmixer_Error(ALMIXER_EFSEEK);
}

static size_t ALMIXER_CALL
stdio_read(ALmixer_RWops * context, void *ptr, size_t size, size_t maxnum)
{
    size_t nread;

    nread = fread(ptr, size, maxnum, context->hidden.stdio.fp);
    if (nread == 0 && ferror(context->hidden.stdio.fp)) {
        ALmixer_Error(ALMIXER_EFREAD);
    }
    return nread;
}

static size_t ALMIXER_CALL
stdio_write(ALmixer_RWops * context, const void *ptr, size_t size, size_t num)
{
    size_t nwrote;

    nwrote = fwrite(ptr, size, num, context->hidden.stdio.fp);
    if (nwrote == 0 && ferror(context->hidden.stdio.fp)) {
        ALmixer_Error(ALMIXER_EFWRITE);
    }
    return nwrote;
}

static int ALMIXER_CALL
stdio_close(ALmixer_RWops * context)
{
    int status = 0;
    if (context) {
        if (context->hidden.stdio.autoclose) {
            /* WARNING:  Check the return value here! */
            if (fclose(context->hidden.stdio.fp) != 0) {
                status = ALmixer_Error(ALMIXER_EFWRITE);
            }
        }
        ALmixer_FreeRW(context);
    }
    return status;
}
#endif /* !ALMIXER_HAVE_STDIO_H */

/* Functions to read/write memory pointers */

static int64_t ALMIXER_CALL
mem_size(ALmixer_RWops * context)
{
    return (int64_t)(context->hidden.mem.stop - context->hidden.mem.base);
}

static int64_t ALMIXER_CALL
mem_seek(ALmixer_RWops * context, int64_t offset, int whence)
{
    uint8_t *newpos;

    switch (whence) {
    case RW_SEEK_SET:
        newpos = context->hidden.mem.base + offset;
        break;
    case RW_SEEK_CUR:
        newpos = context->hidden.mem.here + offset;
        break;
    case RW_SEEK_END:
        newpos = context->hidden.mem.stop + offset;
        break;
    default:
		ALmixer_SetError("Unknown value for 'whence'");
		return -1;
    }
    if (newpos < context->hidden.mem.base) {
        newpos = context->hidden.mem.base;
    }
    if (newpos > context->hidden.mem.stop) {
        newpos = context->hidden.mem.stop;
    }
    context->hidden.mem.here = newpos;
    return (int64_t)(context->hidden.mem.here - context->hidden.mem.base);
}

static size_t ALMIXER_CALL
mem_read(ALmixer_RWops * context, void *ptr, size_t size, size_t maxnum)
{
    size_t total_bytes;
    size_t mem_available;

    total_bytes = (maxnum * size);
    if ((maxnum <= 0) || (size <= 0)
        || ((total_bytes / maxnum) != (size_t) size)) {
        return 0;
    }

    mem_available = (context->hidden.mem.stop - context->hidden.mem.here);
    if (total_bytes > mem_available) {
        total_bytes = mem_available;
    }

    ALmixer_memcpy(ptr, context->hidden.mem.here, total_bytes);
    context->hidden.mem.here += total_bytes;

    return (total_bytes / size);
}

static size_t ALMIXER_CALL
mem_write(ALmixer_RWops * context, const void *ptr, size_t size, size_t num)
{
    if ((context->hidden.mem.here + (num * size)) > context->hidden.mem.stop) {
        num = (context->hidden.mem.stop - context->hidden.mem.here) / size;
    }
    ALmixer_memcpy(context->hidden.mem.here, ptr, num * size);
    context->hidden.mem.here += num * size;
    return num;
}

static size_t ALMIXER_CALL
mem_writeconst(ALmixer_RWops * context, const void *ptr, size_t size, size_t num)
{
    ALmixer_SetError("Can't write to read-only memory");
    return 0;
}

static int ALMIXER_CALL
mem_close(ALmixer_RWops * context)
{
    if (context) {
        ALmixer_FreeRW(context);
    }
    return 0;
}


/* Functions to create ALmixer_RWops structures from various data sources */

ALmixer_RWops *
ALmixer_RWFromFile(const char *file, const char *mode)
{
    ALmixer_RWops *rwops = NULL;
    if (!file || !*file || !mode || !*mode) {
        ALmixer_SetError("ALmixer_RWFromFile(): No file or no mode specified");
        return NULL;
    }
#if defined(__ANDROID__)
#ifdef ALMIXER_HAVE_STDIO_H
    /* Try to open the file on the filesystem first */
    if (*file == '/') {
        FILE *fp = fopen(file, mode);
        if (fp) {
            return ALmixer_RWFromFP(fp, 1);
        }
    } else {
        /* Try opening it from internal storage if it's a relative path */
        char *path;
        FILE *fp;

        path = ALmixer_stack_alloc(char, PATH_MAX);
        if (path) {
            ALmixer_snprintf(path, PATH_MAX, "%s/%s",
                         ALmixer_AndroidGetInternalStoragePath(), file);
            fp = fopen(path, mode);
            ALmixer_stack_free(path);
            if (fp) {
                return ALmixer_RWFromFP(fp, 1);
            }
        }
    }
#endif /* ALMIXER_HAVE_STDIO_H */

    /* Try to open the file from the asset system */
    rwops = ALmixer_AllocRW();
    if (!rwops)
        return NULL;            /* ALmixer_SetError already setup by ALmixer_AllocRW() */
    if (ALmixer_Android_JNI_FileOpen(rwops, file, mode) < 0) {
        ALmixer_FreeRW(rwops);
        return NULL;
    }
    rwops->size = ALmixer_Android_JNI_FileSize;
    rwops->seek = ALmixer_Android_JNI_FileSeek;
    rwops->read = ALmixer_Android_JNI_FileRead;
    rwops->write = ALmixer_Android_JNI_FileWrite;
    rwops->close = ALmixer_Android_JNI_FileClose;
    rwops->type = ALMIXER_RWOPS_JNIFILE;

#elif defined(__WIN32__)
    rwops = ALmixer_AllocRW();
    if (!rwops)
        return NULL;            /* ALmixer_SetError already setup by ALmixer_AllocRW() */
    if (windows_file_open(rwops, file, mode) < 0) {
        ALmixer_FreeRW(rwops);
        return NULL;
    }
    rwops->size = windows_file_size;
    rwops->seek = windows_file_seek;
    rwops->read = windows_file_read;
    rwops->write = windows_file_write;
    rwops->close = windows_file_close;
    rwops->type = ALMIXER_RWOPS_WINFILE;

#elif ALMIXER_HAVE_STDIO_H
    {
        #ifdef __APPLE__
        FILE *fp = ALmixer_OpenFPFromBundleOrFallback(file, mode);
        #elif __WINRT__
        FILE *fp = NULL;
        fopen_s(&fp, file, mode);
        #else
        FILE *fp = fopen(file, mode);
        #endif
        if (fp == NULL) {
            ALmixer_SetError("Couldn't open %s", file);
        } else {
            rwops = ALmixer_RWFromFP(fp, 1);
        }
    }
#else
    ALmixer_SetError("ALmixer_RWops not compiled with stdio support");
#endif /* !ALMIXER_HAVE_STDIO_H */

    return rwops;
}

#ifdef ALMIXER_HAVE_STDIO_H
ALmixer_RWops *
ALmixer_RWFromFP(FILE * fp, ALmixer_bool autoclose)
{
    ALmixer_RWops *rwops = NULL;

    rwops = ALmixer_AllocRW();
    if (rwops != NULL) {
        rwops->size = stdio_size;
        rwops->seek = stdio_seek;
        rwops->read = stdio_read;
        rwops->write = stdio_write;
        rwops->close = stdio_close;
        rwops->hidden.stdio.fp = fp;
        rwops->hidden.stdio.autoclose = autoclose;
        rwops->type = ALMIXER_RWOPS_STDFILE;
    }
    return rwops;
}
#else
ALmixer_RWops *
ALmixer_RWFromFP(void * fp, ALmixer_bool autoclose)
{
    ALmixer_SetError("ALmixer_RWops not compiled with stdio support");
    return NULL;
}
#endif /* ALMIXER_HAVE_STDIO_H */

ALmixer_RWops *
ALmixer_RWFromMem(void *mem, int size)
{
    ALmixer_RWops *rwops = NULL;
    if (!mem) {
      ALmixer_InvalidParamError("mem");
      return rwops;
    }
    if (!size) {
      ALmixer_InvalidParamError("size");
      return rwops;
    }

    rwops = ALmixer_AllocRW();
    if (rwops != NULL) {
        rwops->size = mem_size;
        rwops->seek = mem_seek;
        rwops->read = mem_read;
        rwops->write = mem_write;
        rwops->close = mem_close;
        rwops->hidden.mem.base = (uint8_t *) mem;
        rwops->hidden.mem.here = rwops->hidden.mem.base;
        rwops->hidden.mem.stop = rwops->hidden.mem.base + size;
        rwops->type = ALMIXER_RWOPS_MEMORY;
    }
    return rwops;
}

ALmixer_RWops *
ALmixer_RWFromConstMem(const void *mem, int size)
{
    ALmixer_RWops *rwops = NULL;
    if (!mem) {
      ALmixer_InvalidParamError("mem");
      return rwops;
    }
    if (!size) {
      ALmixer_InvalidParamError("size");
      return rwops;
    }

    rwops = ALmixer_AllocRW();
    if (rwops != NULL) {
        rwops->size = mem_size;
        rwops->seek = mem_seek;
        rwops->read = mem_read;
        rwops->write = mem_writeconst;
        rwops->close = mem_close;
        rwops->hidden.mem.base = (uint8_t *) mem;
        rwops->hidden.mem.here = rwops->hidden.mem.base;
        rwops->hidden.mem.stop = rwops->hidden.mem.base + size;
        rwops->type = ALMIXER_RWOPS_MEMORY_RO;
    }
    return rwops;
}

ALmixer_RWops *
ALmixer_AllocRW(void)
{
    ALmixer_RWops *area;

    area = (ALmixer_RWops *) ALmixer_malloc(sizeof *area);
    if (area == NULL) {
        ALmixer_OutOfMemory();
    } else {
        area->type = ALMIXER_RWOPS_UNKNOWN;
    }
    return area;
}

void
ALmixer_FreeRW(ALmixer_RWops * area)
{
    ALmixer_free(area);
}

/* Functions for dynamically reading and writing endian-specific values */

uint8_t
ALmixer_ReadU8(ALmixer_RWops * src)
{
    uint8_t value = 0;

    ALmixer_RWread(src, &value, sizeof (value), 1);
    return value;
}

uint16_t
ALmixer_ReadLE16(ALmixer_RWops * src)
{
    uint16_t value = 0;

    ALmixer_RWread(src, &value, sizeof (value), 1);
    return ALmixer_SwapLE16(value);
}

uint16_t
ALmixer_ReadBE16(ALmixer_RWops * src)
{
    uint16_t value = 0;

    ALmixer_RWread(src, &value, sizeof (value), 1);
    return ALmixer_SwapBE16(value);
}

uint32_t
ALmixer_ReadLE32(ALmixer_RWops * src)
{
    uint32_t value = 0;

    ALmixer_RWread(src, &value, sizeof (value), 1);
    return ALmixer_SwapLE32(value);
}

uint32_t
ALmixer_ReadBE32(ALmixer_RWops * src)
{
    uint32_t value = 0;

    ALmixer_RWread(src, &value, sizeof (value), 1);
    return ALmixer_SwapBE32(value);
}

uint64_t
ALmixer_ReadLE64(ALmixer_RWops * src)
{
    uint64_t value = 0;

    ALmixer_RWread(src, &value, sizeof (value), 1);
    return ALmixer_SwapLE64(value);
}

uint64_t
ALmixer_ReadBE64(ALmixer_RWops * src)
{
    uint64_t value = 0;

    ALmixer_RWread(src, &value, sizeof (value), 1);
    return ALmixer_SwapBE64(value);
}

size_t
ALmixer_WriteU8(ALmixer_RWops * dst, uint8_t value)
{
    return ALmixer_RWwrite(dst, &value, sizeof (value), 1);
}

size_t
ALmixer_WriteLE16(ALmixer_RWops * dst, uint16_t value)
{
    const uint16_t swapped = ALmixer_SwapLE16(value);
    return ALmixer_RWwrite(dst, &swapped, sizeof (swapped), 1);
}

size_t
ALmixer_WriteBE16(ALmixer_RWops * dst, uint16_t value)
{
    const uint16_t swapped = ALmixer_SwapBE16(value);
    return ALmixer_RWwrite(dst, &swapped, sizeof (swapped), 1);
}

size_t
ALmixer_WriteLE32(ALmixer_RWops * dst, uint32_t value)
{
    const uint32_t swapped = ALmixer_SwapLE32(value);
    return ALmixer_RWwrite(dst, &swapped, sizeof (swapped), 1);
}

size_t
ALmixer_WriteBE32(ALmixer_RWops * dst, uint32_t value)
{
    const uint32_t swapped = ALmixer_SwapBE32(value);
    return ALmixer_RWwrite(dst, &swapped, sizeof (swapped), 1);
}

size_t
ALmixer_WriteLE64(ALmixer_RWops * dst, uint64_t value)
{
    const uint64_t swapped = ALmixer_SwapLE64(value);
    return ALmixer_RWwrite(dst, &swapped, sizeof (swapped), 1);
}

size_t
ALmixer_WriteBE64(ALmixer_RWops * dst, uint64_t value)
{
    const uint64_t swapped = ALmixer_SwapBE64(value);
    return ALmixer_RWwrite(dst, &swapped, sizeof (swapped), 1);
}

#endif /* #ifdef ALMIXER_COMPILE_WITH_SDL */


/* vi: set ts=4 sw=4 expandtab: */
