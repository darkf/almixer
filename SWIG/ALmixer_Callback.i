#ifdef SWIG_JAVASCRIPT_V8

%inline {

}

%{

%}

#elif SWIG_JAVASCRIPT_JSC

%inline {
typedef struct ALmixer_PlaybackFinishedCallbackContainer {
  JSValueRef   data;
  JSContextRef context;
  JSObjectRef  thisObject;
  JSObjectRef  func; 
  JSValueRef  *exception;
} ALmixer_PlaybackFinishedCallbackContainer;
}

%typemap(in) void *ALmixer_SetPlaybackFinishedCallbackContainer (void  *argp = 0, int res = 0, ALmixer_PlaybackFinishedCallbackContainer *data) {
  res = SWIG_ConvertPtr($input, (void **)&argp,SWIGTYPE_p_ALmixer_PlaybackFinishedCallbackContainer, $disown | %convertptr_flags);
  if (!SWIG_IsOK(res)) { 
    %argument_fail(res, "$type", $symname, $argnum); 
  }

  data = (ALmixer_PlaybackFinishedCallbackContainer *)(argp);

  data->context    = context;
  data->thisObject = thisObject;
  data->func       = func1;
  data->exception = exception;

  $1 = %reinterpret_cast(data, $ltype);
}

%typemap(in) playback_finished_callback func (JSObjectRef func) {
  if (JSValueIsNull(context, $input)) {
    func = NULL;
    $1   = NULL;
  } else {
    func = JSValueToObject(context, $input, NULL);
    $1 = ALmixer_SetPlaybackFinishedCallback_JSCallbackHook;
  }
}

%{
JSValueRef ALmixer_SetPlaybackFinishedCallback_CallAsFunction(
  ALint which_channel, ALuint al_source, ALmixer_Data* almixer_data, ALboolean finished_naturally, ALmixer_PlaybackFinishedCallbackContainer *data, JSObjectRef func) {

  if (func == NULL) return NULL;

  JSContextRef context    = data->context;
  JSObjectRef  thisObject = data->thisObject;
  JSValueRef   args[5];

  args[0] = JSValueMakeNumber(context, which_channel);
  args[1] = JSValueMakeNumber(context, al_source);
  args[2] = SWIG_NewPointerObj(SWIG_as_voidptr(almixer_data), SWIGTYPE_p_ALmixer_Data, 0);
  args[3] = JSValueMakeNumber(context, finished_naturally);
  args[4] = SWIG_NewPointerObj(SWIG_as_voidptr(data), SWIGTYPE_p_ALmixer_PlaybackFinishedCallbackContainer, 0);

  if (JSObjectIsFunction(data->context, func)) {
    JSValueRef result = JSObjectCallAsFunction(
        context, func, thisObject, 5, args, data->exception);
    return result;
  }

  return NULL;
}
void ALmixer_SetPlaybackFinishedCallback_JSCallbackHook(ALint which_channel, ALuint al_source, ALmixer_Data* almixer_data, ALboolean finished_naturally, void* voi) {
  ALmixer_PlaybackFinishedCallbackContainer *data = (ALmixer_PlaybackFinishedCallbackContainer *)voi;
  ALmixer_SetPlaybackFinishedCallback_CallAsFunction(which_channel,al_source,almixer_data,finished_naturally, data, data->func);
}
%}

#else
%warn "Callback binding not implemented for requested target langauge"
#endif
