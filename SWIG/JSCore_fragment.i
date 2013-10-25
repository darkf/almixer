/*
 * JavaScriptCore Support Fragments
 */
%fragment("SWIG_JSObjectGetNumber", "header") {
static double SWIG_JSObjectGetNumber(JSContextRef context, JSValueRef value, JSValueRef* exception)
{
    double result = JSValueToNumber(context, value, exception);
    return result;
}
}
%fragment("SWIG_JSObjectGetNamedProperty",  "header") {
static JSValueRef SWIG_JSObjectGetNamedProperty(JSContextRef context, JSObjectRef array, const char* property, JSValueRef* exception)
{
    JSStringRef propName = JSStringCreateWithUTF8CString(property);
    JSValueRef result = JSObjectGetProperty(context, array, propName, exception);
    JSStringRelease(propName);
    return result;
}
}