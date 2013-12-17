Introduction:

ALmixer (which I sometimes call "SDL-OpenAL-Mixer" or "SDL_ALmixer") is a cross-platform audio library built on top of OpenAL to make playing and managing sounds easier. 

ALmixer provides a simple API inspired by SDL_mixer to make playing sounds easy with having to worry about directly dealing with OpenAL sources, buffers, 
and buffer queuing directly.

ALmixer currently utilizes SDL_sound behind the scenes to decode 
various audio formats such as WAV, MP3, AAC, MP4, OGG, etc.

This library is targeted towards two major groups:

- People who just want an easy, high performance, way to play audio (don't care if its OpenAL or not)
- People who want to an easy way to play audio in OpenAL but still want access to OpenAL directly.  

ALmixer exposes OpenAL sources in the API so you can freely use ALmixer in larger OpenAL applications that need to apply OpenAL 3D effects and features to playing sounds.

The API is heavily influenced and inspired by SDL_mixer, though there is one major conceptual design difference. ALmixer doesn't divide sound and music playback into two separate play APIs. Instead, there is one unified play API and you specify via the load API whether you want the audio resource loaded as a stream or completely preloaded. This allows you to have any arbitrary number of streaming sources playing simultaneously (such as music and speech) unlike SDL_mixer where you are limited to only one "music" channel.

A less major conceptual design difference is every "Channel" API has a corresponding "Source" API.  Every "channel" (in the SDL_mixer definition context) maps to a corresponding OpenAL source id. You can use this source ID directly with OpenAL API commands to utilize OpenAL effects such as position, Doppler, etc. Convenience APIs are provided to let you convert channel numbers to source ids and vice-versa.

Another change which is a pet-peev of mine with SDL_mixer is the lack of a user_data parameter in callbacks. ALmixer callbacks allow you to pass user_data (aka context) pointers through the callback functions.


SDL_mixer vs. ALmixer:

Why would you want to use ALmixer over SDL_mixer?
- There is no artificial limit or distinction between "music" and "sounds". Instead, you specify if you want to preload a sound fully or as a "stream" and the "play" API automatically and transparently does the right thing. This means you can have multiple streaming sounds playing at the same time like music and speech.
- The callback API allows for void* userdata (a.k.a. context) to be passed through.
- Uses OpenAL as the audio engine instead of SDL.
- Not subject to known SDL and SDL_mixer bugs/limitations
- ALmixer was designed to work cooperatively with OpenAL features and effects.
- ALmixer has been used by millions of users as the sound engine behind the Corona SDK.
- ALmixer has successfully been binded for use in Lua and JavaScript.

Why would you want to use SDL_mixer over ALmixer?
- SDL_mixer has been around longer and audited by more developers.
- OpenAL while an industry cross-platform standard, is still not as ubiquitous as SDL.
- OpenAL may have a different set of bugs and there are different implementations of OpenAL which may have different bugs.
- SDL_mixer effects are not ported. (You should utilize OpenAL effects instead.)


Why ALmixer vs. other sound engines (besides SDL_mixer)?
- ALmixer is written in pure C and has worked on many platforms (e.g. Windows, Linux, Mac, iOS, Android, FreeBSD, Solaris)
- Despite being written in C, the API is very friendly (Corona SDK/Lua developers have done amazing things with it)
- ALmixer interoperates easy with the OpenAL effect APIs
- ALmixer is open source
- ALmixer has good performance and latency characteristics
- ALmixer is relatively easy to bind to other languages
- ALmixer has been used in many, many games, used by millions of users
- ALmixer is has filed more bugs with Apple (and gotten them fixed) than any other sound library


Compile Flags:

There are some #defines you can set to change the behavior at compile time. Most you shouldn't touch.

The one worth noting is ENABLE_ALMIXER_THREADS. If enabled, ALmixer_Update() is automatically called on a background thread so you no longer have to explicitly call it. (The function turns into a no-op so your existing code won't break.) 
Having Update run in a separate thread has some advantages, particularly for streaming audio as all the OpenAL buffer queuing happens in this function. It is less likely the background thread will be blocked for long periods and thus less likely your buffer queues will be starved. However, this means you need to be extra careful about what you do in callback functions as they are invoked from the background thread. 
I still consider this refer to this feature as experimental, but this feature has been in active use in the Corona SDK for many years now and used by millions of users, so it is shippable.



Building:

This project uses CMake.
Check out CMake at http://www.cmake.org
Check out my screencast tutorial at: http://playcontrol.net/ewing/screencasts/getting_started_with_cmake_.html

Typical commandline: (from inside the ALmixer directory)
mkdir BUILD
cd BUILD
cmake ..
make

Or use the ccmake or the CMake GUI to make it easier to configure options like ENABLE_ALMIXER_THREADS.

Android developers should refer to the HelloAndroidALmixer sample project which contains a complete example of how to build OpenAL, ALmixer, and how to integrate into a native Android project.
https://bitbucket.org/ewing/hello-android-almixer


Backstory:

I originally wrote this library back in roughly 2002 to workaround bugs and limitations I was facing with SDL_mixer. I was experiencing latency problems back then with SDL_mixer on certain platforms and I needed the ability to play both music and speech simultaneously which the design of SDL_mixer does not really facilitate. I also needed more decoding formats than SDL_mixer supported, plus at the time, the SDL_mixer backend for music used a different decoding backend than the rest of the library which made it inconsistent.

The ALmixer code was written very quickly in a matter of several weeks.  But in solving all the problems/limitations I had with SDL_mixer, I encountered a whole set of new problems surrounding OpenAL. Back in 2002, OpenAL was on life-support and the 1.0 OpenAL spec was really broken. The differences between implementations of OpenAL differed greatly which made it very difficult to ship a cross-platform application using OpenAL. 

Meanwhile, because the code was written so quickly and also happened to be my first venture into audio (among other things), I felt the code was messy and needed to be cleaned up. The complicated state machine necessary to do what I needed turned out to be very scary. In addition, with all the hacks I needed to  add to workaround OpenAL implementation differences, made the code much more complicated. 

So rather than releasing to the public, I decided to sit on the code and vowed to clean it up some day so I don't embarrass myself. I also expected that SDL_mixer would be rewritten to use SDL_sound soon and maybe some of my other issues might finally be fixed.

Many years passed.

OpenAL 1.1 was ratified and many of the compatibility issues between OpenAL implementations started going away. Every so often, I re-pickup ALmixer and made small changes to update it to support OpenAL 1.1.

Fast forward to 2010 (today). I still haven't cleaned up ALmixer. SDL_mixer has still not been rewritten. And there haven't been any great audio libraries that have emerged in all these years. Furthermore, with renewed interest in playing high performance audio with OpenAL due to the enormous success of iPhone, iPod touch, and iPad, I see there is a still a great void that needs to be filled. (In fact, I just co-authored possibly the most comprehensive book on OpenAL ever written: http://playcontrol.net/iphonegamebook)

And I have recently been working on a project that would benefit greatly from something like ALmixer. I realized that I don't have the time/money to do the clean-up, nor is it feasible for me to do an entire rewrite. I also realize that despite the scariness of the code, the library seems to generally work.

So I have decided to finally release ALmixer, even without the clean ups. My hope is people find it useful and I also get some good testing feedback. Maybe some heros will even make it better. Please be kind when reading the code and reporting bugs. I admit the code is scary and many of the comments are now obsolete.



================
Technical Notes:
================

Threads: 
If you compile with threads enabled, you must understand that most callbacks fire on a background thread. (One exceptions is that HaltChannel callbacks will fire on the calling thread.) 
You are responsible for dealing with thread safety issues.
Also be aware that except for ALmixer_FreeData, it is not safe to call other ALmixer fuctions in a callback. (Recursive locks are NOT used.)
A wishlist item is to switch to spin locks (for speed) instead of recursive locks (instead of safety).


Platform Decoders and minimal depedencies:
ALmixer can be compiled to use SDL+SDL_sound to get the benefit of a mature codebase and vast number of codecs.
However, ALmixer has been developing native platform decoders to eliminate external dependencies (i.e. no SDL and no SDL_sound) and avoid patent and royalty issues, which formats like MP3 and AAC are encumbered with.
ALmixer currently has a mature Core Audio backend (Mac/iOS).
ALmixer has also introduced an Android OpenSL ES backend.
A Windows Media Foundation backend is on the wish list.


Additional Android Notes:
The Android OpenSL ES backend uses Google's decoder API extensions which are introduced in Android 4.0 (API 14). This is the minimum requirement for this backend.
However, Android audio latency problems are infamous (search for Android bug #3434), and things are generally terrible. Android 4.1 improves things significantly (though they still are terrible.
For the sake of avoiding bad reviews, you may want to consider requiring 4.1 as your minimum.

Additionally, the OpenSL ES API extensions fail to provide a classic API that uses function pointers for the fopen/fread/fseek/fclose family to allow abstraction layers to exist. As a consequence, SDL_RWops does not work with the native Android decoder backend. Please file feature requests with Android to convince them to correct this.

See the HelloAndroidALmixer example.
https://bitbucket.org/ewing/hello-android-almixer


LGPL issues:
ALmixer is transitioning away from LGPL to a more liberal license. However, it's dependency on SDL_sound keeps it bound to LGPL. Currently the native Core Audio and Android backends avoid LGPL code as long as you don't compile in additional codecs (labeled under the folder LGPL).


Eric Wing <ewing . public @ playcontrol.net>

ALmixer Home Page: http://playcontrol.net/opensource/ALmixer

