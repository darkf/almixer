//
//  AppDelegate.m
//  playstream
//
//  Created by Eric Wing on 10/10/12.
//  Copyright (c) 2012 PlayControl Software, LLC. All rights reserved.
//
// This example demonstrates how to integrate ALmixer directly within a typical native app.
// It is assumed ALmixer has been compiled with threads so you don't have to explicitly pump the update loop for ALmixer.
// As a consequence, rememember that ALmixer callbacks may happen on a background thread.
// (Currently, HaltChannel() callbacks typically happens on the calling thread, while everything else happens on a background thread.)
// If you do anything NSObject related in the callback, you will want to set up an autoreleasepool for that thread.
// If you do anything UI related, you'll probably want to re-direct back to the main thread as much of view stuff is not thread safe in Cocoa.

#import "AppDelegate.h"

#import "ViewController.h"
#include "ALmixer.h"
#import <AVFoundation/AVFoundation.h>


@interface AppDelegate ()
@property(nonatomic, assign, readonly) ALmixer_Data* musicHandle;
@end

@implementation AppDelegate

@synthesize window = _window;
@synthesize viewController = _viewController;
@synthesize musicHandle = _musicHandle;

- (void)dealloc
{
	ALmixer_HaltChannel(-1);
	ALmixer_FreeData(_musicHandle);
	[_window release];
	[_viewController release];
    [super dealloc];
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    self.window = [[[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]] autorelease];
    // Override point for customization after application launch.
	if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
	    self.viewController = [[[ViewController alloc] initWithNibName:@"ViewController_iPhone" bundle:nil] autorelease];
	} else {
	    self.viewController = [[[ViewController alloc] initWithNibName:@"ViewController_iPad" bundle:nil] autorelease];
	}
	self.window.rootViewController = self.viewController;
    [self.window makeKeyAndVisible];
	
	NSError* the_error = nil;
//	[[AVAudioSession sharedInstance] setActive:NO withOptions:AVAudioSessionSetActiveOptionNotifyOthersOnDeactivation error:&the_error];
	[[AVAudioSession sharedInstance] setActive:YES error:&the_error];
	if(nil != the_error)
	{
		NSLog(@"Error setting AudioSession active: %@\n", [the_error localizedDescription]);
	}
	// iOS 6 changed the way to handle audio sessions and the old way is deprecated. (I'm not going to bother with the pre-iOS 6 way in this example.)
	// What is useful to library writers is that the iOS 6+ notification center based interruption system can now deliver messages to multiple independent listeners that don't need to know about each other. (The old system could only have one global listener and would clobber each other if different frameworks.
	// What is still unfortunate is that in order to get these notifications, the AudioSession must be setActive:YES,
	// and there is still no way to "getActive" (my repeated feature requests are closed).
	// For this example, this is not a problem, but it is a problem for library writers because you may need to defer
	// to another module that is already manipulating the audio sessions and you don't want to fight them.
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(handleAudioInterruption:) name:AVAudioSessionInterruptionNotification object:nil];
	NSLog(@"application launch\n");

	
	ALmixer_Init(0, 0, 0);
	NSString* music_file = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"battle_hymn_of_the_republic.mp3"];
	_musicHandle = ALmixer_LoadStream([music_file fileSystemRepresentation], 0, 0, 0, 0, AL_FALSE);
	
	ALmixer_PlayChannel(-1, _musicHandle, 0);

	
	
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
	// Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
	// Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
	// Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
	// If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
	// Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
	// Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application
{
	// Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
	[[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void) handleAudioInterruption:(NSNotification*)the_notification
{
	NSNumber* ns_interruption_type = [[the_notification userInfo] objectForKey:AVAudioSessionInterruptionTypeKey];
	NSUInteger interruption_type = [ns_interruption_type unsignedIntegerValue];
	NSLog(@"handleAudioInterruption\n");

	switch(interruption_type)
	{
		case AVAudioSessionInterruptionTypeBegan:
		{
			NSLog(@"Begin Interruption\n");

			ALmixer_BeginInterruption();
			break;
		}
		case AVAudioSessionInterruptionTypeEnded:
		{
			NSLog(@"End Interruption\n");
			ALmixer_EndInterruption();
			break;
		}
		default:
		{
			NSLog(@"Unexpected type received in handleAudioInterruption\n");
		}
	}
}

@end
