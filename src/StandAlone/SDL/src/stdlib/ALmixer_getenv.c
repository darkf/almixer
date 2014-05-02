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

/* ALmixer: Only Windows needs this file for getenv(). 
 * So to save space, this file is compiled for Windows only. 
 */
#ifdef _WIN32



#include "../ALmixer_internal.h"

#if defined(__WIN32__)
#include "../core/windows/ALmixer_windows.h"
#endif

#include "ALmixer_stdinc.h"

#if defined(__WIN32__) && (!defined(HAVE_SETENV) || !defined(HAVE_GETENV))
/* Note this isn't thread-safe! */
static char *ALmixer_envmem = NULL; /* Ugh, memory leak */
static size_t ALmixer_envmemlen = 0;
#endif

#if 0

/* Put a variable into the environment */
/* Note: Name may not contain a '=' character. (Reference: http://www.unix.com/man-page/Linux/3/setenv/) */
#if defined(HAVE_SETENV)
int
ALmixer_setenv(const char *name, const char *value, int overwrite)
{
    /* Input validation */
    if (!name || ALmixer_strlen(name) == 0 || ALmixer_strchr(name, '=') != NULL || !value) {
        return (-1);
    }
    
    return setenv(name, value, overwrite);
}
#elif defined(__WIN32__)
int
ALmixer_setenv(const char *name, const char *value, int overwrite)
{
    /* Input validation */
    if (!name || ALmixer_strlen(name) == 0 || ALmixer_strchr(name, '=') != NULL || !value) {
        return (-1);
    }
    
    if (!overwrite) {
        char ch = 0;
        const size_t len = GetEnvironmentVariableA(name, &ch, sizeof (ch));
        if (len > 0) {
            return 0;  /* asked not to overwrite existing value. */
        }
    }
    if (!SetEnvironmentVariableA(name, *value ? value : NULL)) {
        return -1;
    }
    return 0;
}
/* We have a real environment table, but no real setenv? Fake it w/ putenv. */
#elif (defined(HAVE_GETENV) && defined(HAVE_PUTENV) && !defined(HAVE_SETENV))
int
ALmixer_setenv(const char *name, const char *value, int overwrite)
{
    size_t len;
    char *new_variable;

    /* Input validation */
    if (!name || ALmixer_strlen(name) == 0 || ALmixer_strchr(name, '=') != NULL || !value) {
        return (-1);
    }
    
    if (getenv(name) != NULL) {
        if (overwrite) {
            unsetenv(name);
        } else {
            return 0;  /* leave the existing one there. */
        }
    }

    /* This leaks. Sorry. Get a better OS so we don't have to do this. */
    len = ALmixer_strlen(name) + ALmixer_strlen(value) + 2;
    new_variable = (char *) ALmixer_malloc(len);
    if (!new_variable) {
        return (-1);
    }

    ALmixer_snprintf(new_variable, len, "%s=%s", name, value);
    return putenv(new_variable);
}
#else /* roll our own */
static char **ALmixer_env = (char **) 0;
int
ALmixer_setenv(const char *name, const char *value, int overwrite)
{
    int added;
    int len, i;
    char **new_env;
    char *new_variable;

    /* Input validation */
    if (!name || ALmixer_strlen(name) == 0 || ALmixer_strchr(name, '=') != NULL || !value) {
        return (-1);
    }

    /* See if it already exists */
    if (!overwrite && ALmixer_getenv(name)) {
        return 0;
    }

    /* Allocate memory for the variable */
    len = ALmixer_strlen(name) + ALmixer_strlen(value) + 2;
    new_variable = (char *) ALmixer_malloc(len);
    if (!new_variable) {
        return (-1);
    }

    ALmixer_snprintf(new_variable, len, "%s=%s", name, value);
    value = new_variable + ALmixer_strlen(name) + 1;
    name = new_variable;

    /* Actually put it into the environment */
    added = 0;
    i = 0;
    if (ALmixer_env) {
        /* Check to see if it's already there... */
        len = (value - name);
        for (; ALmixer_env[i]; ++i) {
            if (ALmixer_strncmp(ALmixer_env[i], name, len) == 0) {
                break;
            }
        }
        /* If we found it, just replace the entry */
        if (ALmixer_env[i]) {
            ALmixer_free(ALmixer_env[i]);
            ALmixer_env[i] = new_variable;
            added = 1;
        }
    }

    /* Didn't find it in the environment, expand and add */
    if (!added) {
        new_env = ALmixer_realloc(ALmixer_env, (i + 2) * sizeof(char *));
        if (new_env) {
            ALmixer_env = new_env;
            ALmixer_env[i++] = new_variable;
            ALmixer_env[i++] = (char *) 0;
            added = 1;
        } else {
            ALmixer_free(new_variable);
        }
    }
    return (added ? 0 : -1);
}
#endif

#endif /* #if 0 */

/* Retrieve a variable named "name" from the environment */
#if defined(HAVE_GETENV)
char *
ALmixer_getenv(const char *name)
{
    /* Input validation */
    if (!name || ALmixer_strlen(name)==0) {
        return NULL;
    }

    return getenv(name);
}
#elif defined(__WIN32__)
char *
ALmixer_getenv(const char *name)
{
    size_t bufferlen;

    /* Input validation */
    if (!name || ALmixer_strlen(name)==0) {
        return NULL;
    }
    
    bufferlen =
        GetEnvironmentVariableA(name, ALmixer_envmem, (DWORD) ALmixer_envmemlen);
    if (bufferlen == 0) {
        return NULL;
    }
    if (bufferlen > ALmixer_envmemlen) {
        char *newmem = (char *) ALmixer_realloc(ALmixer_envmem, bufferlen);
        if (newmem == NULL) {
            return NULL;
        }
        ALmixer_envmem = newmem;
        ALmixer_envmemlen = bufferlen;
        GetEnvironmentVariableA(name, ALmixer_envmem, (DWORD) ALmixer_envmemlen);
    }
    return ALmixer_envmem;
}
#else
char *
ALmixer_getenv(const char *name)
{
    int len, i;
    char *value;

    /* Input validation */
    if (!name || ALmixer_strlen(name)==0) {
        return NULL;
    }
    
    value = (char *) 0;
    if (ALmixer_env) {
        len = ALmixer_strlen(name);
        for (i = 0; ALmixer_env[i] && !value; ++i) {
            if ((ALmixer_strncmp(ALmixer_env[i], name, len) == 0) &&
                (ALmixer_env[i][len] == '=')) {
                value = &ALmixer_env[i][len + 1];
            }
        }
    }
    return value;
}
#endif


#ifdef TEST_MAIN
#include <stdio.h>

int
main(int argc, char *argv[])
{
    char *value;

    printf("Checking for non-existent variable... ");
    fflush(stdout);
    if (!ALmixer_getenv("EXISTS")) {
        printf("okay\n");
    } else {
        printf("failed\n");
    }
    printf("Setting FIRST=VALUE1 in the environment... ");
    fflush(stdout);
    if (ALmixer_setenv("FIRST", "VALUE1", 0) == 0) {
        printf("okay\n");
    } else {
        printf("failed\n");
    }
    printf("Getting FIRST from the environment... ");
    fflush(stdout);
    value = ALmixer_getenv("FIRST");
    if (value && (ALmixer_strcmp(value, "VALUE1") == 0)) {
        printf("okay\n");
    } else {
        printf("failed\n");
    }
    printf("Setting SECOND=VALUE2 in the environment... ");
    fflush(stdout);
    if (ALmixer_setenv("SECOND", "VALUE2", 0) == 0) {
        printf("okay\n");
    } else {
        printf("failed\n");
    }
    printf("Getting SECOND from the environment... ");
    fflush(stdout);
    value = ALmixer_getenv("SECOND");
    if (value && (ALmixer_strcmp(value, "VALUE2") == 0)) {
        printf("okay\n");
    } else {
        printf("failed\n");
    }
    printf("Setting FIRST=NOVALUE in the environment... ");
    fflush(stdout);
    if (ALmixer_setenv("FIRST", "NOVALUE", 1) == 0) {
        printf("okay\n");
    } else {
        printf("failed\n");
    }
    printf("Getting FIRST from the environment... ");
    fflush(stdout);
    value = ALmixer_getenv("FIRST");
    if (value && (ALmixer_strcmp(value, "NOVALUE") == 0)) {
        printf("okay\n");
    } else {
        printf("failed\n");
    }
    printf("Checking for non-existent variable... ");
    fflush(stdout);
    if (!ALmixer_getenv("EXISTS")) {
        printf("okay\n");
    } else {
        printf("failed\n");
    }
    return (0);
}
#endif /* TEST_MAIN */

#endif /* #ifdef _WIN32 */

/* vi: set ts=4 sw=4 expandtab: */
