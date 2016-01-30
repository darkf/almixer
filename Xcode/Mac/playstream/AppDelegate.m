//
//  AppDelegate.m
//  playstream
//
//  Created by Eric Wing on 1/2/13.
//  Copyright (c) 2013 PlayControl Software LLC. All rights reserved.
//
// This example demonstrates how to integrate ALmixer directly within a typical native app.
// It is assumed ALmixer has been compiled with threads so you don't have to explicitly pump the update loop for ALmixer.
// As a consequence, rememember that ALmixer callbacks may happen on a background thread.
// (Currently, HaltChannel() callbacks typically happens on the calling thread, while everything else happens on a background thread.)
// If you do anything NSObject related in the callback, you will want to set up an autoreleasepool for that thread.
// If you do anything UI related, you'll probably want to re-direct back to the main thread as much of view stuff is not thread safe in Cocoa.

#import "AppDelegate.h"
#include "ALmixer.h"

@interface AppDelegate ()
@property(nonatomic, assign, readonly) ALmixer_Data* musicHandle;
@end


@implementation AppDelegate
@synthesize musicHandle = _musicHandle;

- (void) dealloc
{
	ALmixer_HaltChannel(-1);
	ALmixer_FreeData(_musicHandle);
    [super dealloc];
}

- (void) applicationDidFinishLaunching:(NSNotification*)the_notification
{
	// Insert code here to initialize your application
	ALmixer_Init(0, 0, 0);
	NSString* music_file = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"battle_hymn_of_the_republic.mp3"];
	_musicHandle = ALmixer_LoadStream([music_file fileSystemRepresentation], 0, 0, 0, 0, AL_FALSE);
	
	if(ALmixer_IsPredecoded(_musicHandle))
	{
		NSLog(@"oops, predecoded. Your sample is too short to test LoadStream. It got promoted into a LoadAll");
		assert(1);
	}
	else
	{
		NSLog(@"good to go");
	}
	
	
	ALmixer_PlayChannel(-1, _musicHandle, 1);

}

- (BOOL) applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)the_application
{
	return YES;
}

@end
