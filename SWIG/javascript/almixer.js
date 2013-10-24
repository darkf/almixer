Ti.API.info("In Module_Initialize.js");

var s_JSALmixerIsInitialized;
var s_JSALmixerDataChannelTable;
// I learned the hard way that the Ti Proxy must be held globally or the addEventListener for ALmixerSoundPlaybackFinished gets collected.
var s_JSALmixerTiProxy;

function JSALmixer_Initialize()
{
	if(s_JSALmixerIsInitialized)
	{
	Ti.API.info("s_JSALmixerIsInitialized is true");
		
		return;
	}
	Ti.API.info("s_JSALmixerIsInitialized is false");
	s_JSALmixerIsInitialized = true;

	var almixer_ti_proxy = require('co.lanica.almixer');
	s_JSALmixerTiProxy = almixer_ti_proxy;
	var ALmixer = co_lanica_almixer;
	// Create a table to hold the original versions of the functions I intend to override.
	ALmixer._original = {};
	// Create a table for utility/helper APIs
	ALmixer.util = {};

	s_JSALmixerDataChannelTable = {}
	Ti.API.info("In Module_Initialize.js, past require " + almixer_ti_proxy);


	almixer_ti_proxy.addEventListener('ALmixerSoundPlaybackFinished', 
		function(e)
		{
			
	  Ti.API.info("addEventListener name is "+e.name);
	////  Ti.API.info("handle is "+e.handle);
	  Ti.API.info("addEventListener channel is "+e.channel);
	  Ti.API.info("addEventListener source is "+e.alsource);
	  Ti.API.info("addEventListener completed is "+e.completed);

		var which_channel = e.channel;
		var saved_table = s_JSALmixerDataChannelTable[which_channel];
		Ti.API.info("addEventListener saved_table is "+saved_table);

		var callback_function = saved_table.onComplete;
		var sound_handle = saved_table.soundHandle;
		var event_table =
		{
			name:"ALmixer",
			type:"completed",
			channel:which_channel,
			alsource:e.alsource,
			completed:e.completed,
			handle:saved_table.soundHandle,
		};

		// We can now free our saved reference
		s_JSALmixerDataChannelTable[which_channel] = null;
		//
		// Invoke user callback
		if(null != callback_function)
		{
		  Ti.API.info("callback_function "+callback_function);
			callback_function(event_table);
			event_table = null;
			callback_function = null;
		}
		event_table = null;

	});

	ALmixer._original.PlayChannelTimed = ALmixer.PlayChannelTimed;
	ALmixer.PlayChannelTimed = function(which_channel, sound_handle, num_loops, duration, on_complete)
	{
		var playing_channel = ALmixer._original.PlayChannelTimed(which_channel, sound_handle, num_loops, duration);
		// Do only if playing succeeded
		if(playing_channel > -1)
		{
			// Save the sound_handle to solve two binding problems.
			// Problem 1: Garbage collection. The object looks like it disappears and resurrects later.
			// We can't let the garbage collector collect the object while it is playing.
			// So we need keep a strong reference around.
			// Note: The sound could be playing on multiple channels, so we need to make sure we cover that case.
			// Problem 2: ALmixer_Data for callbacks. Currently it is not clear how to get the ALmixer_Data pointer
			// back to Javascript in a form that is usable using the Titanium callback system since it doesn't handle
			// pointers and doesn't know anything about SWIG.
			// The solution to both problems is to keep a global table mapping channels to sound_handles.
			// For each playing channel, we hold a reference to the sound which keeps the object alive 
			// (and handles the multiple channel case). 
			// In the callback, we can retrieve the data by looking up the channel which solves the pointer problem.
			// Problem 3: We would also like to make the API nicer for Javascript and let people pass in anonymous callback functions for each channel.
			// This table structure will let us also keep around their callback.
			s_JSALmixerDataChannelTable[playing_channel] = { soundHandle:sound_handle, onComplete:on_complete };
		}
		return playing_channel;
	}

	ALmixer._original.PlayChannel = ALmixer.PlayChannel;
	ALmixer.PlayChannel = function(which_channel, sound_handle, num_loops, on_complete)
	{
		var playing_channel = ALmixer._original.PlayChannel(which_channel, sound_handle, num_loops);
		// Do only if playing succeeded
		if(playing_channel > -1)
		{
			// Save the sound_handle to solve two binding problems.
			// Problem 1: Garbage collection. The object looks like it disappears and resurrects later.
			// We can't let the garbage collector collect the object while it is playing.
			// So we need keep a strong reference around.
			// Note: The sound could be playing on multiple channels, so we need to make sure we cover that case.
			// Problem 2: ALmixer_Data for callbacks. Currently it is not clear how to get the ALmixer_Data pointer
			// back to Javascript in a form that is usable using the Titanium callback system since it doesn't handle
			// pointers and doesn't know anything about SWIG.
			// The solution to both problems is to keep a global table mapping channels to sound_handles.
			// For each playing channel, we hold a reference to the sound which keeps the object alive 
			// (and handles the multiple channel case). 
			// In the callback, we can retrieve the data by looking up the channel which solves the pointer problem.
			// Problem 3: We would also like to make the API nicer for Javascript and let people pass in anonymous callback functions for each channel.
			// This table structure will let us also keep around their callback.
			s_JSALmixerDataChannelTable[playing_channel] = { soundHandle:sound_handle, onComplete:on_complete };
		}
		return playing_channel;
	}

	ALmixer._original.PlaySourceTimed = ALmixer.PlaySourceTimed;
	ALmixer.PlaySourceTimed = function(al_source, sound_handle, num_loops, duration, on_complete)
	{
		var playing_source = ALmixer._original.PlaySourceTimed(al_source, sound_handle, num_loops, duration);
		// Do only if playing succeeded
		if(playing_source > 0)
		{
			// convert source to channel
			var playing_channel = ALmixer.GetChannel(playing_source);
			
			// Save the sound_handle to solve two binding problems.
			// Problem 1: Garbage collection. The object looks like it disappears and resurrects later.
			// We can't let the garbage collector collect the object while it is playing.
			// So we need keep a strong reference around.
			// Note: The sound could be playing on multiple channels, so we need to make sure we cover that case.
			// Problem 2: ALmixer_Data for callbacks. Currently it is not clear how to get the ALmixer_Data pointer
			// back to Javascript in a form that is usable using the Titanium callback system since it doesn't handle
			// pointers and doesn't know anything about SWIG.
			// The solution to both problems is to keep a global table mapping channels to sound_handles.
			// For each playing channel, we hold a reference to the sound which keeps the object alive 
			// (and handles the multiple channel case). 
			// In the callback, we can retrieve the data by looking up the channel which solves the pointer problem.
			// Problem 3: We would also like to make the API nicer for Javascript and let people pass in anonymous callback functions for each channel.
			// This table structure will let us also keep around their callback.
			s_JSALmixerDataChannelTable[playing_channel] = { soundHandle:sound_handle, onComplete:on_complete };
		}
		return playing_source;
	}

	ALmixer._original.PlaySource = ALmixer.PlaySource;
	ALmixer.PlaySourceTimed = function(al_source, sound_handle, num_loops, on_complete)
	{
		var playing_source = ALmixer._original.PlaySource(al_source, sound_handle, num_loops);
		// Do only if playing succeeded
		if(playing_source > 0)
		{
			// convert source to channel
			var playing_channel = ALmixer.GetChannel(playing_source);
			
			// Save the sound_handle to solve two binding problems.
			// Problem 1: Garbage collection. The object looks like it disappears and resurrects later.
			// We can't let the garbage collector collect the object while it is playing.
			// So we need keep a strong reference around.
			// Note: The sound could be playing on multiple channels, so we need to make sure we cover that case.
			// Problem 2: ALmixer_Data for callbacks. Currently it is not clear how to get the ALmixer_Data pointer
			// back to Javascript in a form that is usable using the Titanium callback system since it doesn't handle
			// pointers and doesn't know anything about SWIG.
			// The solution to both problems is to keep a global table mapping channels to sound_handles.
			// For each playing channel, we hold a reference to the sound which keeps the object alive 
			// (and handles the multiple channel case). 
			// In the callback, we can retrieve the data by looking up the channel which solves the pointer problem.
			// Problem 3: We would also like to make the API nicer for Javascript and let people pass in anonymous callback functions for each channel.
			// This table structure will let us also keep around their callback.
			s_JSALmixerDataChannelTable[playing_channel] = { soundHandle:sound_handle, onComplete:on_complete };
		}
		return playing_source;
	}


	function JSALmixerPlaySound(sound_handle, options_table)
	{
		var which_channel = -1;
		var num_loops = 0;
		var duration = -1;
		var on_complete = null;
		if(null != options_table)
		{
			if(null != options_table.channel)
			{
				which_channel = options_table.channel;
			}
			else if(null != options_table.alsource)
			{
				which_channel = ALmixer.GetChannel(options_table.alsource);
			}


			if(null != options_table.loops)
			{
				num_loops = options_table.loops;
			}
			if(null != options_table.duration)
			{
				duration = options_table.duration;
			}
			if(null != options_table.onComplete)
			{
				on_complete = options_table.onComplete;
			}
			
		}
//		  Ti.API.info("JSALmixerPlaySound call on channel:" + which_channel + " sound_handle:" + sound_handle);

		// Could call overridden version and omit code instead.
		var playing_channel = ALmixer._original.PlayChannelTimed(which_channel, sound_handle, num_loops, duration);
		// Do only if playing succeeded
		if(playing_channel > -1)
		{
			// Save the sound_handle to solve two binding problems.
			// Problem 1: Garbage collection. The object looks like it disappears and resurrects later.
			// We can't let the garbage collector collect the object while it is playing.
			// So we need keep a strong reference around.
			// Note: The sound could be playing on multiple channels, so we need to make sure we cover that case.
			// Problem 2: ALmixer_Data for callbacks. Currently it is not clear how to get the ALmixer_Data pointer
			// back to Javascript in a form that is usable using the Titanium callback system since it doesn't handle
			// pointers and doesn't know anything about SWIG.
			// The solution to both problems is to keep a global table mapping channels to sound_handles.
			// For each playing channel, we hold a reference to the sound which keeps the object alive 
			// (and handles the multiple channel case). 
			// In the callback, we can retrieve the data by looking up the channel which solves the pointer problem.
			// Problem 3: We would also like to make the API nicer for Javascript and let people pass in anonymous callback functions for each channel.
			// This table structure will let us also keep around their callback.
			s_JSALmixerDataChannelTable[playing_channel] = { soundHandle:sound_handle, onComplete:on_complete };
//			  Ti.API.info("setting callback:" + on_complete);
			
		}
	//		  Ti.API.info("PlayChannelTimed playing on channel:" + playing_channel);

		// I wish Javascript had multiple return values
		return playing_channel;

	}

	ALmixer.util.Play = JSALmixerPlaySound;
	/*
	var resource_dir = Ti.Filesystem.resourcesDirectory;
	Ti.API.info("resource_dir is => "+resource_dir);

	//var resource_dir = Ti.Filesystem.resourcesDirectory + Ti.Filesystem.separator;
	// Originally, I was getting a file://localhost/ at the beginning of the directory. 
	// This is a problem because ALmixer needs file paths compatible with the typical fopen type family.
	resource_dir = resource_dir.replace(/^file:\/\/localhost/g,'');
	// Later, Titanium started giving me URLs like file:// without the localhost. So this is a fallback string replacement.
	resource_dir = resource_dir.replace(/^file:\/\//g,'');
	// Replace %20 with spaces.
	resource_dir = resource_dir.replace(/%20/g,' ');

	var full_file_path = resource_dir + "pew-pew-lei.wav";
	Ti.API.info("full_file_path is => "+full_file_path);

	// A nice convenience function would be to automatically try looking in the Resource directory if the user did not provide an absolute path.
	var sound_handle_pew = ALmixer.LoadAll(full_file_path, 0);
	//var pew_channel = ALmixer.Play(sound_handle_pew, options_table);

		var playing_channel = ALmixer.PlayChannelTimed(-1, sound_handle_pew, 0, -1);
	*/
	  Ti.API.info("In Module_Initialize.js, end ");

}

JSALmixer_Initialize();
