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
 *  \file ALmixer_endian.h
 *
 *  Functions for reading and writing endian-specific values
 */

#ifndef _ALmixer_endian_h
#define _ALmixer_endian_h

#include "ALmixer_stdinc.h"

/**
 *  \name The two types of endianness
 */
/* @{ */
#define ALmixer_LIL_ENDIAN  1234
#define ALmixer_BIG_ENDIAN  4321
/* @} */

#ifndef ALmixer_BYTEORDER           /* Not defined in ALmixer_config.h? */
#ifdef __linux__
#include <endian.h>
#define ALmixer_BYTEORDER  __BYTE_ORDER
#else /* __linux __ */
#if defined(__hppa__) || \
    defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || \
    (defined(__MIPS__) && defined(__MISPEB__)) || \
    defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || \
    defined(__sparc__)
#define ALmixer_BYTEORDER   ALmixer_BIG_ENDIAN
#else
#define ALmixer_BYTEORDER   ALmixer_LIL_ENDIAN
#endif
#endif /* __linux __ */
#endif /* !ALmixer_BYTEORDER */


#include "ALmixer_begin_code.h"



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

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 *  \file ALmixer_endian.h
 */
#if defined(__GNUC__) && defined(__i386__) && \
   !(__GNUC__ == 2 && __GNUC_MINOR__ == 95 /* broken gcc version */)
ALMIXER_FORCE_INLINE uint16_t
ALmixer_Swap16(uint16_t x)
{
  __asm__("xchgb %b0,%h0": "=q"(x):"0"(x));
    return x;
}
#elif defined(__GNUC__) && defined(__x86_64__)
ALMIXER_FORCE_INLINE uint16_t
ALmixer_Swap16(uint16_t x)
{
  __asm__("xchgb %b0,%h0": "=Q"(x):"0"(x));
    return x;
}
#elif defined(__GNUC__) && (defined(__powerpc__) || defined(__ppc__))
ALMIXER_FORCE_INLINE uint16_t
ALmixer_Swap16(uint16_t x)
{
    int result;

  __asm__("rlwimi %0,%2,8,16,23": "=&r"(result):"0"(x >> 8), "r"(x));
    return (uint16_t)result;
}
#elif defined(__GNUC__) && (defined(__M68000__) || defined(__M68020__)) && !defined(__mcoldfire__)
ALMIXER_FORCE_INLINE uint16_t
ALmixer_Swap16(uint16_t x)
{
  __asm__("rorw #8,%0": "=d"(x): "0"(x):"cc");
    return x;
}
#else
ALMIXER_FORCE_INLINE uint16_t
ALmixer_Swap16(uint16_t x)
{
    return ALmixer_static_cast(uint16_t, ((x << 8) | (x >> 8)));
}
#endif

#if defined(__GNUC__) && defined(__i386__)
ALMIXER_FORCE_INLINE uint32_t
ALmixer_Swap32(uint32_t x)
{
  __asm__("bswap %0": "=r"(x):"0"(x));
    return x;
}
#elif defined(__GNUC__) && defined(__x86_64__)
ALMIXER_FORCE_INLINE uint32_t
ALmixer_Swap32(uint32_t x)
{
  __asm__("bswapl %0": "=r"(x):"0"(x));
    return x;
}
#elif defined(__GNUC__) && (defined(__powerpc__) || defined(__ppc__))
ALMIXER_FORCE_INLINE uint32_t
ALmixer_Swap32(uint32_t x)
{
    uint32_t result;

  __asm__("rlwimi %0,%2,24,16,23": "=&r"(result):"0"(x >> 24), "r"(x));
  __asm__("rlwimi %0,%2,8,8,15": "=&r"(result):"0"(result), "r"(x));
  __asm__("rlwimi %0,%2,24,0,7": "=&r"(result):"0"(result), "r"(x));
    return result;
}
#elif defined(__GNUC__) && (defined(__M68000__) || defined(__M68020__)) && !defined(__mcoldfire__)
ALMIXER_FORCE_INLINE uint32_t
ALmixer_Swap32(uint32_t x)
{
  __asm__("rorw #8,%0\n\tswap %0\n\trorw #8,%0": "=d"(x): "0"(x):"cc");
    return x;
}
#else
ALMIXER_FORCE_INLINE uint32_t
ALmixer_Swap32(uint32_t x)
{
    return ALmixer_static_cast(uint32_t, ((x << 24) | ((x << 8) & 0x00FF0000) |
                                    ((x >> 8) & 0x0000FF00) | (x >> 24)));
}
#endif

#if defined(__GNUC__) && defined(__i386__)
ALMIXER_FORCE_INLINE uint64_t
ALmixer_Swap64(uint64_t x)
{
    union
    {
        struct
        {
            uint32_t a, b;
        } s;
        uint64_t u;
    } v;
    v.u = x;
  __asm__("bswapl %0 ; bswapl %1 ; xchgl %0,%1": "=r"(v.s.a), "=r"(v.s.b):"0"(v.s.a),
            "1"(v.s.
                b));
    return v.u;
}
#elif defined(__GNUC__) && defined(__x86_64__)
ALMIXER_FORCE_INLINE uint64_t
ALmixer_Swap64(uint64_t x)
{
  __asm__("bswapq %0": "=r"(x):"0"(x));
    return x;
}
#else
ALMIXER_FORCE_INLINE uint64_t
ALmixer_Swap64(uint64_t x)
{
    uint32_t hi, lo;

    /* Separate into high and low 32-bit values and swap them */
    lo = ALmixer_static_cast(uint32_t, x & 0xFFFFFFFF);
    x >>= 32;
    hi = ALmixer_static_cast(uint32_t, x & 0xFFFFFFFF);
    x = ALmixer_Swap32(lo);
    x <<= 32;
    x |= ALmixer_Swap32(hi);
    return (x);
}
#endif


ALMIXER_FORCE_INLINE float
ALmixer_SwapFloat(float x)
{
    union
    {
        float f;
        uint32_t ui32;
    } swapper;
    swapper.f = x;
    swapper.ui32 = ALmixer_Swap32(swapper.ui32);
    return swapper.f;
}


/**
 *  \name Swap to native
 *  Byteswap item from the specified endianness to the native endianness.
 */
/* @{ */
#if ALmixer_BYTEORDER == ALmixer_LIL_ENDIAN
#define ALmixer_SwapLE16(X) (X)
#define ALmixer_SwapLE32(X) (X)
#define ALmixer_SwapLE64(X) (X)
#define ALmixer_SwapFloatLE(X)  (X)
#define ALmixer_SwapBE16(X) ALmixer_Swap16(X)
#define ALmixer_SwapBE32(X) ALmixer_Swap32(X)
#define ALmixer_SwapBE64(X) ALmixer_Swap64(X)
#define ALmixer_SwapFloatBE(X)  ALmixer_SwapFloat(X)
#else
#define ALmixer_SwapLE16(X) ALmixer_Swap16(X)
#define ALmixer_SwapLE32(X) ALmixer_Swap32(X)
#define ALmixer_SwapLE64(X) ALmixer_Swap64(X)
#define ALmixer_SwapFloatLE(X)  ALmixer_SwapFloat(X)
#define ALmixer_SwapBE16(X) (X)
#define ALmixer_SwapBE32(X) (X)
#define ALmixer_SwapBE64(X) (X)
#define ALmixer_SwapFloatBE(X)  (X)
#endif
/* @} *//* Swap to native */

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif
#include "ALmixer_close_code.h"

#endif /* _ALmixer_endian_h */

/* vi: set ts=4 sw=4 expandtab: */
