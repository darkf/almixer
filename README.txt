Introduction:

ALmixer is a cross-platform audio library built on top of OpenAL to make playing and managing sounds easier. 

ALmixer provides a simple API inspired by SDL_mixer to make playing sounds easy with having to worry about directly dealing with OpenAL sources, buffers, 
and buffer queuing directly.

While its origins come from the SDL universe, ALmixer no longer requires SDL and can be built completely standalone, with the only dependendcy being OpenAL.
This means you can use ALmixer in any environment you wish.

ALmixer provides native decoders for Mac, iOS (CoreAudio), Android (OpenSLES), and Windows (Windows Media Foundation).
This yields the best performance, best battery life, and minimal binary size when leveraging the native backends, while providing WAV, MP3, and the ever elusive AAC/MP4 formats.
Additional decoders may be compiled in with ALmixer either directly, or optionally through the use of SDL_sound, such as OGG. 
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
- ALmixer has been battle hardened since 2011, as the core audio engine of the Corona SDK by thousands of developers and millions of players (iOS, Android, Mac, Windows)
- ALmixer has successfully been binded for use in Lua and JavaScript.
- ALmixer has been around for over a decade.
- ALmixer owned Apple's bug list on OpenAL for many years, and helped work out many problems on Mac and iOS.
- Thanks to commercial interests, a lot more attention has been focused on mobile (iOS/Android) than most other cross platform audio libraries.


Why would you want to use SDL_mixer over ALmixer?
- Despite ALmixer's age, SDL_mixer has still been around longer and audited by more developers.
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


By default, ALmixer compiles for standalone mode. Optionally, you may change this so SDL is a dependency. This will shrink the binary size slightly. (I measure 26KB with -Os on Mac.) 
Another option is to compile with SDL_sound as a dependency. This assumes SDL is a dependency and will disable all the built-in ALmixer decoders. In this mode, ALmixer will defer to SDL_sound and the decoders it provides.


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

Mac and iOS:
I have Xcode projects in addition to CMake. These enable ENABLE_ALMIXER_THREADS and do not have SDL dependencies.
If you use CMake, there is a currently a bug/limitation in CMake that looks in /System/Library/Frameworks for built-in frameworks. This used to be the correct thing, but is no longer the case because Apple does not put header files in there any more. To fix the OPENAL_INCLUDE_DIR option, you need to point it to the correct location inside the Xcode.app, e.g.
/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk/System/Library/Frameworks/OpenAL.framework/Headers



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
ALmixer can be compiled to use SDL to minimize some code duplication.
You may additionally use SDL_sound to gain access to different decoders.
However, ALmixer has been developing native platform decoders to eliminate external dependencies (i.e. no SDL and no SDL_sound) and avoid patent and royalty issues, which formats like MP3 and AAC/MP4 are encumbered with.
ALmixer currently has a Core Audio backend (Mac/iOS), Android OpenSL ES backend, and a Windows Media Foundation backend, all of which support MP3 and AAC/MP4 with the platform vendors providing protection for patents/royalties.
(Be aware, for MP3, you may be subject to royalties if you distribute MP3 files. AAC/MP4 typically does not share this restriction. Please consult your legal counsel.)

Additional Android Notes:
The Android OpenSL ES backend uses Google's decoder API extensions which are introduced in Android 4.0 (API 14). This is the minimum requirement for this backend.
However, Android audio latency problems are infamous (search for Android bug #3434), and things are generally terrible. Android 4.1 improves things significantly (though they still are terrible.
For the sake of avoiding bad reviews, you may want to consider requiring 4.1 as your minimum.

Additionally, the OpenSL ES API extensions fail to provide a classic API that uses function pointers for the fopen/fread/fseek/fclose family to allow abstraction layers to exist. As a consequence, only physical files are supported and reading sources from memory will not work with the native Android decoder backend. Please file feature requests with Android to convince them to correct this.

See the HelloAndroidALmixer example.
https://bitbucket.org/ewing/hello-android-almixer


Windows Media Foundation:
A native Windows Media Foundation implementation is available. Microsoft introduced the APIs in Vista. However, due to bugs and limitations in Vista with respect to WMF, everybody says assume Windows 7 is the minimum requirement.
Now that Microsoft has officially ended XP support, and the fact that nobody runs Vista, and the lion's share of users now run Windows 7, this seems like a reasonable minimum system requirement.



RWops:
ALmixer_RWops is a direct copy from SDL_RWops. In fact, its intent is to behave identically and be binary compatible with SDL_RWops. 
When compiled with SDL as a dependency, ALmixer actually uses SDL_rwops directly and casts the pointer behind the scenes.
When compiled as standalone, source code was copied from SDL and then stripped down to remove everything but the parts needed for RWops. 
The ALmixer_RWops header should have the same fields and sizes and alignments as SDL_RWops, and the implementation is supposed to be the same, so in theory, these are still interchangable via pointer casting.


LGPL issues:
ALmixer is transitioning away from LGPL to a more liberal license. However, it's dependency on SDL_sound keeps it bound to LGPL. Currently the native Core Audio and Android backends avoid LGPL code as long as you don't compile in additional codecs (labeled under the folder LGPL).


Repository Layout:

include/
Public Headers are in include/
ALmixer.h
ALmixer_RWops.h
ALmixer_PlatformExtensions.h

src/
Files contained directly in src are considered "core" ALmixer files

src/StandAlone
Contains files needed to implement ALmixer when SDL is not used

src/StandAlone/SDL
Contains the ripped out SDL_RWops implementation from SDL for when SDL is not used

src/StandAlone/SoundDecoder
Contains the SoundDecoder interface and decoders for when SDL_sound is not used

src/StandAlone/SoundDecoder/LGPL
Contains additional decoders for use with SoundDecoder, but are under the LGPL license.



Eric Wing <ewing . public @ playcontrol.net>

ALmixer Home Page: http://playcontrol.net/opensource/ALmixer

