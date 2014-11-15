local s_LuaALmixerIsInitialized;
local s_LuaALmixerDataChannelTable;
-- I learned the hard way that the Ti Proxy must be held globally or the addEventListener for ALmixerSoundPlaybackFinished gets collected.

local playbackFinishedCallbackContainer;

-- Ugh: There is a race condition because the callbacks may be deferred.
-- Calling Halt immediatelly followed by play is legal in C because the ALmixer callback fires immediately.
-- But in a scripting language binding where the callback may be queued (e.g. thread redirection),
-- we can't assume that. 
-- The problem is that to deal with memory management, 
-- I'm keeping my own shadow table that keeps ALmixer_Data rooted and keeps mappings 
-- to scripting language defined callback functions.
-- If Play is immediately called after Halt(), but the callback hasn't fired yet,
-- I will accidentally overwrite the callback data that is supposed to fire with 
-- the next Play's callback data.
-- To avoid, I need to keep a queue of callback data which PushCallbackTable and ShiftCallbackTable provide.
function LuaALmixer_PushCallbackTable(which_channel, saved_table)
	if(not s_LuaALmixerDataChannelTable[which_channel])
	then
		s_LuaALmixerDataChannelTable[which_channel] = {};
	end
	-- table.insert(s_LuaALmixerDataChannelTable[which_channel], saved_table);
	local current_table = s_LuaALmixerDataChannelTable[which_channel]
	current_table[#current_table+1] = saved_table
end

function LuaALmixer_ShiftCallbackTable(which_channel)
	return table.remove(s_LuaALmixerDataChannelTable[which_channel])
end

	local function LuaALmixer_ALmixerSoundPlaybackFinished(al_source, finished_naturally)
		-- This will remove the callback table from the queue and return it.
		local which_channel = ALmixer.GetChannel(al_source);
		local saved_table = LuaALmixer_ShiftCallbackTable(which_channel);

		local callback_function = saved_table.onComplete;
		local sound_handle = saved_table.soundHandle;
		local event_table =
		{
			name="ALmixer",
			type="completed",
			channel=which_channel,
			alsource=al_source,
			finishedNaturally=finished_naturally,
			handle=saved_table.soundHandle,
			userData=saved_table.userData
		};

		-- Invoke user callback
		if(callback_function)
		then
			callback_function(event_table);
			event_table = nil;
			callback_function = nil;
		end
		event_table = nil;
		saved_table = nil;
	end



function LuaALmixer_Initialize()
	if(s_LuaALmixerIsInitialized)
	then
		return;
	end
	s_LuaALmixerIsInitialized = true;

	-- The SWIG binding isn't a proper module. Don't require because it doesn't follow packaing conventions.
	-- Load the file directly from C in a dofile fashion.
--	local ALmixer = require('ALmixer');
	-- Create a table to hold the original versions of the functions I intend to override.
	ALmixer._original = {};
	-- Create a table for utility/helper APIs
	ALmixer.util = {};
	-- Create a table for experiemental functions
	ALmixer.experimental = {};

	s_LuaALmixerDataChannelTable = {};

	ALmixer.experimental.collectgarbage = function()
		collectgarbage()
	end

	ALmixer._original.PlayChannelTimed = ALmixer.PlayChannelTimed;
	ALmixer.PlayChannelTimed = function(which_channel, sound_handle, num_loops, duration, on_complete, user_data)
		-- Allow which_channel to be omitted (automatically assumes -1).
		-- This means all parameters need to shift over
		if(type(which_channel) ~= "number")
		then
			-- Careful: order of the shift matters or we clobber a localiable we need to save
			user_data = on_complete;
			on_complete = duration;
			duration = num_loops;
			num_loops = sound_handle;
			sound_handle = which_channel;
			which_channel = -1;
		end

		local playing_channel = ALmixer._original.PlayChannelTimed(which_channel, sound_handle, num_loops, duration);
		-- Do only if playing succeeded
		if(playing_channel > -1)
		then
			-- Save the sound_handle to solve two binding problems.
			-- Problem 1: Garbage collection. The object looks like it disappears and resurrects later.
			-- We can't let the garbage collector collect the object while it is playing.
			-- So we need keep a strong reference around.
			-- Note: The sound could be playing on multiple channels, so we need to make sure we cover that case.
			-- Problem 2: ALmixer_Data for callbacks. Currently it is not clear how to get the ALmixer_Data pointer
			-- back to Javascript in a form that is usable using the Titanium callback system since it doesn't handle
			-- pointers and doesn't know anything about SWIG.
			-- The solution to both problems is to keep a global table mapping channels to sound_handles.
			-- For each playing channel, we hold a reference to the sound which keeps the object alive 
			-- (and handles the multiple channel case). 
			-- In the callback, we can retrieve the data by looking up the channel which solves the pointer problem.
			-- Problem 3: We would also like to make the API nicer for Javascript and let people pass in anonymous callback functions for each channel.
			-- This table structure will let us also keep around their callback.
			LuaALmixer_PushCallbackTable(playing_channel, { soundHandle=sound_handle, onComplete=on_complete, userData=user_data } );

		end
		return playing_channel, ALmixer.GetSource(playing_channel);
	end;

	ALmixer._original.PlayChannel = ALmixer.PlayChannel;
	ALmixer.PlayChannel = function(which_channel, sound_handle, num_loops, on_complete, user_data)
		-- Allow which_channel to be omitted (automatically assumes -1).
		-- This means all parameters need to shift over
		-- Assumption is that the following parameter MUST be the ALmixer_Data which is an object.
		if(type(which_channel) ~= "number")
		then
			-- Careful: order of the shift matters or we clobber a localiable we need to save
			user_data = on_complete;
			on_complete = num_loops;
			num_loops = sound_handle;
			sound_handle = which_channel;
			which_channel = -1;
		end

		-- Also allow num_loops to be omitted (automatically assumes 0).
		-- This means all parameters following need to shift over
		-- Note: Because the on_complete is optional, we explicitly test for the number and blindly shift.
		if(type(num_loops) ~= "number")
		then
			-- Careful: order of the shift matters or we clobber a localiable we need to save
			user_data = on_complete;
			on_complete = num_loops;
			num_loops = 0;
		end

		local playing_channel = ALmixer._original.PlayChannel(which_channel, sound_handle, num_loops);
		-- Do only if playing succeeded
		if(playing_channel > -1)
		then
			-- Save the sound_handle to solve two binding problems.
			-- Problem 1: Garbage collection. The object looks like it disappears and resurrects later.
			-- We can't let the garbage collector collect the object while it is playing.
			-- So we need keep a strong reference around.
			-- Note: The sound could be playing on multiple channels, so we need to make sure we cover that case.
			-- Problem 2: ALmixer_Data for callbacks. Currently it is not clear how to get the ALmixer_Data pointer
			-- back to Javascript in a form that is usable using the Titanium callback system since it doesn't handle
			-- pointers and doesn't know anything about SWIG.
			-- The solution to both problems is to keep a global table mapping channels to sound_handles.
			-- For each playing channel, we hold a reference to the sound which keeps the object alive 
			-- (and handles the multiple channel case). 
			-- In the callback, we can retrieve the data by looking up the channel which solves the pointer problem.
			-- Problem 3: We would also like to make the API nicer for Javascript and let people pass in anonymous callback functions for each channel.
			-- This table structure will let us also keep around their callback.
			LuaALmixer_PushCallbackTable(playing_channel, { soundHandle=sound_handle, onComplete=on_complete, userData=user_data } );
		end
		return playing_channel, ALmixer.GetSource(playing_channel);
	end;

	ALmixer._original.PlaySourceTimed = ALmixer.PlaySourceTimed;
	ALmixer.PlaySourceTimed = function(al_source, sound_handle, num_loops, duration, on_complete, user_data)
		-- Allow which_channel to be omitted (automatically assumes 0).
		-- This means all parameters need to shift over
		if(type(al_source) ~= "number")
		then
			-- Careful: order of the shift matters or we clobber a localiable we need to save
			user_data = on_complete;
			on_complete = duration;
			duration = num_loops;
			num_loops = sound_handle;
			sound_handle = al_source;
			al_source = 0;
		end

		local playing_source = ALmixer._original.PlaySourceTimed(al_source, sound_handle, num_loops, duration);
		-- Do only if playing succeeded
		if(playing_source > 0)
		then
			-- convert source to channel
			local playing_channel = ALmixer.GetChannel(playing_source);
			
			-- Save the sound_handle to solve two binding problems.
			-- Problem 1: Garbage collection. The object looks like it disappears and resurrects later.
			-- We can't let the garbage collector collect the object while it is playing.
			-- So we need keep a strong reference around.
			-- Note: The sound could be playing on multiple channels, so we need to make sure we cover that case.
			-- Problem 2: ALmixer_Data for callbacks. Currently it is not clear how to get the ALmixer_Data pointer
			-- back to Javascript in a form that is usable using the Titanium callback system since it doesn't handle
			-- pointers and doesn't know anything about SWIG.
			-- The solution to both problems is to keep a global table mapping channels to sound_handles.
			-- For each playing channel, we hold a reference to the sound which keeps the object alive 
			-- (and handles the multiple channel case). 
			-- In the callback, we can retrieve the data by looking up the channel which solves the pointer problem.
			-- Problem 3: We would also like to make the API nicer for Javascript and let people pass in anonymous callback functions for each channel.
			-- This table structure will let us also keep around their callback.
			LuaALmixer_PushCallbackTable(playing_channel, { soundHandle=sound_handle, onComplete=on_complete, userData=user_data } );
		end
		return playing_source, ALmixer.GetChannel(playing_source);
	end;

	ALmixer._original.PlaySource = ALmixer.PlaySource;
	ALmixer.PlaySource = function(al_source, sound_handle, num_loops, on_complete, user_data)
		-- Allow which_channel to be omitted (automatically assumes 0).
		-- This means all parameters need to shift over
		if(type(al_source) ~= "number")
		then
			-- Careful: order of the shift matters or we clobber a localiable we need to save
			user_data = on_complete;
			on_complete = num_loops;
			num_loops = sound_handle;
			sound_handle = al_source;
			al_source = 0;
		end

		-- Also allow num_loops to be omitted (automatically assumes 0).
		-- This means all parameters following need to shift over
		-- Note: Because the on_complete is optional, we explicitly test for the number and blindly shift.
		if(type(num_loops) ~= "number")
		then
			-- Careful: order of the shift matters or we clobber a localiable we need to save
			user_data = on_complete;
			on_complete = num_loops;
			num_loops = 0;
		end

		local playing_source = ALmixer._original.PlaySource(al_source, sound_handle, num_loops);
		-- Do only if playing succeeded
		if(playing_source > 0)
		then
			-- convert source to channel
			local playing_channel = ALmixer.GetChannel(playing_source);
			
			-- Save the sound_handle to solve two binding problems.
			-- Problem 1: Garbage collection. The object looks like it disappears and resurrects later.
			-- We can't let the garbage collector collect the object while it is playing.
			-- So we need keep a strong reference around.
			-- Note: The sound could be playing on multiple channels, so we need to make sure we cover that case.
			-- Problem 2: ALmixer_Data for callbacks. Currently it is not clear how to get the ALmixer_Data pointer
			-- back to Javascript in a form that is usable using the Titanium callback system since it doesn't handle
			-- pointers and doesn't know anything about SWIG.
			-- The solution to both problems is to keep a global table mapping channels to sound_handles.
			-- For each playing channel, we hold a reference to the sound which keeps the object alive 
			-- (and handles the multiple channel case). 
			-- In the callback, we can retrieve the data by looking up the channel which solves the pointer problem.
			-- Problem 3: We would also like to make the API nicer for Javascript and let people pass in anonymous callback functions for each channel.
			-- This table structure will let us also keep around their callback.
			LuaALmixer_PushCallbackTable(playing_channel, { soundHandle=sound_handle, onComplete=on_complete, userData=user_data } );
		end
		return playing_source, ALmixer.GetChannel(playing_source);
	end;


	function LuaALmixerPlaySound(sound_handle, options_table)
		local which_channel = -1;
		local al_source = 0;
		local num_loops = 0;
		local duration = -1;
		local on_complete = nil;
		local user_data = nil;
		local using_source_instead_of_channel = false;
		if(options_table)
		then
			if(options_table.channel)
			then
				which_channel = options_table.channel;
			elseif(options_table.alsource)
			then
				which_channel = ALmixer.GetChannel(options_table.alsource);
				using_source_instead_of_channel = true;
			end


			if(options_table.loops)
			then
				num_loops = options_table.loops;
			end 
			if(options_table.duration)
			then
				duration = options_table.duration;
			end
			if(options_table.onComplete)
			then
				on_complete = options_table.onComplete;
			end
			if(options_table.userData)
			then
				user_data = options_table.userData;
			end
		end
		-- Could call overridden version and omit code instead.
		local playing_channel = ALmixer._original.PlayChannelTimed(which_channel, sound_handle, num_loops, duration);
		-- Do only if playing succeeded
		if(playing_channel > -1)
		then
			-- Save the sound_handle to solve two binding problems.
			-- Problem 1: Garbage collection. The object looks like it disappears and resurrects later.
			-- We can't let the garbage collector collect the object while it is playing.
			-- So we need keep a strong reference around.
			-- Note: The sound could be playing on multiple channels, so we need to make sure we cover that case.
			-- Problem 2: ALmixer_Data for callbacks. Currently it is not clear how to get the ALmixer_Data pointer
			-- back to Javascript in a form that is usable using the Titanium callback system since it doesn't handle
			-- pointers and doesn't know anything about SWIG.
			-- The solution to both problems is to keep a global table mapping channels to sound_handles.
			-- For each playing channel, we hold a reference to the sound which keeps the object alive 
			-- (and handles the multiple channel case). 
			-- In the callback, we can retrieve the data by looking up the channel which solves the pointer problem.
			-- Problem 3: We would also like to make the API nicer for Javascript and let people pass in anonymous callback functions for each channel.
			-- This table structure will let us also keep around their callback.
			LuaALmixer_PushCallbackTable(playing_channel, { soundHandle=sound_handle, onComplete=on_complete, userData=user_data } );
		end

		-- I wish Javascript had multiple return values
		if(using_source_instead_of_channel)
		then
			local playing_source = 0;
			if(playing_channel > -1)
			then
				playing_source = ALmixer.GetSource(playing_channel);
			end
			return playing_channel, playing_source;
		else
			return playing_channel, ALmixer.GetSource(playing_channel);
		end
	end

	ALmixer.util.Play = LuaALmixerPlaySound;


	function LuaALmixerSanitizeFilePath(filename)
		local resource_dir;

		--[[ There are several cases:
		 -- User passed an absolute path starting with /
		 -- User passed some kind of URL
		 -- User passed a relative path (which we will interpret as looking in the platform's default resource directory)
		 --]]

		 local function string_starts_with(the_string, start_characters)
			 return string.sub(the_string, 1, string.len(start_characters))==start_characters
		 end

		-- http://stackoverflow.com/questions/646628/javascript-startswith, see Mark Byers
		if(string_starts_with(filename, "/"))
		then
			-- do nothing and keep the filename exactly as it was
		-- I don't know all the permutations, but my Titanium encounters so far start with file:
		--[[
		elseif(filename.lastIndexOf("file:", 0) === 0)
		then
			-- Originally, I was getting a file:--localhost/ at the beginning of the directory. 
			-- This is a problem because ALmixer needs file paths compatible with the typical fopen type family.
			--filename = filename.replace(/^file:\/\/localhost/g,'');
			-- Later, Titanium started giving me URLs like file:-- without the localhost. So this is a fallback string replacement.
			--filename = filename.replace(/^file:\/\--g,'');
			-- Replace %20 with spaces.
			--filename = filename.replace(/%20/g,' ');
			--]]
		else
			-- In Titanium, for Android, the relative path must be prepended with Resources/
			-- Mac/iOS should manually compose the NSBundle resourcePath.
			-- else???
		end
		return filename;
	end
	ALmixer.util.SanitizeFilePath = LuaALmixerSanitizeFilePath;

	ALmixer._original.LoadAll = ALmixer.LoadAll;
	ALmixer.LoadAll = function(filename, access_data)
		if(not access_data)
		then
			access_data = 0;
		end
		filename = LuaALmixerSanitizeFilePath(filename);
		return ALmixer._original.LoadAll(filename, access_data);
	end

	ALmixer._original.LoadStream = ALmixer.LoadStream;
	ALmixer.LoadStream = function(filename, buffer_size, max_queue_buffers, num_startup_buffers, suggested_number_of_buffers_to_queue_per_update_pass, access_data)
		if(not buffer_size)
		then
			buffer_size = 0;
		end
		if(not max_queue_buffers)
		then
			max_queue_buffers = 0;
		end
		if(not num_startup_buffers)
		then
			num_startup_buffers = 0;
		end
		if(not suggested_number_of_buffers_to_queue_per_update_pass)
		then
			suggested_number_of_buffers_to_queue_per_update_pass = 0;
		end
		if(not access_data)
		then
			access_data = 0;
		end
		filename = LuaALmixerSanitizeFilePath(filename);
		return ALmixer._original.LoadStream(filename, buffer_size, max_queue_buffers, num_startup_buffers, suggested_number_of_buffers_to_queue_per_update_pass, access_data);
	end

	ALmixer._original.Quit = ALmixer.Quit;
	ALmixer.Quit = function()
		-- Because audio resources need to be cleaned up before Quit, we really need to try our best to force garbage collection.
		ALmixer.experimental.collectgarbage();
		ALmixer._original.Quit();
	end
	ALmixer._original.QuitWithoutFreeData = ALmixer.QuitWithoutFreeData;
	ALmixer.QuitWithoutFreeData = function()
		-- Because audio resources need to be cleaned up before Quit, we really need to try our best to force garbage collection.
		ALmixer.experimental.collectgarbage();
		ALmixer._original.QuitWithoutFreeData();
	end

	ALmixer._private = {};
	ALmixer._private.HandleALmixerSoundPlaybackFinished = LuaALmixer_ALmixerSoundPlaybackFinished;

end

LuaALmixer_Initialize();
