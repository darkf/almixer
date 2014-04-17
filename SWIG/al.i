%module OpenAL

%{
#include "al.h"
#include "alc.h"
#ifdef __APPLE__
	#include <TargetConditionals.h>
	#if (TARGET_OS_IPHONE == 1) || (TARGET_IPHONE_SIMULATOR == 1)
		#include "oalMacOSX_OALExtensions.h"
	#else
		#include "MacOSX_OALExtensions.h"
	#endif
#endif
%}


/* The Javascript backend has not implemented typemaps.i, so we do things the hard way. */
#ifdef SWIG_JAVASCRIPT_JSC
	%include "al_javascript_jscore.i"
#elif SWIG_JAVASCRIPT_V8
	%include "al_javascript_v8.i"

#else
	%warn "al.i not fully implemented for target language"
	/* typemaps.i can be used to solve most problems. */
#endif

/* 
	Problem: It is dangerous to redefine #defines, typedefs manually instead of %include "al.h" 
	because it is not guaranteed that different OpenAL implementations use the same values.
	But if use %include "al.h", then I must override values.
	This can lead to SWIG generation warnings, for example, I need to override ALboolean to bool from char.
	I must declare it myself, before I %include "al.h", with my overridden type. I do not know how to get SWIG to ignore it.
	As an alternative, I tried: %apply ALboolean { bool };
	before and after, but it didn't make a difference.
*/


/** For SWIG, change char to bool for the ALboolean type. */
typedef bool ALboolean;

%include "al.h"
/* Warning: alc not tested */
%include "alc.h"

#ifdef __APPLE__
    /* Warning: not tested */
    %include "oalMacOSX_OALExtensions.h"
#endif
