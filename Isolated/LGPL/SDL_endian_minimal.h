/*
 SDL - Simple DirectMedia Layer
 Copyright (C) 1997-2009 Sam Lantinga
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 
 Sam Lantinga
 slouken@libsdl.org
 */

/* 
 Attention: This is a stripped down file of SDL_endian for our purposes. 
 This code is licensed under the LGPL.
 This means we must not compile this code into anything that we are not willing to
 publicly release source code. 
 You should compile this into a separate dynamic library that is isolated from proprietary code.
 */


#ifndef SDL_endian_minimal_h
#define SDL_endian_minimal_h

#ifdef ANDROID_NDK
#include <endian.h>

#define SDL_BYTEORDER _BYTE_ORDER
#define SDL_BIG_ENDIAN _BIG_ENDIAN
#define SDL_LITTLE_ENDIAN _LITTLE_ENDIAN
#endif

#include <stdint.h>


#if defined(__GNUC__) && defined(__i386__) && \
   !(__GNUC__ == 2 && __GNUC_MINOR__ <= 95 /* broken gcc version */)
static __inline__ uint32_t SDL_Swap32(uint32_t x)
{
	__asm__("bswap %0" : "=r" (x) : "0" (x));
	return x;
}
#elif defined(__GNUC__) && defined(__x86_64__)
static __inline__ uint32_t SDL_Swap32(uint32_t x)
{
	__asm__("bswapl %0" : "=r" (x) : "0" (x));
	return x;
}
#elif defined(__GNUC__) && (defined(__powerpc__) || defined(__ppc__))
static __inline__ uint32_t SDL_Swap32(uint32_t x)
{
	uint32_t result;

	__asm__("rlwimi %0,%2,24,16,23" : "=&r" (result) : "0" (x>>24), "r" (x));
	__asm__("rlwimi %0,%2,8,8,15"   : "=&r" (result) : "0" (result),    "r" (x));
	__asm__("rlwimi %0,%2,24,0,7"   : "=&r" (result) : "0" (result),    "r" (x));
	return result;
}
#elif defined(__GNUC__) && (defined(__M68000__) || defined(__M68020__))
static __inline__ uint32_t SDL_Swap32(uint32_t x)
{
	__asm__("rorw #8,%0\n\tswap %0\n\trorw #8,%0" : "=d" (x) :  "0" (x) : "cc");
	return x;
}
#else
static __inline__ uint32_t SDL_Swap32(uint32_t x) {
	return((x<<24)|((x<<8)&0x00FF0000)|((x>>8)&0x0000FF00)|(x>>24));
}
#endif


#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define SDL_SwapLE16(X)	SDL_Swap16(X)
#define SDL_SwapLE32(X)	SDL_Swap32(X)
#define SDL_SwapLE64(X)	SDL_Swap64(X)
#define SDL_SwapBE16(X)	(X)
#define SDL_SwapBE32(X)	(X)
#define SDL_SwapBE64(X)	(X)
#else
#define SDL_SwapLE16(X)	(X)
#define SDL_SwapLE32(X)	(X)
#define SDL_SwapLE64(X)	(X)
#define SDL_SwapBE16(X)	SDL_Swap16(X)
#define SDL_SwapBE32(X)	SDL_Swap32(X)
#define SDL_SwapBE64(X)	SDL_Swap64(X)
#endif


#endif // SDL_endian_minimal_h
