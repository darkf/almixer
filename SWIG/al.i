%module OpenAL

%{
#include "al.h"
%}


#ifdef SWIG_JAVASCRIPT_JSC

// alSourcefv
%typemap(in) const ALfloat* alsourcefv_values (int input_array_length = 0, JSObjectRef input_js_array, JSValueRef current_array_jsvalue, int current_array_element, ALfloat target_array[3])
{
	/* for alSourcefv, the usable array length is no more than 3 */
	if(JSValueIsObject(context, $input))
	{
		// Convert into Array
		input_js_array = JSValueToObject(context, $input, NULL);

		input_array_length = (int)SWIG_JSObjectGetNumber(context, SWIG_JSObjectGetNamedProperty(context, input_js_array, "length", NULL), NULL);

		/* Make sure the array length doesn't exceed 3 */
		if(input_array_length > 3)
		{
			input_array_length = 3;
		}
		/* Make sure there is at least one element in the array */
		if(input_array_length < 1)
		{
			SWIG_exception_fail(SWIG_ERROR, "$input array is empty");
		}

		/* Get each element from array */
		for(current_array_element=0; current_array_element<input_array_length; current_array_element++)
		{
			current_array_jsvalue = JSObjectGetPropertyAtIndex(context, input_js_array, current_array_element, NULL);

			if(JSValueIsNumber(context, current_array_jsvalue))
			{
				target_array[current_array_element] = JSValueIsNumber(context, current_array_jsvalue);
			}
			else
			{
				SWIG_exception_fail(SWIG_TypeError, "Not a number type in array for ALfloat* values");
			}
		}

		$1 = target_array;
	}
	else
	{
		SWIG_exception_fail(SWIG_ERROR, "$input is not JSObjectRef");
	}
}

// alSourceiv
%typemap(in) const ALint* alsourceiv_values (int input_array_length = 0, JSObjectRef input_js_array, JSValueRef current_array_jsvalue, int current_array_element, ALint target_array[3])
{
	/* for alSourceiv, the usable array length is no more than 3 */
	if(JSValueIsObject(context, $input))
	{
		// Convert into Array
		input_js_array = JSValueToObject(context, $input, NULL);

		input_array_length = (int)SWIG_JSObjectGetNumber(context, SWIG_JSObjectGetNamedProperty(context, input_js_array, "length", NULL), NULL);

		/* Make sure the array length doesn't exceed 3 */
		if(input_array_length > 3)
		{
			input_array_length = 3;
		}
		/* Make sure there is at least one element in the array */
		if(input_array_length < 1)
		{
			SWIG_exception_fail(SWIG_ERROR, "$input array is empty");
		}

		/* Get each element from array */
		for(current_array_element=0; current_array_element<input_array_length; current_array_element++)
		{
			current_array_jsvalue = JSObjectGetPropertyAtIndex(context, input_js_array, current_array_element, NULL);

			if(JSValueIsNumber(context, current_array_jsvalue))
			{
				target_array[current_array_element] = JSValueIsNumber(context, current_array_jsvalue);
			}
			else
			{
				SWIG_exception_fail(SWIG_TypeError, "Not a number type in array for ALfloat* values");
			}
		}

		$1 = target_array;
	}
	else
	{
		SWIG_exception_fail(SWIG_ERROR, "$input is not JSObjectRef");
	}
}

// alGetSourcef, alGetSource3f
/* Set the input argument to point to a temporary variable */
%typemap(in, numinputs=1) ALfloat* algetsourcef_value (ALfloat internal_outvalue_storage)
{
   $1 = &internal_outvalue_storage;
}
%typemap(argout) ALfloat* algetsourcef_value (JSObjectRef input_js_array)
{
	if(JSValueIsObject(context, $input))
	{
		input_js_array = JSValueToObject(context, $input, NULL);
		/* The -3 is needed because in the C APIs that use this typemap, the ALfloat* starts at the 3rd parameter, so $argnum-3 will allow the array elements to start at 0. */
		JSObjectSetPropertyAtIndex(context, input_js_array, $argnum-3, JSValueMakeNumber(context, *$1), NULL);
	}
	else
	{
		SWIG_exception_fail(SWIG_TypeError, "Must provide a Javascript array for the ALfloat* value");
	}
}

// alGetSourcefv
/* Set the input argument to point to a temporary variable */
/* alGetSourcefv has at most 3 elements */
%typemap(in, numinputs=1) ALfloat* algetsourcefv_values (ALfloat internal_outvalue_storage[3])
{
   $1 = &internal_outvalue_storage[0];
}
%typemap(argout) ALfloat* algetsourcefv_values (JSObjectRef input_js_array, int current_array_element)
{
	if(JSValueIsObject(context, $input))
	{
		input_js_array = JSValueToObject(context, $input, NULL);
		/* Set each element from array */
		for(current_array_element=0; current_array_element<3; current_array_element++)
		{
			JSObjectSetPropertyAtIndex(context, input_js_array, current_array_element, JSValueMakeNumber(context, $1[current_array_element]), NULL);
		}
	}
	else
	{
		SWIG_exception_fail(SWIG_TypeError, "Must provide a Javascript array for the ALfloat* value");
	}
}


// alListenerfv
%typemap(in) const ALfloat* allistenerfv_values (int input_array_length = 0, JSObjectRef input_js_array, JSValueRef current_array_jsvalue, int current_array_element, ALfloat target_array[6])
{
	/* for alListenerfv, the usable array length is no more than 6 */
	if(JSValueIsObject(context, $input))
	{
		// Convert into Array
		input_js_array = JSValueToObject(context, $input, NULL);

		input_array_length = (int)SWIG_JSObjectGetNumber(context, SWIG_JSObjectGetNamedProperty(context, input_js_array, "length", NULL), NULL);

		/* Make sure the array length doesn't exceed 3 */
		if(input_array_length > 6)
		{
			input_array_length = 6;
		}
		/* Make sure there is at least one element in the array */
		if(input_array_length < 1)
		{
			SWIG_exception_fail(SWIG_ERROR, "$input array is empty");
		}

		/* Get each element from array */
		for(current_array_element=0; current_array_element<input_array_length; current_array_element++)
		{
			current_array_jsvalue = JSObjectGetPropertyAtIndex(context, input_js_array, current_array_element, NULL);

			if(JSValueIsNumber(context, current_array_jsvalue))
			{
				target_array[current_array_element] = JSValueIsNumber(context, current_array_jsvalue);
			}
			else
			{
				SWIG_exception_fail(SWIG_TypeError, "Not a number type in array for ALfloat* values");
			}
		}

		$1 = target_array;
	}
	else
	{
		SWIG_exception_fail(SWIG_ERROR, "$input is not JSObjectRef");
	}
}

// alListeneriv
%typemap(in) const ALint* allisteneriv_values (int input_array_length = 0, JSObjectRef input_js_array, JSValueRef current_array_jsvalue, int current_array_element, ALint target_array[6])
{
	/* for alListenerfv, the usable array length is no more than 6 */
	if(JSValueIsObject(context, $input))
	{
		// Convert into Array
		input_js_array = JSValueToObject(context, $input, NULL);

		input_array_length = (int)SWIG_JSObjectGetNumber(context, SWIG_JSObjectGetNamedProperty(context, input_js_array, "length", NULL), NULL);

		/* Make sure the array length doesn't exceed 3 */
		if(input_array_length > 6)
		{
			input_array_length = 6;
		}
		/* Make sure there is at least one element in the array */
		if(input_array_length < 1)
		{
			SWIG_exception_fail(SWIG_ERROR, "$input array is empty");
		}

		/* Get each element from array */
		for(current_array_element=0; current_array_element<input_array_length; current_array_element++)
		{
			current_array_jsvalue = JSObjectGetPropertyAtIndex(context, input_js_array, current_array_element, NULL);

			if(JSValueIsNumber(context, current_array_jsvalue))
			{
				target_array[current_array_element] = JSValueIsNumber(context, current_array_jsvalue);
			}
			else
			{
				SWIG_exception_fail(SWIG_TypeError, "Not a number type in array for ALfloat* values");
			}
		}

		$1 = target_array;
	}
	else
	{
		SWIG_exception_fail(SWIG_ERROR, "$input is not JSObjectRef");
	}
}

#endif /* SWIG_JAVASCRIPT_JSC */

/* Problem: It is dangerous to redefine #defines, typedefs manually instead of %include "al.h" because it is not guaranteed that different OpenAL implementations use the same values.
	But if we %include "al.h", typemaps get tricky because I don't want the exact same typemap applied globally to all OpenAL functions.
	For example, I don't want the same treatment for ALfloat* applied to:
	alGetSourcef(ALuint, ALenum, ALfloat*) and alGetSourcefv(ALuint, ALenum, ALfloat*) and alGetBufferf/v, and alGetListenerf/v
	
	Additionally, it is possible for different OpenAL implementations to use different parameter variable names, so it is hard to rely on that.

	Solution: I am going to predeclare those functions I need to use typemaps here with their own unique variable names and hope SWIG doesn't complain about multiple declarations.
*/
void alSourcefv( ALuint sid, ALenum param, const ALfloat* alsourcefv_values );
void alSourceiv( ALuint sid, ALenum param, const ALint* alsourceiv_values );
void alGetSourcef( ALuint sid, ALenum param, ALfloat* algetsourcef_value );
/* Yes, all 3 parameters are named the same thing intentionally to get the ALfloat* algetsourcef_value to apply to all parameters. */
void alGetSource3f( ALuint sid, ALenum param, ALfloat* algetsourcef_value, ALfloat* algetsourcef_value, ALfloat* algetsourcef_value);
void alGetSourcefv( ALuint sid, ALenum param, ALfloat* algetsourcefv_values );

void alListenerfv( ALenum param, const ALfloat* allistenerfv_values );
void alListeneriv( ALenum param, const ALint* allisteneriv_values );




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

