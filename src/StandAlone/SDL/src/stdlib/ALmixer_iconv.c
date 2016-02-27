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

/* ALmixer: Only Windows needs this file.
 * Since this is kind of big, I'm making it Windows only for now.
 */
#ifdef _WIN32

#include "../ALmixer_internal.h"

/* This file contains portable iconv functions for SDL */

#include "ALmixer_stdinc.h"
#include "ALmixer_endian.h"

/* Needed for ALmixer_bool */
#include "ALmixer_RWops.h"

/* Needed because I'm missing a header declaration for this */
extern char * ALmixer_getenv(const char *name);

#ifdef HAVE_ICONV

/* Depending on which standard the iconv() was implemented with,
   iconv() may or may not use const char ** for the inbuf param.
   If we get this wrong, it's just a warning, so no big deal.
*/
#if defined(_XGP6) || defined(__APPLE__) || \
    (defined(__GLIBC__) && ((__GLIBC__ > 2) || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 2)))
#define ICONV_INBUF_NONCONST
#endif

#include <errno.h>

ALmixer_COMPILE_TIME_ASSERT(iconv_t, sizeof (iconv_t) <= sizeof (ALmixer_iconv_t));

ALmixer_iconv_t
ALmixer_iconv_open(const char *tocode, const char *fromcode)
{
    return (ALmixer_iconv_t) ((size_t) iconv_open(tocode, fromcode));
}

int
ALmixer_iconv_close(ALmixer_iconv_t cd)
{
    return iconv_close((iconv_t) ((size_t) cd));
}

size_t
ALmixer_iconv(ALmixer_iconv_t cd,
          const char **inbuf, size_t * inbytesleft,
          char **outbuf, size_t * outbytesleft)
{
    size_t retCode;
#ifdef ICONV_INBUF_NONCONST
    retCode = iconv((iconv_t) ((size_t) cd), (char **) inbuf, inbytesleft, outbuf, outbytesleft);
#else
    retCode = iconv((iconv_t) ((size_t) cd), inbuf, inbytesleft, outbuf, outbytesleft);
#endif
    if (retCode == (size_t) - 1) {
        switch (errno) {
        case E2BIG:
            return ALmixer_ICONV_E2BIG;
        case EILSEQ:
            return ALmixer_ICONV_EILSEQ;
        case EINVAL:
            return ALmixer_ICONV_EINVAL;
        default:
            return ALmixer_ICONV_ERROR;
        }
    }
    return retCode;
}

#else

/* Lots of useful information on Unicode at:
	http://www.cl.cam.ac.uk/~mgk25/unicode.html
*/

#define UNICODE_BOM	0xFEFF

#define UNKNOWN_ASCII	'?'
#define UNKNOWN_UNICODE	0xFFFD

enum
{
    ENCODING_UNKNOWN,
    ENCODING_ASCII,
    ENCODING_LATIN1,
    ENCODING_UTF8,
    ENCODING_UTF16,             /* Needs byte order marker */
    ENCODING_UTF16BE,
    ENCODING_UTF16LE,
    ENCODING_UTF32,             /* Needs byte order marker */
    ENCODING_UTF32BE,
    ENCODING_UTF32LE,
    ENCODING_UCS2BE,
    ENCODING_UCS2LE,
    ENCODING_UCS4BE,
    ENCODING_UCS4LE,
};
#if ALmixer_BYTEORDER == ALmixer_BIG_ENDIAN
#define ENCODING_UTF16NATIVE	ENCODING_UTF16BE
#define ENCODING_UTF32NATIVE	ENCODING_UTF32BE
#define ENCODING_UCS2NATIVE     ENCODING_UCS2BE
#define ENCODING_UCS4NATIVE     ENCODING_UCS4BE
#else
#define ENCODING_UTF16NATIVE	ENCODING_UTF16LE
#define ENCODING_UTF32NATIVE	ENCODING_UTF32LE
#define ENCODING_UCS2NATIVE     ENCODING_UCS2LE
#define ENCODING_UCS4NATIVE     ENCODING_UCS4LE
#endif

struct _ALmixer_iconv_t
{
    int src_fmt;
    int dst_fmt;
};

static struct
{
    const char *name;
    int format;
} encodings[] = {
/* *INDENT-OFF* */
    { "ASCII", ENCODING_ASCII },
    { "US-ASCII", ENCODING_ASCII },
    { "8859-1", ENCODING_LATIN1 },
    { "ISO-8859-1", ENCODING_LATIN1 },
    { "UTF8", ENCODING_UTF8 },
    { "UTF-8", ENCODING_UTF8 },
    { "UTF16", ENCODING_UTF16 },
    { "UTF-16", ENCODING_UTF16 },
    { "UTF16BE", ENCODING_UTF16BE },
    { "UTF-16BE", ENCODING_UTF16BE },
    { "UTF16LE", ENCODING_UTF16LE },
    { "UTF-16LE", ENCODING_UTF16LE },
    { "UTF32", ENCODING_UTF32 },
    { "UTF-32", ENCODING_UTF32 },
    { "UTF32BE", ENCODING_UTF32BE },
    { "UTF-32BE", ENCODING_UTF32BE },
    { "UTF32LE", ENCODING_UTF32LE },
    { "UTF-32LE", ENCODING_UTF32LE },
    { "UCS2", ENCODING_UCS2BE },
    { "UCS-2", ENCODING_UCS2BE },
    { "UCS-2LE", ENCODING_UCS2LE },
    { "UCS-2BE", ENCODING_UCS2BE },
    { "UCS-2-INTERNAL", ENCODING_UCS2NATIVE },
    { "UCS4", ENCODING_UCS4BE },
    { "UCS-4", ENCODING_UCS4BE },
    { "UCS-4LE", ENCODING_UCS4LE },
    { "UCS-4BE", ENCODING_UCS4BE },
    { "UCS-4-INTERNAL", ENCODING_UCS4NATIVE },
/* *INDENT-ON* */
};

static const char *
getlocale(char *buffer, size_t bufsize)
{
    const char *lang;
    char *ptr;

    lang = ALmixer_getenv("LC_ALL");
    if (!lang) {
        lang = ALmixer_getenv("LC_CTYPE");
    }
    if (!lang) {
        lang = ALmixer_getenv("LC_MESSAGES");
    }
    if (!lang) {
        lang = ALmixer_getenv("LANG");
    }
    if (!lang || !*lang || ALmixer_strcmp(lang, "C") == 0) {
        lang = "ASCII";
    }

    /* We need to trim down strings like "en_US.UTF-8@blah" to "UTF-8" */
    ptr = ALmixer_strchr(lang, '.');
    if (ptr != NULL) {
        lang = ptr + 1;
    }

    ALmixer_strlcpy(buffer, lang, bufsize);
    ptr = ALmixer_strchr(buffer, '@');
    if (ptr != NULL) {
        *ptr = '\0';            /* chop end of string. */
    }

    return buffer;
}

ALmixer_iconv_t
ALmixer_iconv_open(const char *tocode, const char *fromcode)
{
    int src_fmt = ENCODING_UNKNOWN;
    int dst_fmt = ENCODING_UNKNOWN;
    int i;
    char fromcode_buffer[64];
    char tocode_buffer[64];

    if (!fromcode || !*fromcode) {
        fromcode = getlocale(fromcode_buffer, sizeof(fromcode_buffer));
    }
    if (!tocode || !*tocode) {
        tocode = getlocale(tocode_buffer, sizeof(tocode_buffer));
    }
    for (i = 0; i < ALmixer_arraysize(encodings); ++i) {
        if (ALmixer_strcasecmp(fromcode, encodings[i].name) == 0) {
            src_fmt = encodings[i].format;
            if (dst_fmt != ENCODING_UNKNOWN) {
                break;
            }
        }
        if (ALmixer_strcasecmp(tocode, encodings[i].name) == 0) {
            dst_fmt = encodings[i].format;
            if (src_fmt != ENCODING_UNKNOWN) {
                break;
            }
        }
    }
    if (src_fmt != ENCODING_UNKNOWN && dst_fmt != ENCODING_UNKNOWN) {
        ALmixer_iconv_t cd = (ALmixer_iconv_t) ALmixer_malloc(sizeof(*cd));
        if (cd) {
            cd->src_fmt = src_fmt;
            cd->dst_fmt = dst_fmt;
            return cd;
        }
    }
    return (ALmixer_iconv_t) - 1;
}

size_t
ALmixer_iconv(ALmixer_iconv_t cd,
          const char **inbuf, size_t * inbytesleft,
          char **outbuf, size_t * outbytesleft)
{
    /* For simplicity, we'll convert everything to and from UCS-4 */
    const char *src;
    char *dst;
    size_t srclen, dstlen;
    uint32_t ch = 0;
    size_t total;

    if (!inbuf || !*inbuf) {
        /* Reset the context */
        return 0;
    }
    if (!outbuf || !*outbuf || !outbytesleft || !*outbytesleft) {
        return ALmixer_ICONV_E2BIG;
    }
    src = *inbuf;
    srclen = (inbytesleft ? *inbytesleft : 0);
    dst = *outbuf;
    dstlen = *outbytesleft;

    switch (cd->src_fmt) {
    case ENCODING_UTF16:
        /* Scan for a byte order marker */
        {
            uint8_t *p = (uint8_t *) src;
            size_t n = srclen / 2;
            while (n) {
                if (p[0] == 0xFF && p[1] == 0xFE) {
                    cd->src_fmt = ENCODING_UTF16BE;
                    break;
                } else if (p[0] == 0xFE && p[1] == 0xFF) {
                    cd->src_fmt = ENCODING_UTF16LE;
                    break;
                }
                p += 2;
                --n;
            }
            if (n == 0) {
                /* We can't tell, default to host order */
                cd->src_fmt = ENCODING_UTF16NATIVE;
            }
        }
        break;
    case ENCODING_UTF32:
        /* Scan for a byte order marker */
        {
            uint8_t *p = (uint8_t *) src;
            size_t n = srclen / 4;
            while (n) {
                if (p[0] == 0xFF && p[1] == 0xFE &&
                    p[2] == 0x00 && p[3] == 0x00) {
                    cd->src_fmt = ENCODING_UTF32BE;
                    break;
                } else if (p[0] == 0x00 && p[1] == 0x00 &&
                           p[2] == 0xFE && p[3] == 0xFF) {
                    cd->src_fmt = ENCODING_UTF32LE;
                    break;
                }
                p += 4;
                --n;
            }
            if (n == 0) {
                /* We can't tell, default to host order */
                cd->src_fmt = ENCODING_UTF32NATIVE;
            }
        }
        break;
    }

    switch (cd->dst_fmt) {
    case ENCODING_UTF16:
        /* Default to host order, need to add byte order marker */
        if (dstlen < 2) {
            return ALmixer_ICONV_E2BIG;
        }
        *(uint16_t *) dst = UNICODE_BOM;
        dst += 2;
        dstlen -= 2;
        cd->dst_fmt = ENCODING_UTF16NATIVE;
        break;
    case ENCODING_UTF32:
        /* Default to host order, need to add byte order marker */
        if (dstlen < 4) {
            return ALmixer_ICONV_E2BIG;
        }
        *(uint32_t *) dst = UNICODE_BOM;
        dst += 4;
        dstlen -= 4;
        cd->dst_fmt = ENCODING_UTF32NATIVE;
        break;
    }

    total = 0;
    while (srclen > 0) {
        /* Decode a character */
        switch (cd->src_fmt) {
        case ENCODING_ASCII:
            {
                uint8_t *p = (uint8_t *) src;
                ch = (uint32_t) (p[0] & 0x7F);
                ++src;
                --srclen;
            }
            break;
        case ENCODING_LATIN1:
            {
                uint8_t *p = (uint8_t *) src;
                ch = (uint32_t) p[0];
                ++src;
                --srclen;
            }
            break;
        case ENCODING_UTF8:    /* RFC 3629 */
            {
                uint8_t *p = (uint8_t *) src;
                size_t left = 0;
                ALmixer_bool overlong = ALMIXER_FALSE;
                if (p[0] >= 0xFC) {
                    if ((p[0] & 0xFE) != 0xFC) {
                        /* Skip illegal sequences
                           return ALmixer_ICONV_EILSEQ;
                         */
                        ch = UNKNOWN_UNICODE;
                    } else {
                        if (p[0] == 0xFC && srclen > 1 && (p[1] & 0xFC) == 0x80) {
                            overlong = ALMIXER_TRUE;
                        }
                        ch = (uint32_t) (p[0] & 0x01);
                        left = 5;
                    }
                } else if (p[0] >= 0xF8) {
                    if ((p[0] & 0xFC) != 0xF8) {
                        /* Skip illegal sequences
                           return ALmixer_ICONV_EILSEQ;
                         */
                        ch = UNKNOWN_UNICODE;
                    } else {
                        if (p[0] == 0xF8 && srclen > 1 && (p[1] & 0xF8) == 0x80) {
                            overlong = ALMIXER_TRUE;
                        }
                        ch = (uint32_t) (p[0] & 0x03);
                        left = 4;
                    }
                } else if (p[0] >= 0xF0) {
                    if ((p[0] & 0xF8) != 0xF0) {
                        /* Skip illegal sequences
                           return ALmixer_ICONV_EILSEQ;
                         */
                        ch = UNKNOWN_UNICODE;
                    } else {
                        if (p[0] == 0xF0 && srclen > 1 && (p[1] & 0xF0) == 0x80) {
                            overlong = ALMIXER_TRUE;
                        }
                        ch = (uint32_t) (p[0] & 0x07);
                        left = 3;
                    }
                } else if (p[0] >= 0xE0) {
                    if ((p[0] & 0xF0) != 0xE0) {
                        /* Skip illegal sequences
                           return ALmixer_ICONV_EILSEQ;
                         */
                        ch = UNKNOWN_UNICODE;
                    } else {
                        if (p[0] == 0xE0 && srclen > 1 && (p[1] & 0xE0) == 0x80) {
                            overlong = ALMIXER_TRUE;
                        }
                        ch = (uint32_t) (p[0] & 0x0F);
                        left = 2;
                    }
                } else if (p[0] >= 0xC0) {
                    if ((p[0] & 0xE0) != 0xC0) {
                        /* Skip illegal sequences
                           return ALmixer_ICONV_EILSEQ;
                         */
                        ch = UNKNOWN_UNICODE;
                    } else {
                        if ((p[0] & 0xDE) == 0xC0) {
                            overlong = ALMIXER_TRUE;
                        }
                        ch = (uint32_t) (p[0] & 0x1F);
                        left = 1;
                    }
                } else {
                    if ((p[0] & 0x80) != 0x00) {
                        /* Skip illegal sequences
                           return ALmixer_ICONV_EILSEQ;
                         */
                        ch = UNKNOWN_UNICODE;
                    } else {
                        ch = (uint32_t) p[0];
                    }
                }
                ++src;
                --srclen;
                if (srclen < left) {
                    return ALmixer_ICONV_EINVAL;
                }
                while (left--) {
                    ++p;
                    if ((p[0] & 0xC0) != 0x80) {
                        /* Skip illegal sequences
                           return ALmixer_ICONV_EILSEQ;
                         */
                        ch = UNKNOWN_UNICODE;
                        break;
                    }
                    ch <<= 6;
                    ch |= (p[0] & 0x3F);
                    ++src;
                    --srclen;
                }
                if (overlong) {
                    /* Potential security risk
                       return ALmixer_ICONV_EILSEQ;
                     */
                    ch = UNKNOWN_UNICODE;
                }
                if ((ch >= 0xD800 && ch <= 0xDFFF) ||
                    (ch == 0xFFFE || ch == 0xFFFF) || ch > 0x10FFFF) {
                    /* Skip illegal sequences
                       return ALmixer_ICONV_EILSEQ;
                     */
                    ch = UNKNOWN_UNICODE;
                }
            }
            break;
        case ENCODING_UTF16BE: /* RFC 2781 */
            {
                uint8_t *p = (uint8_t *) src;
                uint16_t W1, W2;
                if (srclen < 2) {
                    return ALmixer_ICONV_EINVAL;
                }
                W1 = ((uint16_t) p[0] << 8) | (uint16_t) p[1];
                src += 2;
                srclen -= 2;
                if (W1 < 0xD800 || W1 > 0xDFFF) {
                    ch = (uint32_t) W1;
                    break;
                }
                if (W1 > 0xDBFF) {
                    /* Skip illegal sequences
                       return ALmixer_ICONV_EILSEQ;
                     */
                    ch = UNKNOWN_UNICODE;
                    break;
                }
                if (srclen < 2) {
                    return ALmixer_ICONV_EINVAL;
                }
                p = (uint8_t *) src;
                W2 = ((uint16_t) p[0] << 8) | (uint16_t) p[1];
                src += 2;
                srclen -= 2;
                if (W2 < 0xDC00 || W2 > 0xDFFF) {
                    /* Skip illegal sequences
                       return ALmixer_ICONV_EILSEQ;
                     */
                    ch = UNKNOWN_UNICODE;
                    break;
                }
                ch = (((uint32_t) (W1 & 0x3FF) << 10) |
                      (uint32_t) (W2 & 0x3FF)) + 0x10000;
            }
            break;
        case ENCODING_UTF16LE: /* RFC 2781 */
            {
                uint8_t *p = (uint8_t *) src;
                uint16_t W1, W2;
                if (srclen < 2) {
                    return ALmixer_ICONV_EINVAL;
                }
                W1 = ((uint16_t) p[1] << 8) | (uint16_t) p[0];
                src += 2;
                srclen -= 2;
                if (W1 < 0xD800 || W1 > 0xDFFF) {
                    ch = (uint32_t) W1;
                    break;
                }
                if (W1 > 0xDBFF) {
                    /* Skip illegal sequences
                       return ALmixer_ICONV_EILSEQ;
                     */
                    ch = UNKNOWN_UNICODE;
                    break;
                }
                if (srclen < 2) {
                    return ALmixer_ICONV_EINVAL;
                }
                p = (uint8_t *) src;
                W2 = ((uint16_t) p[1] << 8) | (uint16_t) p[0];
                src += 2;
                srclen -= 2;
                if (W2 < 0xDC00 || W2 > 0xDFFF) {
                    /* Skip illegal sequences
                       return ALmixer_ICONV_EILSEQ;
                     */
                    ch = UNKNOWN_UNICODE;
                    break;
                }
                ch = (((uint32_t) (W1 & 0x3FF) << 10) |
                      (uint32_t) (W2 & 0x3FF)) + 0x10000;
            }
            break;
        case ENCODING_UCS2LE:
            {
                uint8_t *p = (uint8_t *) src;
                if (srclen < 2) {
                    return ALmixer_ICONV_EINVAL;
                }
                ch = ((uint32_t) p[1] << 8) | (uint32_t) p[0];
                src += 2;
                srclen -= 2;
            }
            break;
        case ENCODING_UCS2BE:
            {
                uint8_t *p = (uint8_t *) src;
                if (srclen < 2) {
                    return ALmixer_ICONV_EINVAL;
                }
                ch = ((uint32_t) p[0] << 8) | (uint32_t) p[1];
                src += 2;
                srclen -= 2;
            }
            break;
        case ENCODING_UCS4BE:
        case ENCODING_UTF32BE:
            {
                uint8_t *p = (uint8_t *) src;
                if (srclen < 4) {
                    return ALmixer_ICONV_EINVAL;
                }
                ch = ((uint32_t) p[0] << 24) |
                    ((uint32_t) p[1] << 16) |
                    ((uint32_t) p[2] << 8) | (uint32_t) p[3];
                src += 4;
                srclen -= 4;
            }
            break;
        case ENCODING_UCS4LE:
        case ENCODING_UTF32LE:
            {
                uint8_t *p = (uint8_t *) src;
                if (srclen < 4) {
                    return ALmixer_ICONV_EINVAL;
                }
                ch = ((uint32_t) p[3] << 24) |
                    ((uint32_t) p[2] << 16) |
                    ((uint32_t) p[1] << 8) | (uint32_t) p[0];
                src += 4;
                srclen -= 4;
            }
            break;
        }

        /* Encode a character */
        switch (cd->dst_fmt) {
        case ENCODING_ASCII:
            {
                uint8_t *p = (uint8_t *) dst;
                if (dstlen < 1) {
                    return ALmixer_ICONV_E2BIG;
                }
                if (ch > 0x7F) {
                    *p = UNKNOWN_ASCII;
                } else {
                    *p = (uint8_t) ch;
                }
                ++dst;
                --dstlen;
            }
            break;
        case ENCODING_LATIN1:
            {
                uint8_t *p = (uint8_t *) dst;
                if (dstlen < 1) {
                    return ALmixer_ICONV_E2BIG;
                }
                if (ch > 0xFF) {
                    *p = UNKNOWN_ASCII;
                } else {
                    *p = (uint8_t) ch;
                }
                ++dst;
                --dstlen;
            }
            break;
        case ENCODING_UTF8:    /* RFC 3629 */
            {
                uint8_t *p = (uint8_t *) dst;
                if (ch > 0x10FFFF) {
                    ch = UNKNOWN_UNICODE;
                }
                if (ch <= 0x7F) {
                    if (dstlen < 1) {
                        return ALmixer_ICONV_E2BIG;
                    }
                    *p = (uint8_t) ch;
                    ++dst;
                    --dstlen;
                } else if (ch <= 0x7FF) {
                    if (dstlen < 2) {
                        return ALmixer_ICONV_E2BIG;
                    }
                    p[0] = 0xC0 | (uint8_t) ((ch >> 6) & 0x1F);
                    p[1] = 0x80 | (uint8_t) (ch & 0x3F);
                    dst += 2;
                    dstlen -= 2;
                } else if (ch <= 0xFFFF) {
                    if (dstlen < 3) {
                        return ALmixer_ICONV_E2BIG;
                    }
                    p[0] = 0xE0 | (uint8_t) ((ch >> 12) & 0x0F);
                    p[1] = 0x80 | (uint8_t) ((ch >> 6) & 0x3F);
                    p[2] = 0x80 | (uint8_t) (ch & 0x3F);
                    dst += 3;
                    dstlen -= 3;
                } else if (ch <= 0x1FFFFF) {
                    if (dstlen < 4) {
                        return ALmixer_ICONV_E2BIG;
                    }
                    p[0] = 0xF0 | (uint8_t) ((ch >> 18) & 0x07);
                    p[1] = 0x80 | (uint8_t) ((ch >> 12) & 0x3F);
                    p[2] = 0x80 | (uint8_t) ((ch >> 6) & 0x3F);
                    p[3] = 0x80 | (uint8_t) (ch & 0x3F);
                    dst += 4;
                    dstlen -= 4;
                } else if (ch <= 0x3FFFFFF) {
                    if (dstlen < 5) {
                        return ALmixer_ICONV_E2BIG;
                    }
                    p[0] = 0xF8 | (uint8_t) ((ch >> 24) & 0x03);
                    p[1] = 0x80 | (uint8_t) ((ch >> 18) & 0x3F);
                    p[2] = 0x80 | (uint8_t) ((ch >> 12) & 0x3F);
                    p[3] = 0x80 | (uint8_t) ((ch >> 6) & 0x3F);
                    p[4] = 0x80 | (uint8_t) (ch & 0x3F);
                    dst += 5;
                    dstlen -= 5;
                } else {
                    if (dstlen < 6) {
                        return ALmixer_ICONV_E2BIG;
                    }
                    p[0] = 0xFC | (uint8_t) ((ch >> 30) & 0x01);
                    p[1] = 0x80 | (uint8_t) ((ch >> 24) & 0x3F);
                    p[2] = 0x80 | (uint8_t) ((ch >> 18) & 0x3F);
                    p[3] = 0x80 | (uint8_t) ((ch >> 12) & 0x3F);
                    p[4] = 0x80 | (uint8_t) ((ch >> 6) & 0x3F);
                    p[5] = 0x80 | (uint8_t) (ch & 0x3F);
                    dst += 6;
                    dstlen -= 6;
                }
            }
            break;
        case ENCODING_UTF16BE: /* RFC 2781 */
            {
                uint8_t *p = (uint8_t *) dst;
                if (ch > 0x10FFFF) {
                    ch = UNKNOWN_UNICODE;
                }
                if (ch < 0x10000) {
                    if (dstlen < 2) {
                        return ALmixer_ICONV_E2BIG;
                    }
                    p[0] = (uint8_t) (ch >> 8);
                    p[1] = (uint8_t) ch;
                    dst += 2;
                    dstlen -= 2;
                } else {
                    uint16_t W1, W2;
                    if (dstlen < 4) {
                        return ALmixer_ICONV_E2BIG;
                    }
                    ch = ch - 0x10000;
                    W1 = 0xD800 | (uint16_t) ((ch >> 10) & 0x3FF);
                    W2 = 0xDC00 | (uint16_t) (ch & 0x3FF);
                    p[0] = (uint8_t) (W1 >> 8);
                    p[1] = (uint8_t) W1;
                    p[2] = (uint8_t) (W2 >> 8);
                    p[3] = (uint8_t) W2;
                    dst += 4;
                    dstlen -= 4;
                }
            }
            break;
        case ENCODING_UTF16LE: /* RFC 2781 */
            {
                uint8_t *p = (uint8_t *) dst;
                if (ch > 0x10FFFF) {
                    ch = UNKNOWN_UNICODE;
                }
                if (ch < 0x10000) {
                    if (dstlen < 2) {
                        return ALmixer_ICONV_E2BIG;
                    }
                    p[1] = (uint8_t) (ch >> 8);
                    p[0] = (uint8_t) ch;
                    dst += 2;
                    dstlen -= 2;
                } else {
                    uint16_t W1, W2;
                    if (dstlen < 4) {
                        return ALmixer_ICONV_E2BIG;
                    }
                    ch = ch - 0x10000;
                    W1 = 0xD800 | (uint16_t) ((ch >> 10) & 0x3FF);
                    W2 = 0xDC00 | (uint16_t) (ch & 0x3FF);
                    p[1] = (uint8_t) (W1 >> 8);
                    p[0] = (uint8_t) W1;
                    p[3] = (uint8_t) (W2 >> 8);
                    p[2] = (uint8_t) W2;
                    dst += 4;
                    dstlen -= 4;
                }
            }
            break;
        case ENCODING_UCS2BE:
            {
                uint8_t *p = (uint8_t *) dst;
                if (ch > 0xFFFF) {
                    ch = UNKNOWN_UNICODE;
                }
                if (dstlen < 2) {
                    return ALmixer_ICONV_E2BIG;
                }
                p[0] = (uint8_t) (ch >> 8);
                p[1] = (uint8_t) ch;
                dst += 2;
                dstlen -= 2;
            }
            break;
        case ENCODING_UCS2LE:
            {
                uint8_t *p = (uint8_t *) dst;
                if (ch > 0xFFFF) {
                    ch = UNKNOWN_UNICODE;
                }
                if (dstlen < 2) {
                    return ALmixer_ICONV_E2BIG;
                }
                p[1] = (uint8_t) (ch >> 8);
                p[0] = (uint8_t) ch;
                dst += 2;
                dstlen -= 2;
            }
            break;
        case ENCODING_UTF32BE:
            if (ch > 0x10FFFF) {
                ch = UNKNOWN_UNICODE;
            }
        case ENCODING_UCS4BE:
            if (ch > 0x7FFFFFFF) {
                ch = UNKNOWN_UNICODE;
            }
            {
                uint8_t *p = (uint8_t *) dst;
                if (dstlen < 4) {
                    return ALmixer_ICONV_E2BIG;
                }
                p[0] = (uint8_t) (ch >> 24);
                p[1] = (uint8_t) (ch >> 16);
                p[2] = (uint8_t) (ch >> 8);
                p[3] = (uint8_t) ch;
                dst += 4;
                dstlen -= 4;
            }
            break;
        case ENCODING_UTF32LE:
            if (ch > 0x10FFFF) {
                ch = UNKNOWN_UNICODE;
            }
        case ENCODING_UCS4LE:
            if (ch > 0x7FFFFFFF) {
                ch = UNKNOWN_UNICODE;
            }
            {
                uint8_t *p = (uint8_t *) dst;
                if (dstlen < 4) {
                    return ALmixer_ICONV_E2BIG;
                }
                p[3] = (uint8_t) (ch >> 24);
                p[2] = (uint8_t) (ch >> 16);
                p[1] = (uint8_t) (ch >> 8);
                p[0] = (uint8_t) ch;
                dst += 4;
                dstlen -= 4;
            }
            break;
        }

        /* Update state */
        *inbuf = src;
        *inbytesleft = srclen;
        *outbuf = dst;
        *outbytesleft = dstlen;
        ++total;
    }
    return total;
}

int
ALmixer_iconv_close(ALmixer_iconv_t cd)
{
    if (cd != (ALmixer_iconv_t)-1) {
        ALmixer_free(cd);
    }
    return 0;
}

#endif /* !HAVE_ICONV */

char *
ALmixer_iconv_string(const char *tocode, const char *fromcode, const char *inbuf,
                 size_t inbytesleft)
{
    ALmixer_iconv_t cd;
    char *string;
    size_t stringsize;
    char *outbuf;
    size_t outbytesleft;
    size_t retCode = 0;

    cd = ALmixer_iconv_open(tocode, fromcode);
    if (cd == (ALmixer_iconv_t) - 1) {
        /* See if we can recover here (fixes iconv on Solaris 11) */
        if (!tocode || !*tocode) {
            tocode = "UTF-8";
        }
        if (!fromcode || !*fromcode) {
            fromcode = "UTF-8";
        }
        cd = ALmixer_iconv_open(tocode, fromcode);
    }
    if (cd == (ALmixer_iconv_t) - 1) {
        return NULL;
    }

    stringsize = inbytesleft > 4 ? inbytesleft : 4;
    string = ALmixer_malloc(stringsize);
    if (!string) {
        ALmixer_iconv_close(cd);
        return NULL;
    }
    outbuf = string;
    outbytesleft = stringsize;
    ALmixer_memset(outbuf, 0, 4);

    while (inbytesleft > 0) {
        retCode = ALmixer_iconv(cd, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
        switch (retCode) {
        case ALmixer_ICONV_E2BIG:
            {
                char *oldstring = string;
                stringsize *= 2;
                string = ALmixer_realloc(string, stringsize);
                if (!string) {
                    ALmixer_iconv_close(cd);
                    return NULL;
                }
                outbuf = string + (outbuf - oldstring);
                outbytesleft = stringsize - (outbuf - string);
                ALmixer_memset(outbuf, 0, 4);
            }
            break;
        case ALmixer_ICONV_EILSEQ:
            /* Try skipping some input data - not perfect, but... */
            ++inbuf;
            --inbytesleft;
            break;
        case ALmixer_ICONV_EINVAL:
        case ALmixer_ICONV_ERROR:
            /* We can't continue... */
            inbytesleft = 0;
            break;
        }
    }
    ALmixer_iconv_close(cd);

    return string;
}

#endif /* _WIN32 */
/* vi: set ts=4 sw=4 expandtab: */
