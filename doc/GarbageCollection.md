


====
Garbage Collection & Wrappers around resource management
====

If you are writing wrappers around ALmixer which need to manage ALmixer_Data life-cycles, here are details you should know.


So this pattern is tricky under GC:
```` C
ALmixer_PlayChannel(-1, ALmixer_LoadStream(“/path/foo.wav”, 0, 0, 0, access_data), -1);
````

Under GC expectations, the ALmixer_Data returned by LoadStream will be collected after this call to PlayChannel. But PlayChannel needs that data to stay alive until playback has completed.

For garbage collected code, you must keep a reference to the ALmixer_Data returned by ALmixer_LoadStream (or LoadAll) when you start playing it. (If you play the same LoadAll multiple times, you need to keep count. So a reference counter is a good way to go for both cases.)

The good news is that ALmixer thought about this problem. So the ALmixer finished (playing) callbacks are your opportunity to decrement the reference count. The finished callbacks have been designed to always fire the finished callback so you can run your management code. (So when calling Halt explicitly, you will still get a finished callback. The finished_naturally flag is intended to let you distinguish why the playback stopped if you need to tell the difference.) 

So in your finished callback, you can decrement the reference count. Once the reference count goes to 0, you can let the system resume full management of the object. Then if the language sees that there are no more other code references holding the data, it is free to collect the object.


This system was also designed to those building wrappers around ALmixer, perhaps for priority level management of channels. As such, the finished callbacks are also your friends as they are designed to reliably tell you which channels/sources are stil in use.



====
Garbage Collection and shutdown
====
ALmixer_QuitWithoutFreeData is a hack introduced to deal with a case where ALmixer was being used in JavaScript (but could be any GC language), but lived in a really difficult layer of code that didn’t have access to main. The issue was ALmixer might be quit before garbage collection was guaranteed to collect all ALmixer_Data objects. Due to implementation details (partly due to how OpenAL memory will be lost (and maybe leaked) when the OpenAL device is closed), if ALmixer_FreeData is called after Quit, crashes could occur. QuitWithoutFreeData was a hack that tried to shutdown without necessarily releasing all the memory. So if GC came in later, there was still a chance the object could be cleaned up. (There is a chance of a leak this way, but it was considered better than crashing.)
