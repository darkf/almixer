var s_JSALmixerIsInitialized;
var s_JSALmixerDataChannelTable;
// I learned the hard way that the Ti Proxy must be held globally or the addEventListener for ALmixerSoundPlaybackFinished gets collected.
var s_JSALmixerTiProxy;

var playbackFinishedCallbackContainer;

// Ugh: There is a race condition because the Titanium callbacks may be deferred.
// Calling Halt immediatelly followed by play is legal in C because the ALmixer callback fires immediately.
// But in a scripting language binding where the callback may be queued (e.g. thread redirection),
// we can't assume that. 
// The problem is that to deal with memory management, 
// I'm keeping my own shadow table that keeps ALmixer_Data rooted and keeps mappings 
// to scripting language defined callback functions.
// If Play is immediately called after Halt(), but the callback hasn't fired yet,
// I will accidentally overwrite the callback data that is supposed to fire with 
// the next Play's callback data.
// To avoid, I need to keep a queue of callback data which PushCallbackTable and ShiftCallbackTable provide.
function JSALmixer_PushCallbackTable(which_channel, saved_table)
{
	if(!s_JSALmixerDataChannelTable[which_channel])
	{
		s_JSALmixerDataChannelTable[which_channel] = [];
	}
	s_JSALmixerDataChannelTable[which_channel].push(saved_table);
}

function JSALmixer_ShiftCallbackTable(which_channel)
{
	return s_JSALmixerDataChannelTable[which_channel].shift();
}
		function JSALmixer_ALmixerSoundPlaybackFinished(al_source, finished_naturally)
		{
			// This will remove the callback table from the queue and return it.
			var which_channel = ALmixer.GetChannel(al_source);
			var saved_table = JSALmixer_ShiftCallbackTable(which_channel);

			var callback_function = saved_table.onComplete;
			var sound_handle = saved_table.soundHandle;
			var event_table =
			{
				name:"ALmixer",
				type:"completed",
				channel:which_channel,
				alsource:al_source,
				finishedNaturally:finished_naturally,
				handle:saved_table.soundHandle,
				userData:saved_table.userData
			};

			// Invoke user callback
			if(callback_function)
			{
				callback_function(event_table);
				event_table = null;
				callback_function = null;
			}
			event_table = null;
			saved_table = null;
		}

function JSALmixer_Initialize()
{
	if(s_JSALmixerIsInitialized)
	{
		return;
	}
	s_JSALmixerIsInitialized = true;

//	var almixer_ti_proxy = require('co.lanica.almixer');
//	s_JSALmixerTiProxy = almixer_ti_proxy;
//	var ALmixer = co_lanica_almixer;
	// Create a table to hold the original versions of the functions I intend to override.
	ALmixer._original = {};
	// Create a table for utility/helper APIs
	ALmixer.util = {};
	// Create a table for experiemental functions
	ALmixer.experimental = {};

	s_JSALmixerDataChannelTable = {};

/*
	almixer_ti_proxy.addEventListener('ALmixerSoundPlaybackFinished', function(e)
	{
		var which_channel = e.which_channel;
		// This will remove the callback table from the queue and return it.
		var saved_table = JSALmixer_ShiftCallbackTable(which_channel);

		var callback_function = saved_table.onComplete;
		var sound_handle = saved_table.soundHandle;
		var event_table =
		{
			name:"ALmixer",
			type:"completed",
			channel:which_channel,
			alsource:e.channel_source,
			finishedNaturally:e.finished_naturally,
			handle:saved_table.soundHandle,
			userData:saved_table.userData
		};

		// Invoke user callback
		if(callback_function)
		{
			callback_function(event_table);
			event_table = null;
			callback_function = null;
		}
		event_table = null;
		saved_table = null;
	});
*/



/*
	ALmixer.experimental.collectgarbage = function()
	{
		almixer_ti_proxy.collectgarbage();
	}
*/
	ALmixer._original.PlayChannelTimed = ALmixer.PlayChannelTimed;
	ALmixer.PlayChannelTimed = function(which_channel, sound_handle, num_loops, duration, on_complete, user_data)
	{
		// Allow which_channel to be omitted (automatically assumes -1).
		// This means all parameters need to shift over
		if(typeof(which_channel) === "object")
		{
			// Careful: order of the shift matters or we clobber a variable we need to save
			user_data = on_complete;
			on_complete = duration;
			duration = num_loops;
			num_loops = sound_handle;
			sound_handle = which_channel;
			which_channel = -1;
		}

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
			JSALmixer_PushCallbackTable(playing_channel, { soundHandle:sound_handle, onComplete:on_complete, userData:user_data } );

		}
		return playing_channel;
	};

	ALmixer._original.PlayChannel = ALmixer.PlayChannel;
	ALmixer.PlayChannel = function(which_channel, sound_handle, num_loops, on_complete, user_data)
	{
		// Allow which_channel to be omitted (automatically assumes -1).
		// This means all parameters need to shift over
		// Assumption is that the following parameter MUST be the ALmixer_Data which is an object.
		if(typeof(which_channel) === "object")
		{
			// Careful: order of the shift matters or we clobber a variable we need to save
			user_data = on_complete;
			on_complete = num_loops;
			num_loops = sound_handle;
			sound_handle = which_channel;
			which_channel = -1;
		}

		// Also allow num_loops to be omitted (automatically assumes 0).
		// This means all parameters following need to shift over
		// Note: Because the on_complete is optional, we explicitly test for the number and blindly shift.
		if(typeof(num_loops) !== "number")
		{
			// Careful: order of the shift matters or we clobber a variable we need to save
			user_data = on_complete;
			on_complete = num_loops;
			num_loops = 0;
		}

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
			JSALmixer_PushCallbackTable(playing_channel, { soundHandle:sound_handle, onComplete:on_complete, userData:user_data } );
		}
		return playing_channel;
	};

	ALmixer._original.PlaySourceTimed = ALmixer.PlaySourceTimed;
	ALmixer.PlaySourceTimed = function(al_source, sound_handle, num_loops, duration, on_complete, user_data)
	{
		// Allow which_channel to be omitted (automatically assumes 0).
		// This means all parameters need to shift over
		if(typeof(al_source) === "object")
		{
			// Careful: order of the shift matters or we clobber a variable we need to save
			user_data = on_complete;
			on_complete = duration;
			duration = num_loops;
			num_loops = sound_handle;
			sound_handle = al_source;
			al_source = 0;
		}

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
			JSALmixer_PushCallbackTable(playing_channel, { soundHandle:sound_handle, onComplete:on_complete, userData:user_data } );
		}
		return playing_source;
	};

	ALmixer._original.PlaySource = ALmixer.PlaySource;
	ALmixer.PlaySource = function(al_source, sound_handle, num_loops, on_complete, user_data)
	{
		// Allow which_channel to be omitted (automatically assumes 0).
		// This means all parameters need to shift over
		if(typeof(al_source) === "object")
		{
			// Careful: order of the shift matters or we clobber a variable we need to save
			user_data = on_complete;
			on_complete = num_loops;
			num_loops = sound_handle;
			sound_handle = al_source;
			al_source = 0;
		}

		// Also allow num_loops to be omitted (automatically assumes 0).
		// This means all parameters following need to shift over
		// Note: Because the on_complete is optional, we explicitly test for the number and blindly shift.
		if(typeof(num_loops) !== "number")
		{
			// Careful: order of the shift matters or we clobber a variable we need to save
			user_data = on_complete;
			on_complete = num_loops;
			num_loops = 0;
		}

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
			JSALmixer_PushCallbackTable(playing_channel, { soundHandle:sound_handle, onComplete:on_complete, userData:user_data } );
		}
		return playing_source;
	};


	function JSALmixerPlaySound(sound_handle, options_table)
	{
		var which_channel = -1;
		var al_source = 0;
		var num_loops = 0;
		var duration = -1;
		var on_complete = null;
		var user_data = null;
		var using_source_instead_of_channel = false;
		if(options_table)
		{
			if(options_table.channel)
			{
				which_channel = options_table.channel;
			}
			else if(options_table.alsource)
			{
				which_channel = ALmixer.GetChannel(options_table.alsource);
				using_source_instead_of_channel = true;
			}


			if(options_table.loops)
			{
				num_loops = options_table.loops;
			} 
			if(options_table.duration)
			{
				duration = options_table.duration;
			}
			if(options_table.onComplete)
			{
				on_complete = options_table.onComplete;
			}
			if(options_table.userData)
			{
				user_data = options_table.userData;
			}
		}
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
			JSALmixer_PushCallbackTable(playing_channel, { soundHandle:sound_handle, onComplete:on_complete, userData:user_data } );
		}

		// I wish Javascript had multiple return values
		if(using_source_instead_of_channel)
		{
			var playing_source = 0;
			if(playing_channel > -1)
			{
				playing_source = ALmixer.GetSource(playing_channel);
			}
			return playing_source;
		}
		else
		{
			return playing_channel;
		}
	}

	ALmixer.util.Play = JSALmixerPlaySound;


	function JSALmixerSanitizeFilePath(filename)
	{
		var resource_dir;

		/* There are several cases:
		 * User passed an absolute path starting with /
		 * User passed some kind of URL
		 * User passed a relative path (which we will interpret as looking in the platform's default resource directory)
		 */
		/* http://stackoverflow.com/questions/646628/javascript-startswith, see Mark Byers */
		if(filename.lastIndexOf("/", 0) === 0)
		{
			// do nothing and keep the filename exactly as it was
		}
		// I don't know all the permutations, but my Titanium encounters so far start with file:
		else if(filename.lastIndexOf("file:", 0) === 0)
		{
			// Originally, I was getting a file://localhost/ at the beginning of the directory. 
			// This is a problem because ALmixer needs file paths compatible with the typical fopen type family.
			filename = filename.replace(/^file:\/\/localhost/g,'');
			// Later, Titanium started giving me URLs like file:// without the localhost. So this is a fallback string replacement.
			filename = filename.replace(/^file:\/\//g,'');
			// Replace %20 with spaces.
			filename = filename.replace(/%20/g,' ');
		}
		else
		{
			// In Titanium, for Android, the relative path must be prepended with Resources/
			// Mac/iOS should manually compose the NSBundle resourcePath.
			if(typeof Titanium !== "undefined")
			{
				if(Titanium.Platform.osname === 'android')
				{
					filename = "Resources/" + filename;
				}
				else
				{
					resource_dir = Ti.Filesystem.resourcesDirectory;
					//var resource_dir = Ti.Filesystem.resourcesDirectory + Ti.Filesystem.separator;
					// Originally, I was getting a file://localhost/ at the beginning of the directory. 
					// This is a problem because ALmixer needs file paths compatible with the typical fopen type family.
					resource_dir = resource_dir.replace(/^file:\/\/localhost/g,'');
					// Later, Titanium started giving me URLs like file:// without the localhost. So this is a fallback string replacement.
					resource_dir = resource_dir.replace(/^file:\/\//g,'');
					// Replace %20 with spaces.
					resource_dir = resource_dir.replace(/%20/g,' ');
					filename = resource_dir + filename;
				}
			}
			// else???
		}
		return filename;
	}
	ALmixer.util.SanitizeFilePath = JSALmixerSanitizeFilePath;

	ALmixer._original.LoadAll = ALmixer.LoadAll;
	ALmixer.LoadAll = function(filename, access_data)
	{
		if(!access_data)
		{
			access_data = 0;
		}
		filename = JSALmixerSanitizeFilePath(filename);
		return ALmixer._original.LoadAll(filename, access_data);
	}

	ALmixer._original.LoadStream = ALmixer.LoadStream;
	ALmixer.LoadStream = function(filename, buffer_size, max_queue_buffers, num_startup_buffers, suggested_number_of_buffers_to_queue_per_update_pass, access_data)
	{
		if(!buffer_size)
		{
			buffer_size = 0;
		}
		if(!max_queue_buffers)
		{
			max_queue_buffers = 0;
		}
		if(!num_startup_buffers)
		{
			num_startup_buffers = 0;
		}
		if(!suggested_number_of_buffers_to_queue_per_update_pass)
		{
			suggested_number_of_buffers_to_queue_per_update_pass = 0;
		}
		if(!access_data)
		{
			access_data = 0;
		}
		filename = JSALmixerSanitizeFilePath(filename);
		return ALmixer._original.LoadStream(filename, buffer_size, max_queue_buffers, num_startup_buffers, suggested_number_of_buffers_to_queue_per_update_pass, access_data);
	}

	ALmixer._original.Quit = ALmixer.Quit;
	ALmixer.Quit = function()
	{
		// Because audio resources need to be cleaned up before Quit, we really need to try our best to force garbage collection.
		ALmixer.experimental.collectgarbage();
		ALmixer._original.Quit();
	}
	ALmixer._original.QuitWithoutFreeData = ALmixer.QuitWithoutFreeData;
	ALmixer.QuitWithoutFreeData = function()
	{
		// Because audio resources need to be cleaned up before Quit, we really need to try our best to force garbage collection.
		ALmixer.experimental.collectgarbage();
		ALmixer._original.QuitWithoutFreeData();
	}

	ALmixer._private = {};
	ALmixer._private.HandleALmixerSoundPlaybackFinished = JSALmixer_ALmixerSoundPlaybackFinished;
}

JSALmixer_Initialize();
