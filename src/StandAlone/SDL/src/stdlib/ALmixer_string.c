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
#include "../ALmixer_internal.h"

/* This file contains portable string manipulation functions for ALmixer */

#include "ALmixer_stdinc.h"

/* Used for ALmixer_bool */
#include "ALmixer_RWops.h"

#define ALmixer_isupperhex(X)   (((X) >= 'A') && ((X) <= 'F'))
#define ALmixer_islowerhex(X)   (((X) >= 'a') && ((X) <= 'f'))

#if 0
#define UTF8_IsLeadByte(c) ((c) >= 0xC0 && (c) <= 0xF4)
#define UTF8_IsTrailingByte(c) ((c) >= 0x80 && (c) <= 0xBF)

static int UTF8_TrailingBytes(unsigned char c)
{
    if (c >= 0xC0 && c <= 0xDF)
        return 1;
    else if (c >= 0xE0 && c <= 0xEF)
        return 2;
    else if (c >= 0xF0 && c <= 0xF4)
        return 3;
    else
        return 0;
}

#endif /* #if 0 */

#if !defined(HAVE_VSSCANF) || !defined(HAVE_STRTOL)
static size_t
ALmixer_ScanLong(const char *text, int radix, long *valuep)
{
    const char *textstart = text;
    long value = 0;
    ALmixer_bool negative = ALMIXER_FALSE;

    if (*text == '-') {
        negative = ALMIXER_TRUE;
        ++text;
    }
    if (radix == 16 && ALmixer_strncmp(text, "0x", 2) == 0) {
        text += 2;
    }
    for (;;) {
        int v;
        if (ALmixer_isdigit((unsigned char) *text)) {
            v = *text - '0';
        } else if (radix == 16 && ALmixer_isupperhex(*text)) {
            v = 10 + (*text - 'A');
        } else if (radix == 16 && ALmixer_islowerhex(*text)) {
            v = 10 + (*text - 'a');
        } else {
            break;
        }
        value *= radix;
        value += v;
        ++text;
    }
    if (valuep) {
        if (negative && value) {
            *valuep = -value;
        } else {
            *valuep = value;
        }
    }
    return (text - textstart);
}
#endif

#if 0
#if !defined(HAVE_VSSCANF) || !defined(HAVE_STRTOUL) || !defined(HAVE_STRTOD)
static size_t
ALmixer_ScanUnsignedLong(const char *text, int radix, unsigned long *valuep)
{
    const char *textstart = text;
    unsigned long value = 0;

    if (radix == 16 && ALmixer_strncmp(text, "0x", 2) == 0) {
        text += 2;
    }
    for (;;) {
        int v;
        if (ALmixer_isdigit((unsigned char) *text)) {
            v = *text - '0';
        } else if (radix == 16 && ALmixer_isupperhex(*text)) {
            v = 10 + (*text - 'A');
        } else if (radix == 16 && ALmixer_islowerhex(*text)) {
            v = 10 + (*text - 'a');
        } else {
            break;
        }
        value *= radix;
        value += v;
        ++text;
    }
    if (valuep) {
        *valuep = value;
    }
    return (text - textstart);
}
#endif

#ifndef HAVE_VSSCANF
static size_t
ALmixer_ScanUintPtrT(const char *text, int radix, uintptr_t * valuep)
{
    const char *textstart = text;
    uintptr_t value = 0;

    if (radix == 16 && ALmixer_strncmp(text, "0x", 2) == 0) {
        text += 2;
    }
    for (;;) {
        int v;
        if (ALmixer_isdigit((unsigned char) *text)) {
            v = *text - '0';
        } else if (radix == 16 && ALmixer_isupperhex(*text)) {
            v = 10 + (*text - 'A');
        } else if (radix == 16 && ALmixer_islowerhex(*text)) {
            v = 10 + (*text - 'a');
        } else {
            break;
        }
        value *= radix;
        value += v;
        ++text;
    }
    if (valuep) {
        *valuep = value;
    }
    return (text - textstart);
}
#endif

#if !defined(HAVE_VSSCANF) || !defined(HAVE_STRTOLL)
static size_t
ALmixer_ScanLongLong(const char *text, int radix, int64_t * valuep)
{
    const char *textstart = text;
    int64_t value = 0;
    ALmixer_bool negative = ALMIXER_FALSE;

    if (*text == '-') {
        negative = ALMIXER_TRUE;
        ++text;
    }
    if (radix == 16 && ALmixer_strncmp(text, "0x", 2) == 0) {
        text += 2;
    }
    for (;;) {
        int v;
        if (ALmixer_isdigit((unsigned char) *text)) {
            v = *text - '0';
        } else if (radix == 16 && ALmixer_isupperhex(*text)) {
            v = 10 + (*text - 'A');
        } else if (radix == 16 && ALmixer_islowerhex(*text)) {
            v = 10 + (*text - 'a');
        } else {
            break;
        }
        value *= radix;
        value += v;
        ++text;
    }
    if (valuep) {
        if (negative && value) {
            *valuep = -value;
        } else {
            *valuep = value;
        }
    }
    return (text - textstart);
}
#endif

#if !defined(HAVE_VSSCANF) || !defined(HAVE_STRTOULL)
static size_t
ALmixer_ScanUnsignedLongLong(const char *text, int radix, uint64_t * valuep)
{
    const char *textstart = text;
    uint64_t value = 0;

    if (radix == 16 && ALmixer_strncmp(text, "0x", 2) == 0) {
        text += 2;
    }
    for (;;) {
        int v;
        if (ALmixer_isdigit((unsigned char) *text)) {
            v = *text - '0';
        } else if (radix == 16 && ALmixer_isupperhex(*text)) {
            v = 10 + (*text - 'A');
        } else if (radix == 16 && ALmixer_islowerhex(*text)) {
            v = 10 + (*text - 'a');
        } else {
            break;
        }
        value *= radix;
        value += v;
        ++text;
    }
    if (valuep) {
        *valuep = value;
    }
    return (text - textstart);
}
#endif

#if !defined(HAVE_VSSCANF) || !defined(HAVE_STRTOD)
static size_t
ALmixer_ScanFloat(const char *text, double *valuep)
{
    const char *textstart = text;
    unsigned long lvalue = 0;
    double value = 0.0;
    ALmixer_bool negative = ALMIXER_FALSE;

    if (*text == '-') {
        negative = ALMIXER_TRUE;
        ++text;
    }
    text += ALmixer_ScanUnsignedLong(text, 10, &lvalue);
    value += lvalue;
    if (*text == '.') {
        int mult = 10;
        ++text;
        while (ALmixer_isdigit((unsigned char) *text)) {
            lvalue = *text - '0';
            value += (double) lvalue / mult;
            mult *= 10;
            ++text;
        }
    }
    if (valuep) {
        if (negative && value) {
            *valuep = -value;
        } else {
            *valuep = value;
        }
    }
    return (text - textstart);
}
#endif
#endif /* #if 0 */

void *
ALmixer_memset(void *dst, int c, size_t len)
{
#if defined(HAVE_MEMSET)
    return memset(dst, c, len);
#else
    size_t left = (len % 4);
    uint32_t *dstp4;
    uint8_t *dstp1;
    uint32_t value4 = (c | (c << 8) | (c << 16) | (c << 24));
    uint8_t value1 = (uint8_t) c;

    dstp4 = (uint32_t *) dst;
    len /= 4;
    while (len--) {
        *dstp4++ = value4;
    }

    dstp1 = (uint8_t *) dstp4;
    switch (left) {
    case 3:
        *dstp1++ = value1;
    case 2:
        *dstp1++ = value1;
    case 1:
        *dstp1++ = value1;
    }

    return dst;
#endif /* HAVE_MEMSET */
}

void *
ALmixer_memcpy(void *dst, const void *src, size_t len)
{
#ifdef __GNUC__
    /* Presumably this is well tuned for speed.
       On my machine this is twice as fast as the C code below.
     */
    return __builtin_memcpy(dst, src, len);
#elif defined(HAVE_MEMCPY)
    return memcpy(dst, src, len);
#elif defined(HAVE_BCOPY)
    bcopy(src, dst, len);
    return dst;
#else
    /* GCC 4.9.0 with -O3 will generate movaps instructions with the loop
       using uint32_t* pointers, so we need to make sure the pointers are
       aligned before we loop using them.
     */
    if (((intptr_t)src & 0x3) || ((intptr_t)dst & 0x3)) {
        /* Do an unaligned byte copy */
		uint8_t *srcp1 = (uint8_t *)src;
        uint8_t *dstp1 = (uint8_t *)dst;

        while (len--) {
            *dstp1++ = *srcp1++;
        }
    } else {
        size_t left = (len % 4);
        uint32_t *srcp4, *dstp4;
        uint8_t *srcp1, *dstp1;

        srcp4 = (uint32_t *) src;
        dstp4 = (uint32_t *) dst;
        len /= 4;
        while (len--) {
            *dstp4++ = *srcp4++;
        }

        srcp1 = (uint8_t *) srcp4;
        dstp1 = (uint8_t *) dstp4;
        switch (left) {
        case 3:
            *dstp1++ = *srcp1++;
        case 2:
            *dstp1++ = *srcp1++;
        case 1:
            *dstp1++ = *srcp1++;
        }
    }
    return dst;
#endif /* __GNUC__ */
}
#if 0
void *
ALmixer_memmove(void *dst, const void *src, size_t len)
{
#if defined(HAVE_MEMMOVE)
    return memmove(dst, src, len);
#else
    char *srcp = (char *) src;
    char *dstp = (char *) dst;

    if (src < dst) {
        srcp += len - 1;
        dstp += len - 1;
        while (len--) {
            *dstp-- = *srcp--;
        }
    } else {
        while (len--) {
            *dstp++ = *srcp++;
        }
    }
    return dst;
#endif /* HAVE_MEMMOVE */
}

int
ALmixer_memcmp(const void *s1, const void *s2, size_t len)
{
#if defined(HAVE_MEMCMP)
    return memcmp(s1, s2, len);
#else
    char *s1p = (char *) s1;
    char *s2p = (char *) s2;
    while (len--) {
        if (*s1p != *s2p) {
            return (*s1p - *s2p);
        }
        ++s1p;
        ++s2p;
    }
    return 0;
#endif /* HAVE_MEMCMP */
}
#endif /* #if 0 */

size_t
ALmixer_strlen(const char *string)
{
#if defined(HAVE_STRLEN)
    return strlen(string);
#else
    size_t len = 0;
    while (*string++) {
        ++len;
    }
    return len;
#endif /* HAVE_STRLEN */
}

size_t
ALmixer_wcslen(const wchar_t * string)
{
#if defined(HAVE_WCSLEN)
    return wcslen(string);
#else
    size_t len = 0;
    while (*string++) {
        ++len;
    }
    return len;
#endif /* HAVE_WCSLEN */
}

#if 0

size_t
ALmixer_wcslcpy(wchar_t *dst, const wchar_t *src, size_t maxlen)
{
#if defined(HAVE_WCSLCPY)
    return wcslcpy(dst, src, maxlen);
#else
    size_t srclen = ALmixer_wcslen(src);
    if (maxlen > 0) {
        size_t len = ALmixer_min(srclen, maxlen - 1);
        ALmixer_memcpy(dst, src, len * sizeof(wchar_t));
        dst[len] = '\0';
    }
    return srclen;
#endif /* HAVE_WCSLCPY */
}

size_t
ALmixer_wcslcat(wchar_t *dst, const wchar_t *src, size_t maxlen)
{
#if defined(HAVE_WCSLCAT)
    return wcslcat(dst, src, maxlen);
#else
    size_t dstlen = ALmixer_wcslen(dst);
    size_t srclen = ALmixer_wcslen(src);
    if (dstlen < maxlen) {
        ALmixer_wcslcpy(dst + dstlen, src, maxlen - dstlen);
    }
    return dstlen + srclen;
#endif /* HAVE_WCSLCAT */
}
#endif /* #if 0 */

size_t
ALmixer_strlcpy(char *dst, const char *src, size_t maxlen)
{
#if defined(HAVE_STRLCPY)
    return strlcpy(dst, src, maxlen);
#else
    size_t srclen = ALmixer_strlen(src);
    if (maxlen > 0) {
        size_t len = ALmixer_min(srclen, maxlen - 1);
        ALmixer_memcpy(dst, src, len);
        dst[len] = '\0';
    }
    return srclen;
#endif /* HAVE_STRLCPY */
}

#if 0
size_t ALmixer_utf8strlcpy(char *dst, const char *src, size_t dst_bytes)
{
    size_t src_bytes = ALmixer_strlen(src);
    size_t bytes = ALmixer_min(src_bytes, dst_bytes - 1);
    size_t i = 0;
    char trailing_bytes = 0;
    if (bytes)
    {
        unsigned char c = (unsigned char)src[bytes - 1];
        if (UTF8_IsLeadByte(c))
            --bytes;
        else if (UTF8_IsTrailingByte(c))
        {
            for (i = bytes - 1; i != 0; --i)
            {
                c = (unsigned char)src[i];
                trailing_bytes = UTF8_TrailingBytes(c);
                if (trailing_bytes)
                {
                    if (bytes - i != trailing_bytes + 1)
                        bytes = i;

                    break;
                }
            }
        }
        ALmixer_memcpy(dst, src, bytes);
    }
    dst[bytes] = '\0';
    return bytes;
}

size_t
ALmixer_strlcat(char *dst, const char *src, size_t maxlen)
{
#if defined(HAVE_STRLCAT)
    return strlcat(dst, src, maxlen);
#else
    size_t dstlen = ALmixer_strlen(dst);
    size_t srclen = ALmixer_strlen(src);
    if (dstlen < maxlen) {
        ALmixer_strlcpy(dst + dstlen, src, maxlen - dstlen);
    }
    return dstlen + srclen;
#endif /* HAVE_STRLCAT */
}
#endif /* #if 0 */

char *
ALmixer_strdup(const char *string)
{
#if defined(HAVE_STRDUP)
    return strdup(string);
#else
    size_t len = ALmixer_strlen(string) + 1;
    char *newstr = ALmixer_malloc(len);
    if (newstr) {
        ALmixer_strlcpy(newstr, string, len);
    }
    return newstr;
#endif /* HAVE_STRDUP */
}

char *
ALmixer_strrev(char *string)
{
#if defined(HAVE__STRREV)
    return _strrev(string);
#else
    size_t len = ALmixer_strlen(string);
    char *a = &string[0];
    char *b = &string[len - 1];
    len /= 2;
    while (len--) {
        char c = *a;
        *a++ = *b;
        *b-- = c;
    }
    return string;
#endif /* HAVE__STRREV */
}

char *
ALmixer_strupr(char *string)
{
#if defined(HAVE__STRUPR)
    return _strupr(string);
#else
    char *bufp = string;
    while (*bufp) {
        *bufp = ALmixer_toupper((unsigned char) *bufp);
        ++bufp;
    }
    return string;
#endif /* HAVE__STRUPR */
}

char *
ALmixer_strlwr(char *string)
{
#if defined(HAVE__STRLWR)
    return _strlwr(string);
#else
    char *bufp = string;
    while (*bufp) {
        *bufp = ALmixer_tolower((unsigned char) *bufp);
        ++bufp;
    }
    return string;
#endif /* HAVE__STRLWR */
}

char *
ALmixer_strchr(const char *string, int c)
{
#ifdef HAVE_STRCHR
    return ALmixer_const_cast(char*,strchr(string, c));
#elif defined(HAVE_INDEX)
    return ALmixer_const_cast(char*,index(string, c));
#else
    while (*string) {
        if (*string == c) {
            return (char *) string;
        }
        ++string;
    }
    return NULL;
#endif /* HAVE_STRCHR */
}

#if 0
char *
ALmixer_strrchr(const char *string, int c)
{
#ifdef HAVE_STRRCHR
    return ALmixer_const_cast(char*,strrchr(string, c));
#elif defined(HAVE_RINDEX)
    return ALmixer_const_cast(char*,rindex(string, c));
#else
    const char *bufp = string + ALmixer_strlen(string) - 1;
    while (bufp >= string) {
        if (*bufp == c) {
            return (char *) bufp;
        }
        --bufp;
    }
    return NULL;
#endif /* HAVE_STRRCHR */
}

char *
ALmixer_strstr(const char *haystack, const char *needle)
{
#if defined(HAVE_STRSTR)
    return ALmixer_const_cast(char*,strstr(haystack, needle));
#else
    size_t length = ALmixer_strlen(needle);
    while (*haystack) {
        if (ALmixer_strncmp(haystack, needle, length) == 0) {
            return (char *) haystack;
        }
        ++haystack;
    }
    return NULL;
#endif /* HAVE_STRSTR */
}
#endif /* #if 0 */

#if !defined(HAVE__LTOA) || !defined(HAVE__I64TOA) || \
    !defined(HAVE__ULTOA) || !defined(HAVE__UI64TOA)
static const char ntoa_table[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y', 'Z'
};
#endif /* ntoa() conversion table */

char *
ALmixer_itoa(int value, char *string, int radix)
{
#ifdef HAVE_ITOA
    return itoa(value, string, radix);
#else
    return ALmixer_ltoa((long)value, string, radix);
#endif /* HAVE_ITOA */
}

char *
ALmixer_uitoa(unsigned int value, char *string, int radix)
{
#ifdef HAVE__UITOA
    return _uitoa(value, string, radix);
#else
    return ALmixer_ultoa((unsigned long)value, string, radix);
#endif /* HAVE__UITOA */
}

char *
ALmixer_ltoa(long value, char *string, int radix)
{
#if defined(HAVE__LTOA)
    return _ltoa(value, string, radix);
#else
    char *bufp = string;

    if (value < 0) {
        *bufp++ = '-';
        ALmixer_ultoa(-value, bufp, radix);
    } else {
        ALmixer_ultoa(value, bufp, radix);
    }

    return string;
#endif /* HAVE__LTOA */
}

char *
ALmixer_ultoa(unsigned long value, char *string, int radix)
{
#if defined(HAVE__ULTOA)
    return _ultoa(value, string, radix);
#else
    char *bufp = string;

    if (value) {
        while (value > 0) {
            *bufp++ = ntoa_table[value % radix];
            value /= radix;
        }
    } else {
        *bufp++ = '0';
    }
    *bufp = '\0';

    /* The numbers went into the string backwards. :) */
    ALmixer_strrev(string);

    return string;
#endif /* HAVE__ULTOA */
}

char *
ALmixer_lltoa(int64_t value, char *string, int radix)
{
#if defined(HAVE__I64TOA)
    return _i64toa(value, string, radix);
#else
    char *bufp = string;

    if (value < 0) {
        *bufp++ = '-';
        ALmixer_ulltoa(-value, bufp, radix);
    } else {
        ALmixer_ulltoa(value, bufp, radix);
    }

    return string;
#endif /* HAVE__I64TOA */
}

char *
ALmixer_ulltoa(uint64_t value, char *string, int radix)
{
#if defined(HAVE__UI64TOA)
    return _ui64toa(value, string, radix);
#else
    char *bufp = string;

    if (value) {
        while (value > 0) {
            *bufp++ = ntoa_table[value % radix];
            value /= radix;
        }
    } else {
        *bufp++ = '0';
    }
    *bufp = '\0';

    /* The numbers went into the string backwards. :) */
    ALmixer_strrev(string);

    return string;
#endif /* HAVE__UI64TOA */
}

#if 0
int ALmixer_atoi(const char *string)
{
#ifdef HAVE_ATOI
    return atoi(string);
#else
    return ALmixer_strtol(string, NULL, 0);
#endif /* HAVE_ATOI */
}

double ALmixer_atof(const char *string)
{
#ifdef HAVE_ATOF
    return (double) atof(string);
#else
    return ALmixer_strtod(string, NULL);
#endif /* HAVE_ATOF */
}
#endif /* #if 0 */

long
ALmixer_strtol(const char *string, char **endp, int base)
{
#if defined(HAVE_STRTOL)
    return strtol(string, endp, base);
#else
    size_t len;
    long value;

    if (!base) {
        if ((ALmixer_strlen(string) > 2) && (ALmixer_strncmp(string, "0x", 2) == 0)) {
            base = 16;
        } else {
            base = 10;
        }
    }

    len = ALmixer_ScanLong(string, base, &value);
    if (endp) {
        *endp = (char *) string + len;
    }
    return value;
#endif /* HAVE_STRTOL */
}

#if 0
unsigned long
ALmixer_strtoul(const char *string, char **endp, int base)
{
#if defined(HAVE_STRTOUL)
    return strtoul(string, endp, base);
#else
    size_t len;
    unsigned long value;

    if (!base) {
        if ((ALmixer_strlen(string) > 2) && (ALmixer_strncmp(string, "0x", 2) == 0)) {
            base = 16;
        } else {
            base = 10;
        }
    }

    len = ALmixer_ScanUnsignedLong(string, base, &value);
    if (endp) {
        *endp = (char *) string + len;
    }
    return value;
#endif /* HAVE_STRTOUL */
}

int64_t
ALmixer_strtoll(const char *string, char **endp, int base)
{
#if defined(HAVE_STRTOLL)
    return strtoll(string, endp, base);
#else
    size_t len;
    int64_t value;

    if (!base) {
        if ((ALmixer_strlen(string) > 2) && (ALmixer_strncmp(string, "0x", 2) == 0)) {
            base = 16;
        } else {
            base = 10;
        }
    }

    len = ALmixer_ScanLongLong(string, base, &value);
    if (endp) {
        *endp = (char *) string + len;
    }
    return value;
#endif /* HAVE_STRTOLL */
}

uint64_t
ALmixer_strtoull(const char *string, char **endp, int base)
{
#if defined(HAVE_STRTOULL)
    return strtoull(string, endp, base);
#else
    size_t len;
    uint64_t value;

    if (!base) {
        if ((ALmixer_strlen(string) > 2) && (ALmixer_strncmp(string, "0x", 2) == 0)) {
            base = 16;
        } else {
            base = 10;
        }
    }

    len = ALmixer_ScanUnsignedLongLong(string, base, &value);
    if (endp) {
        *endp = (char *) string + len;
    }
    return value;
#endif /* HAVE_STRTOULL */
}

double
ALmixer_strtod(const char *string, char **endp)
{
#if defined(HAVE_STRTOD)
    return strtod(string, endp);
#else
    size_t len;
    double value;

    len = ALmixer_ScanFloat(string, &value);
    if (endp) {
        *endp = (char *) string + len;
    }
    return value;
#endif /* HAVE_STRTOD */
}

#endif /* #if 0 */

int
ALmixer_strcmp(const char *str1, const char *str2)
{
#if defined(HAVE_STRCMP)
    return strcmp(str1, str2);
#else
    while (*str1 && *str2) {
        if (*str1 != *str2)
            break;
        ++str1;
        ++str2;
    }
    return (int) ((unsigned char) *str1 - (unsigned char) *str2);
#endif /* HAVE_STRCMP */
}


int
ALmixer_strncmp(const char *str1, const char *str2, size_t maxlen)
{
#if defined(HAVE_STRNCMP)
    return strncmp(str1, str2, maxlen);
#else
    while (*str1 && *str2 && maxlen) {
        if (*str1 != *str2)
            break;
        ++str1;
        ++str2;
        --maxlen;
    }
    if (!maxlen) {
        return 0;
    }
    return (int) ((unsigned char) *str1 - (unsigned char) *str2);
#endif /* HAVE_STRNCMP */
}

int
ALmixer_strcasecmp(const char *str1, const char *str2)
{
#ifdef HAVE_STRCASECMP
    return strcasecmp(str1, str2);
#elif defined(HAVE__STRICMP)
    return _stricmp(str1, str2);
#else
    char a = 0;
    char b = 0;
    while (*str1 && *str2) {
        a = ALmixer_toupper((unsigned char) *str1);
        b = ALmixer_toupper((unsigned char) *str2);
        if (a != b)
            break;
        ++str1;
        ++str2;
    }
    a = ALmixer_toupper(*str1);
    b = ALmixer_toupper(*str2);
    return (int) ((unsigned char) a - (unsigned char) b);
#endif /* HAVE_STRCASECMP */
}

#if 0

int
ALmixer_strncasecmp(const char *str1, const char *str2, size_t maxlen)
{
#ifdef HAVE_STRNCASECMP
    return strncasecmp(str1, str2, maxlen);
#elif defined(HAVE__STRNICMP)
    return _strnicmp(str1, str2, maxlen);
#else
    char a = 0;
    char b = 0;
    while (*str1 && *str2 && maxlen) {
        a = ALmixer_tolower((unsigned char) *str1);
        b = ALmixer_tolower((unsigned char) *str2);
        if (a != b)
            break;
        ++str1;
        ++str2;
        --maxlen;
    }
    if (maxlen == 0) {
        return 0;
    } else {
        a = ALmixer_tolower((unsigned char) *str1);
        b = ALmixer_tolower((unsigned char) *str2);
        return (int) ((unsigned char) a - (unsigned char) b);
    }
#endif /* HAVE_STRNCASECMP */
}

int
ALmixer_sscanf(const char *text, const char *fmt, ...)
{
    int rc;
    va_list ap;
    va_start(ap, fmt);
    rc = ALmixer_vsscanf(text, fmt, ap);
    va_end(ap);
    return rc;
}

#ifdef HAVE_VSSCANF
int
ALmixer_vsscanf(const char *text, const char *fmt, va_list ap)
{
    return vsscanf(text, fmt, ap);
}
#else
int
ALmixer_vsscanf(const char *text, const char *fmt, va_list ap)
{
    int retval = 0;

    while (*fmt) {
        if (*fmt == ' ') {
            while (ALmixer_isspace((unsigned char) *text)) {
                ++text;
            }
            ++fmt;
            continue;
        }
        if (*fmt == '%') {
            ALmixer_bool done = ALMIXER_FALSE;
            long count = 0;
            int radix = 10;
            enum
            {
                DO_SHORT,
                DO_INT,
                DO_LONG,
                DO_LONGLONG
            } inttype = DO_INT;
            ALmixer_bool suppress = ALMIXER_FALSE;

            ++fmt;
            if (*fmt == '%') {
                if (*text == '%') {
                    ++text;
                    ++fmt;
                    continue;
                }
                break;
            }
            if (*fmt == '*') {
                suppress = ALMIXER_TRUE;
                ++fmt;
            }
            fmt += ALmixer_ScanLong(fmt, 10, &count);

            if (*fmt == 'c') {
                if (!count) {
                    count = 1;
                }
                if (suppress) {
                    while (count--) {
                        ++text;
                    }
                } else {
                    char *valuep = va_arg(ap, char *);
                    while (count--) {
                        *valuep++ = *text++;
                    }
                    ++retval;
                }
                continue;
            }

            while (ALmixer_isspace((unsigned char) *text)) {
                ++text;
            }

            /* FIXME: implement more of the format specifiers */
            while (!done) {
                switch (*fmt) {
                case '*':
                    suppress = ALMIXER_TRUE;
                    break;
                case 'h':
                    if (inttype > DO_SHORT) {
                        ++inttype;
                    }
                    break;
                case 'l':
                    if (inttype < DO_LONGLONG) {
                        ++inttype;
                    }
                    break;
                case 'I':
                    if (ALmixer_strncmp(fmt, "I64", 3) == 0) {
                        fmt += 2;
                        inttype = DO_LONGLONG;
                    }
                    break;
                case 'i':
                    {
                        int index = 0;
                        if (text[index] == '-') {
                            ++index;
                        }
                        if (text[index] == '0') {
                            if (ALmixer_tolower((unsigned char) text[index + 1]) == 'x') {
                                radix = 16;
                            } else {
                                radix = 8;
                            }
                        }
                    }
                    /* Fall through to %d handling */
                case 'd':
                    if (inttype == DO_LONGLONG) {
                        int64_t value;
                        text += ALmixer_ScanLongLong(text, radix, &value);
                        if (!suppress) {
                            int64_t *valuep = va_arg(ap, int64_t *);
                            *valuep = value;
                            ++retval;
                        }
                    } else {
                        long value;
                        text += ALmixer_ScanLong(text, radix, &value);
                        if (!suppress) {
                            switch (inttype) {
                            case DO_SHORT:
                                {
                                    short *valuep = va_arg(ap, short *);
                                    *valuep = (short) value;
                                }
                                break;
                            case DO_INT:
                                {
                                    int *valuep = va_arg(ap, int *);
                                    *valuep = (int) value;
                                }
                                break;
                            case DO_LONG:
                                {
                                    long *valuep = va_arg(ap, long *);
                                    *valuep = value;
                                }
                                break;
                            case DO_LONGLONG:
                                /* Handled above */
                                break;
                            }
                            ++retval;
                        }
                    }
                    done = ALMIXER_TRUE;
                    break;
                case 'o':
                    if (radix == 10) {
                        radix = 8;
                    }
                    /* Fall through to unsigned handling */
                case 'x':
                case 'X':
                    if (radix == 10) {
                        radix = 16;
                    }
                    /* Fall through to unsigned handling */
                case 'u':
                    if (inttype == DO_LONGLONG) {
                        uint64_t value;
                        text += ALmixer_ScanUnsignedLongLong(text, radix, &value);
                        if (!suppress) {
                            uint64_t *valuep = va_arg(ap, uint64_t *);
                            *valuep = value;
                            ++retval;
                        }
                    } else {
                        unsigned long value;
                        text += ALmixer_ScanUnsignedLong(text, radix, &value);
                        if (!suppress) {
                            switch (inttype) {
                            case DO_SHORT:
                                {
                                    short *valuep = va_arg(ap, short *);
                                    *valuep = (short) value;
                                }
                                break;
                            case DO_INT:
                                {
                                    int *valuep = va_arg(ap, int *);
                                    *valuep = (int) value;
                                }
                                break;
                            case DO_LONG:
                                {
                                    long *valuep = va_arg(ap, long *);
                                    *valuep = value;
                                }
                                break;
                            case DO_LONGLONG:
                                /* Handled above */
                                break;
                            }
                            ++retval;
                        }
                    }
                    done = ALMIXER_TRUE;
                    break;
                case 'p':
                    {
                        uintptr_t value;
                        text += ALmixer_ScanUintPtrT(text, 16, &value);
                        if (!suppress) {
                            void **valuep = va_arg(ap, void **);
                            *valuep = (void *) value;
                            ++retval;
                        }
                    }
                    done = ALMIXER_TRUE;
                    break;
                case 'f':
                    {
                        double value;
                        text += ALmixer_ScanFloat(text, &value);
                        if (!suppress) {
                            float *valuep = va_arg(ap, float *);
                            *valuep = (float) value;
                            ++retval;
                        }
                    }
                    done = ALMIXER_TRUE;
                    break;
                case 's':
                    if (suppress) {
                        while (!ALmixer_isspace((unsigned char) *text)) {
                            ++text;
                            if (count) {
                                if (--count == 0) {
                                    break;
                                }
                            }
                        }
                    } else {
                        char *valuep = va_arg(ap, char *);
                        while (!ALmixer_isspace((unsigned char) *text)) {
                            *valuep++ = *text++;
                            if (count) {
                                if (--count == 0) {
                                    break;
                                }
                            }
                        }
                        *valuep = '\0';
                        ++retval;
                    }
                    done = ALMIXER_TRUE;
                    break;
                default:
                    done = ALMIXER_TRUE;
                    break;
                }
                ++fmt;
            }
            continue;
        }
        if (*text == *fmt) {
            ++text;
            ++fmt;
            continue;
        }
        /* Text didn't match format specifier */
        break;
    }

    return retval;
}
#endif /* HAVE_VSSCANF */
#endif /* #if 0 */

int
ALmixer_snprintf(char *text, size_t maxlen, const char *fmt, ...)
{
    va_list ap;
    int retval;

    va_start(ap, fmt);
    retval = ALmixer_vsnprintf(text, maxlen, fmt, ap);
    va_end(ap);

    return retval;
}

#ifdef HAVE_VSNPRINTF
int ALmixer_vsnprintf(char *text, size_t maxlen, const char *fmt, va_list ap)
{
    return vsnprintf(text, maxlen, fmt, ap);
}
#else
 /* FIXME: implement more of the format specifiers */
typedef enum
{
    ALmixer_CASE_NOCHANGE,
    ALmixer_CASE_LOWER,
    ALmixer_CASE_UPPER
} ALmixer_letter_case;

typedef struct
{
    ALmixer_bool left_justify;
    ALmixer_bool force_sign;
    ALmixer_bool force_type;
    ALmixer_bool pad_zeroes;
    ALmixer_letter_case force_case;
    int width;
    int radix;
    int precision;
} ALmixer_FormatInfo;

static size_t
ALmixer_PrintString(char *text, size_t maxlen, ALmixer_FormatInfo *info, const char *string)
{
    size_t length = 0;

    if (info && info->width && (size_t)info->width > ALmixer_strlen(string)) {
        char fill = info->pad_zeroes ? '0' : ' ';
        size_t width = info->width - ALmixer_strlen(string);
        while (width-- > 0 && maxlen > 0) {
            *text++ = fill;
            ++length;
            --maxlen;
        }
    }

    length += ALmixer_strlcpy(text, string, maxlen);

    if (info) {
        if (info->force_case == ALmixer_CASE_LOWER) {
            ALmixer_strlwr(text);
        } else if (info->force_case == ALmixer_CASE_UPPER) {
            ALmixer_strupr(text);
        }
    }
    return length;
}

static size_t
ALmixer_PrintLong(char *text, size_t maxlen, ALmixer_FormatInfo *info, long value)
{
    char num[130];

    ALmixer_ltoa(value, num, info ? info->radix : 10);
    return ALmixer_PrintString(text, maxlen, info, num);
}

static size_t
ALmixer_PrintUnsignedLong(char *text, size_t maxlen, ALmixer_FormatInfo *info, unsigned long value)
{
    char num[130];

    ALmixer_ultoa(value, num, info ? info->radix : 10);
    return ALmixer_PrintString(text, maxlen, info, num);
}

static size_t
ALmixer_PrintLongLong(char *text, size_t maxlen, ALmixer_FormatInfo *info, int64_t value)
{
    char num[130];

    ALmixer_lltoa(value, num, info ? info->radix : 10);
    return ALmixer_PrintString(text, maxlen, info, num);
}

static size_t
ALmixer_PrintUnsignedLongLong(char *text, size_t maxlen, ALmixer_FormatInfo *info, uint64_t value)
{
    char num[130];

    ALmixer_ulltoa(value, num, info ? info->radix : 10);
    return ALmixer_PrintString(text, maxlen, info, num);
}

static size_t
ALmixer_PrintFloat(char *text, size_t maxlen, ALmixer_FormatInfo *info, double arg)
{
    int width;
    size_t len;
    size_t left = maxlen;
    char *textstart = text;

    if (arg) {
        /* This isn't especially accurate, but hey, it's easy. :) */
        unsigned long value;

        if (arg < 0) {
            if (left > 1) {
                *text = '-';
                --left;
            }
            ++text;
            arg = -arg;
        } else if (info->force_sign) {
            if (left > 1) {
                *text = '+';
                --left;
            }
            ++text;
        }
        value = (unsigned long) arg;
        len = ALmixer_PrintUnsignedLong(text, left, NULL, value);
        text += len;
        if (len >= left) {
            left = ALmixer_min(left, 1);
        } else {
            left -= len;
        }
        arg -= value;
        if (info->precision < 0) {
            info->precision = 6;
        }
        if (info->force_type || info->precision > 0) {
            int mult = 10;
            if (left > 1) {
                *text = '.';
                --left;
            }
            ++text;
            while (info->precision-- > 0) {
                value = (unsigned long) (arg * mult);
                len = ALmixer_PrintUnsignedLong(text, left, NULL, value);
                text += len;
                if (len >= left) {
                    left = ALmixer_min(left, 1);
                } else {
                    left -= len;
                }
                arg -= (double) value / mult;
                mult *= 10;
            }
        }
    } else {
        if (left > 1) {
            *text = '0';
            --left;
        }
        ++text;
        if (info->force_type) {
            if (left > 1) {
                *text = '.';
                --left;
            }
            ++text;
        }
    }

    width = info->width - (int)(text - textstart);
    if (width > 0) {
        char fill = info->pad_zeroes ? '0' : ' ';
        char *end = text+left-1;
        len = (text - textstart);
        for (len = (text - textstart); len--; ) {
            if ((textstart+len+width) < end) {
                *(textstart+len+width) = *(textstart+len);
            }
        }
        len = (size_t)width;
        text += len;
        if (len >= left) {
            left = ALmixer_min(left, 1);
        } else {
            left -= len;
        }
        while (len--) {
            if (textstart+len < end) {
                textstart[len] = fill;
            }
        }
    }

    return (text - textstart);
}

int
ALmixer_vsnprintf(char *text, size_t maxlen, const char *fmt, va_list ap)
{
    size_t left = maxlen;
    char *textstart = text;

    while (*fmt) {
        if (*fmt == '%') {
            ALmixer_bool done = ALMIXER_FALSE;
            size_t len = 0;
            ALmixer_bool check_flag;
            ALmixer_FormatInfo info;
            enum
            {
                DO_INT,
                DO_LONG,
                DO_LONGLONG
            } inttype = DO_INT;

            ALmixer_zero(info);
            info.radix = 10;
            info.precision = -1;

            check_flag = ALMIXER_TRUE;
            while (check_flag) {
                ++fmt;
                switch (*fmt) {
                case '-':
                    info.left_justify = ALMIXER_TRUE;
                    break;
                case '+':
                    info.force_sign = ALMIXER_TRUE;
                    break;
                case '#':
                    info.force_type = ALMIXER_TRUE;
                    break;
                case '0':
                    info.pad_zeroes = ALMIXER_TRUE;
                    break;
                default:
                    check_flag = ALMIXER_FALSE;
                    break;
                }
            }

            if (*fmt >= '0' && *fmt <= '9') {
                info.width = ALmixer_strtol(fmt, (char **)&fmt, 0);
            }

            if (*fmt == '.') {
                ++fmt;
                if (*fmt >= '0' && *fmt <= '9') {
                    info.precision = ALmixer_strtol(fmt, (char **)&fmt, 0);
                } else {
                    info.precision = 0;
                }
            }

            while (!done) {
                switch (*fmt) {
                case '%':
                    if (left > 1) {
                        *text = '%';
                    }
                    len = 1;
                    done = ALMIXER_TRUE;
                    break;
                case 'c':
                    /* char is promoted to int when passed through (...) */
                    if (left > 1) {
                        *text = (char) va_arg(ap, int);
                    }
                    len = 1;
                    done = ALMIXER_TRUE;
                    break;
                case 'h':
                    /* short is promoted to int when passed through (...) */
                    break;
                case 'l':
                    if (inttype < DO_LONGLONG) {
                        ++inttype;
                    }
                    break;
                case 'I':
                    if (ALmixer_strncmp(fmt, "I64", 3) == 0) {
                        fmt += 2;
                        inttype = DO_LONGLONG;
                    }
                    break;
                case 'i':
                case 'd':
                    switch (inttype) {
                    case DO_INT:
                        len = ALmixer_PrintLong(text, left, &info,
                                            (long) va_arg(ap, int));
                        break;
                    case DO_LONG:
                        len = ALmixer_PrintLong(text, left, &info,
                                            va_arg(ap, long));
                        break;
                    case DO_LONGLONG:
                        len = ALmixer_PrintLongLong(text, left, &info,
                                                va_arg(ap, int64_t));
                        break;
                    }
                    done = ALMIXER_TRUE;
                    break;
                case 'p':
                case 'x':
                    info.force_case = ALmixer_CASE_LOWER;
                    /* Fall through to 'X' handling */
                case 'X':
                    if (info.force_case == ALmixer_CASE_NOCHANGE) {
                        info.force_case = ALmixer_CASE_UPPER;
                    }
                    if (info.radix == 10) {
                        info.radix = 16;
                    }
                    if (*fmt == 'p') {
                        inttype = DO_LONG;
                    }
                    /* Fall through to unsigned handling */
                case 'o':
                    if (info.radix == 10) {
                        info.radix = 8;
                    }
                    /* Fall through to unsigned handling */
                case 'u':
                    info.pad_zeroes = ALMIXER_TRUE;
                    switch (inttype) {
                    case DO_INT:
                        len = ALmixer_PrintUnsignedLong(text, left, &info,
                                                    (unsigned long)
                                                    va_arg(ap, unsigned int));
                        break;
                    case DO_LONG:
                        len = ALmixer_PrintUnsignedLong(text, left, &info,
                                                    va_arg(ap, unsigned long));
                        break;
                    case DO_LONGLONG:
                        len = ALmixer_PrintUnsignedLongLong(text, left, &info,
                                                        va_arg(ap, uint64_t));
                        break;
                    }
                    done = ALMIXER_TRUE;
                    break;
                case 'f':
                    len = ALmixer_PrintFloat(text, left, &info, va_arg(ap, double));
                    done = ALMIXER_TRUE;
                    break;
                case 's':
                    len = ALmixer_PrintString(text, left, &info, va_arg(ap, char *));
                    done = ALMIXER_TRUE;
                    break;
                default:
                    done = ALMIXER_TRUE;
                    break;
                }
                ++fmt;
            }
            text += len;
            if (len >= left) {
                left = ALmixer_min(left, 1);
            } else {
                left -= len;
            }
        } else {
            if (left > 1) {
                *text = *fmt;
                --left;
            }
            ++fmt;
            ++text;
        }
    }
    if (left > 0) {
        *text = '\0';
    }
    return (int)(text - textstart);
}
#endif /* HAVE_VSNPRINTF */

/* vi: set ts=4 sw=4 expandtab: */
