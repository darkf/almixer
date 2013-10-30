var s_JSALmixerIsInitialized;
var s_JSALmixerDataChannelTable;
// I learned the hard way that the Ti Proxy must be held globally or the addEventListener for ALmixerSoundPlaybackFinished gets collected.
var s_JSALmixerTiProxy;

var playbackFinishedCallbackContainer;

function JSALmixer_Initialize() {
	if(s_JSALmixerIsInitialized) {
		return;
	}
	s_JSALmixerIsInitialized = true;

	var almixer_ti_proxy = require('co.lanica.almixer');
	s_JSALmixerTiProxy = almixer_ti_proxy;
	var ALmixer = co_lanica_almixer;
	// Create a table to hold the original versions of the functions I intend to override.
	ALmixer._original = {};
	// Create a table for utility/helper APIs
	ALmixer.util = {};

	s_JSALmixerDataChannelTable = {};

	almixer_ti_proxy.addEventListener('ALmixerSoundPlaybackFinished', function(e) {
		var which_channel = e.which_channel;
		var saved_table = s_JSALmixerDataChannelTable[which_channel];

		var callback_function = saved_table.onComplete;
		var sound_handle = saved_table.soundHandle;
		var event_table =
		{
			name:"ALmixer",
			type:"completed",
			which_channel:which_channel,
			channel_source:e.channel_source,
			completed:e.completed,
			handle:saved_table.soundHandle,
		};

		// We can now free our saved reference
		s_JSALmixerDataChannelTable[which_channel] = null;
		//
		// Invoke user callback
		if(null !== callback_function && undefined !== callback_function) {
			callback_function(event_table);
			event_table = null;
			callback_function = null;
		}
		event_table = null;
	});

	ALmixer._original.PlayChannelTimed = ALmixer.PlayChannelTimed;
	ALmixer.PlayChannelTimed = function(which_channel, sound_handle, num_loops, duration, on_complete) {
		var playing_channel = ALmixer._original.PlayChannelTimed(which_channel, sound_handle, num_loops, duration);
		// Do only if playing succeeded
		if(playing_channel > -1) {
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
	};

	ALmixer._original.PlayChannel = ALmixer.PlayChannel;
	ALmixer.PlayChannel = function(which_channel, sound_handle, num_loops, on_complete) {
		var playing_channel = ALmixer._original.PlayChannel(which_channel, sound_handle, num_loops);
		// Do only if playing succeeded
		if(playing_channel > -1) {
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
	};

	ALmixer._original.PlaySourceTimed = ALmixer.PlaySourceTimed;
	ALmixer.PlaySourceTimed = function(al_source, sound_handle, num_loops, duration, on_complete) {
		var playing_source = ALmixer._original.PlaySourceTimed(al_source, sound_handle, num_loops, duration);
		// Do only if playing succeeded
		if(playing_source > 0) {
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
	};

	ALmixer._original.PlaySource = ALmixer.PlaySource;
	ALmixer.PlaySourceTimed = function(al_source, sound_handle, num_loops, on_complete) {
		var playing_source = ALmixer._original.PlaySource(al_source, sound_handle, num_loops);
		// Do only if playing succeeded
		if(playing_source > 0) {
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
	};


	function JSALmixerPlaySound(sound_handle, options_table) {
		var which_channel = -1;
		var num_loops = 0;
		var duration = -1;
		var on_complete = null;
		if(null !== options_table) {
			if(null !== options_table.which_channel) {
				which_channel = options_table.which_channel;
			} else if(null !== options_table.channel_source) {
				which_channel = ALmixer.GetChannel(options_table.channel_source);
			}


			if(null !== options_table.loops) {
				num_loops = options_table.loops;
			} 
			if(null !== options_table.duration) {
				duration = options_table.duration;
			}
			if(null !== options_table.onComplete) {
				on_complete = options_table.onComplete;
			}
			
		}
		// Could call overridden version and omit code instead.
		var playing_channel = ALmixer._original.PlayChannelTimed(which_channel, sound_handle, num_loops, duration);
		// Do only if playing succeeded
		if(playing_channel > -1) {
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

		// I wish Javascript had multiple return values
		return playing_channel;

	}

	ALmixer.util.Play = JSALmixerPlaySound;
}

JSALmixer_Initialize();
