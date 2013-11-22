
#ifdef SWIG_JAVASCRIPT_V8

// alSourcefv
%typemap(in) const ALfloat* alsourcefv_values (int input_array_length = 0, v8::Local<v8::Array> input_js_array, v8::Local<v8::Value> current_array_jsvalue, int current_array_element, ALfloat target_array[3])
{
	/* for alSourcefv, the usable array length is no more than 3 */
	if($input->IsArray())
	{
		// Convert into Array
        input_js_array = v8::Local<v8::Array>::Cast($input);

		input_array_length = input_js_array->Length();

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
			current_array_jsvalue = input_js_array->Get(current_array_element);

			if(current_array_jsvalue->IsNumber())
			{
				target_array[current_array_element] = current_array_jsvalue->NumberValue();
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
%typemap(in) const ALint* alsourceiv_values (int input_array_length = 0, v8::Local<v8::Array> input_js_array, v8::Local<v8::Value> current_array_jsvalue, int current_array_element, ALint target_array[3])
{
	/* for alSourceiv, the usable array length is no more than 3 */
	if($input->IsArray())
	{
		// Convert into Array
        input_js_array = v8::Local<v8::Array>::Cast($input);

		input_array_length = input_js_array->Length();

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
			current_array_jsvalue = input_js_array->Get(current_array_element);

			if(current_array_jsvalue->IsNumber())
			{
				target_array[current_array_element] = current_array_jsvalue->NumberValue();
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

%typemap(argout) ALfloat* algetsourcef_value (v8::Local<v8::Array> input_js_array)
{
	if($input->IsArray())
	{
		input_js_array = v8::Local<v8::Array>::Cast($input);
		// The -3 is needed because in the C APIs that use this typemap, the ALfloat* starts at the 3rd parameter, so $argnum-3 will allow the array elements to start at 0.
		input_js_array->Set($argnum-3, v8::Number::New(*$1));
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
%typemap(in, numinputs=1) ALint* algetsourcei_value (ALint internal_outvalue_storage)
{
   $1 = &internal_outvalue_storage;
}

%typemap(argout) ALint* algetsourcei_value (v8::Local<v8::Array> input_js_array)
{
	if($input->IsArray())
	{
		input_js_array = v8::Local<v8::Array>::Cast($input);
		/* The -3 is needed because in the C APIs that use this typemap, the ALfloat* starts at the 3rd parameter, so $argnum-3 will allow the array elements to start at 0. */
		input_js_array->Set($argnum-3, v8::Integer::New(*$1));
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
%typemap(argout) ALfloat* algetsourcefv_values (v8::Local<v8::Array> input_js_array, int current_array_element)
{
	if($input->IsArray())
	{
		input_js_array = v8::Local<v8::Array>::Cast($input);
		/* Set each element from array */
		for(current_array_element=0; current_array_element<3; current_array_element++)
		{
			input_js_array->Set($argnum-3, v8::Number::New($1[current_array_element]);
		}
	}
	else
	{
		SWIG_exception_fail(SWIG_TypeError, "Must provide a Javascript array for the ALfloat* value");
	}
}


// alListenerfv
%typemap(in) const ALfloat* allistenerfv_values (int input_array_length = 0, v8::Local<v8::Array> input_js_array, v8::Local<v8::Value> current_array_jsvalue, int current_array_element, ALfloat target_array[6])
{
	/* for alListenerfv, the usable array length is no more than 6 */
	if($input->IsArray())
	{
		// Convert into Array
        input_js_array = v8::Local<v8::Array>::Cast($input);

		input_array_length = input_js_array->Length();

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
			current_array_jsvalue = input_js_array->Get(current_array_element);

			if(current_array_jsvalue->IsNumber())
			{
				target_array[current_array_element] = current_array_jsvalue->NumberValue();
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
%typemap(in) const ALint* allisteneriv_values (int input_array_length = 0, v8::Local<v8::Array> input_js_array, v8::Local<v8::Value> current_array_jsvalue, int current_array_element, ALint target_array[6])
{
	/* for alListenerfv, the usable array length is no more than 6 */
	if($input->IsArray())
	{
		// Convert into Array
        input_js_array = v8::Local<v8::Array>::Cast($input);

		input_array_length = input_js_array->Length();

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
			current_array_jsvalue = input_js_array->Get(current_array_element);

			if(current_array_jsvalue->IsNumber())
			{
				target_array[current_array_element] = current_array_jsvalue->NumberValue();
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

#endif /* SWIG_JAVASCRIPT_V8 */

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

void alGetSourcei( ALuint sid, ALenum param, ALint* algetsourcei_value );
/* Yes, all 3 parameters are named the same thing intentionally to get the ALfloat* algetsourcef_value to apply to all parameters. */
void alGetSource3i( ALuint sid, ALenum param, ALint* algetsourcei_value, ALint* algetsourcei_value, ALint* algetsourcei_value);


void alGetSourcef( ALuint sid, ALenum param, ALfloat* algetsourcef_value );
void alGetSource3f( ALuint sid, ALenum param, ALfloat* algetsourcef_value, ALfloat* algetsourcef_value, ALfloat* algetsourcef_value);

void alListenerfv( ALenum param, const ALfloat* allistenerfv_values );
void alListeneriv( ALenum param, const ALint* allisteneriv_values );



