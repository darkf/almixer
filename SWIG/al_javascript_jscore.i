
#ifdef SWIG_JAVASCRIPT_JSC

// alSourcefv
%typemap(in) const ALfloat* alsourcefv_values (int input_array_length = 0, JSObjectRef input_js_array, JSValueRef current_array_jsvalue, int current_array_element, ALfloat target_array[3])
{
	/* for alSourcefv, the usable array length is no more than 3 */
	if(JSValueIsObject(context, $input))
	{
		/* Convert into Array */
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
		/* Convert into Array */
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
				SWIG_exception_fail(SWIG_TypeError, "Not a number type in array for ALint* values");
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
		JSObjectSetPropertyAtIndex(context, input_js_array, 0, JSValueMakeNumber(context, *$1), NULL);
	}
	else
	{
		SWIG_exception_fail(SWIG_TypeError, "Must provide a Javascript array for the ALfloat* value");
	}
}


/*
%typemap(argout) ALfloat* algetsourcef_value
{
    $result doesn't work (SWIG/JS bug)
    jsresult = JSValueMakeNumber(*$1);
}
*/


// alGetSourcei, alGetSource3i
/* Set the input argument to point to a temporary variable */
%typemap(in, numinputs=1) ALint* algetsourcef_value (ALint internal_outvalue_storage)
{
   $1 = &internal_outvalue_storage;
}

%typemap(argout) ALint* algetsourcei_value (JSObjectRef input_js_array)
{
	if(JSValueIsObject(context, $input))
	{
		input_js_array = JSValueToObject(context, $input, NULL);
		JSObjectSetPropertyAtIndex(context, input_js_array, 0, JSValueMakeNumber(context, *$1), NULL);
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



// alGetSourceiv
/* Set the input argument to point to a temporary variable */
/* alGetSourceiv has at most 3 elements */
%typemap(in, numinputs=1) ALint* algetsourceiv_values (ALint internal_outvalue_storage[3])
{
   $1 = &internal_outvalue_storage[0];
}
%typemap(argout) ALint* algetsourceiv_values (JSObjectRef input_js_array, int current_array_element)
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
		/* Convert into Array */
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
		/* Convert into Array */
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
				SWIG_exception_fail(SWIG_TypeError, "Not a number type in array for ALint* values");
			}
		}

		$1 = target_array;
	}
	else
	{
		SWIG_exception_fail(SWIG_ERROR, "$input is not JSObjectRef");
	}
}




// alGetListenerfv
/* Set the input argument to point to a temporary variable */
/* alGetListenerfv has at most 6 elements */
%typemap(in, numinputs=1) ALfloat* algetlistenerfv_values (ALfloat internal_outvalue_storage[6])
{
   $1 = &internal_outvalue_storage[0];
}
%typemap(argout) ALfloat* algetlistenerfv_values (JSObjectRef input_js_array, int current_array_element)
{
	if(JSValueIsObject(context, $input))
	{
		input_js_array = JSValueToObject(context, $input, NULL);
		/* Set each element from array */
		for(current_array_element=0; current_array_element<6; current_array_element++)
		{
			JSObjectSetPropertyAtIndex(context, input_js_array, current_array_element, JSValueMakeNumber(context, $1[current_array_element]), NULL);
		}
	}
	else
	{
		SWIG_exception_fail(SWIG_TypeError, "Must provide a Javascript array for the ALfloat* values");
	}
}



// alGetListeneriv
/* Set the input argument to point to a temporary variable */
/* alGetListeneriv has at most 6 elements */
%typemap(in, numinputs=1) ALint* algetlisteneriv_values (ALint internal_outvalue_storage[6])
{
   $1 = &internal_outvalue_storage[0];
}
%typemap(argout) ALint* algetlisteneriv_values (JSObjectRef input_js_array, int current_array_element)
{
	if(JSValueIsObject(context, $input))
	{
		input_js_array = JSValueToObject(context, $input, NULL);
		/* Set each element from array */
		for(current_array_element=0; current_array_element<6; current_array_element++)
		{
			JSObjectSetPropertyAtIndex(context, input_js_array, current_array_element, JSValueMakeNumber(context, $1[current_array_element]), NULL);
		}
	}
	else
	{
		SWIG_exception_fail(SWIG_TypeError, "Must provide a Javascript array for the ALfloat* value");
	}
}




// alGetBooleanv
/* Set the input argument to point to a temporary variable */
/* alGetBooleanv has at most 3 elements */
%typemap(in, numinputs=1) ALboolean* algetbooleanv_values (ALboolean internal_outvalue_storage[3])
{
   $1 = &internal_outvalue_storage[0];
}
%typemap(argout) ALboolean* algetbooleanv_values (JSObjectRef input_js_array, int current_array_element)
{
	if(JSValueIsObject(context, $input))
	{
		input_js_array = JSValueToObject(context, $input, NULL);
		/* Set each element from array */
		for(current_array_element=0; current_array_element<3; current_array_element++)
		{
			JSObjectSetPropertyAtIndex(context, input_js_array, current_array_element, JSValueMakeBoolean(context, $1[current_array_element]), NULL);
		}
	}
	else
	{
		SWIG_exception_fail(SWIG_TypeError, "Must provide a Javascript array for the ALboolean* values");
	}
}


// alGetDoublev
/* Set the input argument to point to a temporary variable */
/* alGetDoublev has at most 3 elements */
%typemap(in, numinputs=1) ALdouble* algetdoublev_values (ALdouble internal_outvalue_storage[3])
{
   $1 = &internal_outvalue_storage[0];
}
%typemap(argout) ALdouble* algetdoublev_values (JSObjectRef input_js_array, int current_array_element)
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
		SWIG_exception_fail(SWIG_TypeError, "Must provide a Javascript array for the ALdouble* values");
	}
}





/* alSourcePlayv, alSourceStopv, alSourceRewindv, alSourcePausev 
FIXME: Cheating for now. Assuming max 32
*/
%typemap(in) const ALuint* alsource_do_uint_v_values (int input_array_length = 0, JSObjectRef input_js_array, JSValueRef current_array_jsvalue, int current_array_element, ALuint target_array[32])
{
	/* for alSourcefv, the usable array length is no more than 3 */
	if(JSValueIsObject(context, $input))
	{
		/* Convert into Array */
		input_js_array = JSValueToObject(context, $input, NULL);

		input_array_length = (int)SWIG_JSObjectGetNumber(context, SWIG_JSObjectGetNamedProperty(context, input_js_array, "length", NULL), NULL);

		/* Make sure the array length doesn't exceed 3 */
		if(input_array_length > 32)
		{
			input_array_length = 32;
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
				SWIG_exception_fail(SWIG_TypeError, "Not a number type in array for ALuint* values");
			}
		}

		$1 = target_array;
	}
	else
	{
		SWIG_exception_fail(SWIG_ERROR, "$input is not JSObjectRef");
	}
}



/* alSourceUnqueueBuffers */
/* Set the input argument to point to a temporary variable */
/* FIXME: Lazy hack: alSourceUnqueueBuffers has at most 32 elements */
%typemap(in, numinputs=1) ALuint* alsourceunqueuebuffers_values (ALuint internal_outvalue_storage[32])
{
   $1 = &internal_outvalue_storage[0];
}
%typemap(argout) ALuint* alsourceunqueuebuffers_values (JSObjectRef input_js_array, int current_array_element)
{
	if(JSValueIsObject(context, $input))
	{
		input_js_array = JSValueToObject(context, $input, NULL);
		/* Set each element from array */
		for(current_array_element=0; current_array_element<32; current_array_element++)
		{
			JSObjectSetPropertyAtIndex(context, input_js_array, current_array_element, JSValueMakeNumber(context, $1[current_array_element]), NULL);
		}
	}
	else
	{
		SWIG_exception_fail(SWIG_TypeError, "Must provide a Javascript array for the ALfloat* value");
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

void alGetSourcei( ALuint sid, ALenum param, ALint* algetsourcei_value );
/* Yes, all 3 parameters are named the same thing intentionally to get the ALfloat* algetsourcef_value to apply to all parameters. */
void alGetSource3i( ALuint sid, ALenum param, ALint* algetsourcei_value, ALint* algetsourcei_value, ALint* algetsourcei_value);
void alGetSourceiv( ALuint sid, ALenum param, ALint* algetsourceiv_values );


void alListenerfv( ALenum param, const ALfloat* allistenerfv_values );
void alListeneriv( ALenum param, const ALint* allisteneriv_values );

/* Can reuse alGetSource* typemaps for f,i,3f,3i */
void alGetListenerf( ALenum param, ALfloat* algetsourcef_value );
void alGetListener3f( ALenum param, ALfloat* algetsourcef_value, ALfloat* algetsourcef_value, ALfloat* algetsourcef_value );
void alGetListeneri( ALenum param, ALint* algetsourcei_value );
void alGetListener3i( ALenum param, ALint* algetsourcei_value, ALint* algetsourcei_value, ALint* algetsourcei_value );

/* These support arrays up to 6 elements */
void alGetListenerfv( ALenum param, ALfloat* algetlistenerfv_values );
void alGetListeneriv( ALenum param, ALint* algetlisteneriv_values );


/* For all the alBuffer* APIs, I don't think any thing uses more than one value. So we can be lazy and reuse the source typemaps. */
void alBufferfv( ALuint bid, ALenum param, const ALfloat* alsourcefv_values );
void alBufferiv( ALuint bid, ALenum param, const ALint* alsourceiv_values );
void alGetBufferf( ALuint bid, ALenum param, ALfloat* algetsourcef_value );
void alGetBuffer3f( ALuint bid, ALenum param, ALfloat* algetsourcef_value, ALfloat* algetsourcef_value, ALfloat* algetsourcef_value);
void alGetBufferfv( ALuint bid, ALenum param, ALfloat* algetsourcefv_values );
void alGetBufferi( ALuint bid, ALenum param, ALint* algetsourcei_value );
void alGetBuffer3i( ALuint bid, ALenum param, ALint* algetsourcei_value, ALint* algetsourcei_value, ALint* algetsourcei_value);
void alGetBufferiv( ALuint bid, ALenum param, ALint* algetsourceiv_values );



/* I don't think any of these actually need to return more than 1 value. Assume 3 for a little more safety */
/* Reuse alGetSource for float and int for laziness */
void alGetIntegerv( ALenum param, ALint* algetsourceiv_values );
/* alc only has Integerv */
void alcGetIntegerv( ALCdevice* device, ALCenum param, ALCsizei size, ALint* algetsourceiv_values );
void alGetFloatv( ALenum param, ALfloat* algetsourcefv_values );

void alGetBooleanv( ALenum param, ALboolean* algetbooleanv_values );
void alGetDoublev( ALenum param, ALdouble* algetdoublev_values );



/* FIXME: In principle, these are variable sized arrays and should use dynamic memory.
   But I rather use typemaps.i than write it.
   In practice, these are not going to exceed 32 (iOS limit), so I'll be lazy.
*/
void alSourcePlayv( ALsizei ns, const ALuint* alsource_do_uint_v_values );
void alSourceStopv( ALsizei ns, const ALuint* alsource_do_uint_v_values );
void alSourceRewindv( ALsizei ns, const ALuint* alsource_do_uint_v_values );
void alSourcePausev( ALsizei ns, const ALuint* alsource_do_uint_v_values );


/* FIXME: In principle, these are variable sized arrays and should use dynamic memory.
   But I rather use typemaps.i than write it.
   In practice, this number will not be huge. Reuse alSourcePlayv for laziness.
 */
void alSourceQueueBuffers( ALuint sid, ALsizei numEntries, const ALuint* alsource_do_uint_v_values );
void alSourceUnqueueBuffers( ALuint sid, ALsizei numEntries, ALuint* alsourceunqueuebuffers_values );


