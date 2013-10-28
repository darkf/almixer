#ifdef SWIG_JAVASCRIPT_V8

%{
#include <stdarg.h>
#include <android/log.h>
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG,  "ALmixer", __VA_ARGS__))
%}

%inline {
typedef struct ALmixer_PlaybackFinishedCallbackContainer {
  v8::Persistent<v8::Context> context;
  v8::Persistent<v8::Value> data;
  v8::Persistent<v8::Value> func;
} ALmixer_PlaybackFinishedCallbackContainer;
}

%typemap(in) void *ALmixer_SetPlaybackFinishedCallbackContainer (void  *argp = 0, int res = 0, ALmixer_PlaybackFinishedCallbackContainer *data) {
  res = SWIG_ConvertPtr($input, (void **)&argp,SWIGTYPE_p_ALmixer_PlaybackFinishedCallbackContainer, $disown | %convertptr_flags);
  if (!SWIG_IsOK(res)) { 
    %argument_fail(res, "$type", $symname, $argnum); 
  }

  data = (ALmixer_PlaybackFinishedCallbackContainer *)(argp);
  data->func = v8::Persistent<v8::Value>::New(func1);
  data->context = v8::Persistent<v8::Context>::New(v8::Context::GetCurrent());

  $1 = %reinterpret_cast(data, $ltype);
}

%typemap(in) playback_finished_callback func (v8::Handle<v8::Value> func) {
  if ($input->IsFunction()) {
    func = $input;
    $1   = ALmixer_SetPlaybackFinishedCallback_JSCallbackHook;
  } else {
    func = v8::Null();
    $1   = NULL;
  }
}
%{
v8::Handle<v8::Value> ALmixer_PlaybackFinishedCallbackContainer_Wrap_Pointer(void* ptr, swig_type_info *type_info) {
  v8::HandleScope scope;
  v8::Handle<v8::Value> ptrobj = SWIG_NewPointerObj(SWIG_as_voidptr(ptr), type_info, 0 |  0 );
  return scope.Close(ptrobj);
}

void ALmixer_SetPlaybackFinishedCallback_CallAsFunction(
    ALint which_channel, ALuint al_source, ALmixer_Data* almixer_data,
    ALboolean finished_naturally, ALmixer_PlaybackFinishedCallbackContainer *data, v8::Handle<v8::Value> func) {

  //v8::Isolate* isolate = v8::Isolate::GetCurrent();
  //if (isolate == NULL) {
  //  isolate = v8::Isolate::New();
  //}
  //isolate->Enter();
  {
    v8::Locker locker;
    v8::HandleScope scope;
    v8::Handle<v8::Value> args[5];
    if (!func->IsFunction()) return;

    args[0] = v8::Number::New(which_channel);
    args[1] = v8::Number::New(al_source);
    args[2] = ALmixer_PlaybackFinishedCallbackContainer_Wrap_Pointer(almixer_data, SWIGTYPE_p_ALmixer_Data);
    args[3] = v8::Boolean::New(finished_naturally);
    args[4] = ALmixer_PlaybackFinishedCallbackContainer_Wrap_Pointer(data, SWIGTYPE_p_ALmixer_PlaybackFinishedCallbackContainer);
    v8::Persistent<v8::Function> op = v8::Persistent<v8::Function>::New(v8::Handle<v8::Function>::Cast(func));
    //op->Call(v8::Context::GetCurrent()->Global(), 5, args);
    op->Call(op, 5, args);
  }
  //isolate->Exit();
}
void ALmixer_SetPlaybackFinishedCallback_JSCallbackHook(ALint which_channel, ALuint al_source, ALmixer_Data* almixer_data, ALboolean finished_naturally, void* voi) {
  ALmixer_PlaybackFinishedCallbackContainer *data = (ALmixer_PlaybackFinishedCallbackContainer *)voi;
  ALmixer_SetPlaybackFinishedCallback_CallAsFunction(which_channel,al_source,almixer_data,finished_naturally, data, data->func);
}
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
  args[3] = JSValueMakeBoolean(context, finished_naturally);
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
