#!/usr/bin/perl -w

###########################################################
# 
# Convenince script to generate CMake projects for Android 
# for each architecture and build them.
# Copyright (C) PlayControl Software, LLC. 
# Eric Wing <ewing . public @ playcontrol.net>
#
# Convention:
# You have created standalone toolchains and placed them in a directory called standalone under the $ANDROID_NDK_ROOT:
# $ANDROID_NDK_ROOT/standalone/
#	arm-linux-androideabi-4.6/	
# 	x86-4.6/
# 	
##########################################################


use strict;
use warnings;

# Function to help with command line switches
use Getopt::Long;
# Allows extra "unknown options" to be specified which I will use to pass directly to the cmake executable.
Getopt::Long::Configure("pass_through");

# Function to get the basename of a file
use File::Basename;
# Used for tilde expansion
use File::Glob;
# for make_path (which is mkdir -p)
use File::Path qw(make_path);

# Provides functions to convert relative paths to absolute paths.
use Cwd;


# Global constants

my $kCMakeBootStrapCacheFile = "InitialCache_Android.cmake";

my %kArchToDirectoryNameMap =
(
#	mips => "mips",
	armeabi => "armeabi",
	"armeabi-v7a" => "armeabi-v7a",
#	"armeabi-v7a with NEON" => "armeabi-v7a",
#	"armeabi-v7a with VFPV3" => "armeabi-v7a",
#	"armeabi-v6 with VFP" => "armeabi",
	x86 => "x86"
);

# Global constants
my %kArchToCompilerNameMap =
(
#	mips => "mips",
	armeabi => "arm-linux-androideabi",
	"armeabi-v7a" => "arm-linux-androideabi",
#	"armeabi-v7a with NEON" => "armeabi",
#	"armeabi-v7a with VFPV3" => "armeabi",
#	"armeabi-v6 with VFP" => "armeabi",
	x86 => "x86"
);


my @kSupportedArchitectures =
(
#	"mips",
	"armeabi",
	"armeabi-v7a",
#	"armeabi-v7a with NEON",
#	"armeabi-v7a with VFPV3",
#	"armeabi-v6 with VFP",
	"x86",
);


# Function prototypes 

# main routine
sub main();
# call main
main();

sub main()
{
	my ($targetdir, $standalone, $compilerversion, $should_build, $cmake, $toolchain, $libsdir, $buildtype, $sourcedir, $openalbasedir, $blurrr_sdk_path, @remaining_options) = extract_parameters();

	my $openal_include_dir = $openalbasedir . "/jni/OpenAL/include/AL";
	my $openal_include_dir_option = "-DOPENAL_INCLUDE_DIR=" . $openal_include_dir;

	# Save in case we need to return to the original current working directory.
#	my $original_current_working_directory = Cwd::cwd();

#	print("targetdir: ", $targetdir, "\n"); 
#	print("cmake: ", $cmake, "\n"); 
#	print("toolchain: ", $toolchain, "\n"); 
#	print("buildtype: ", $buildtype, "\n"); 
#	print("sourcedir: ", $sourcedir, "\n"); 
#	print("remaining_options: ", @remaining_options, "\n"); 

	foreach my $arch(@kSupportedArchitectures)
	{
		# First choose the correct compiler.
		my $found_compiler;
		my $compiler_base_name = $kArchToCompilerNameMap{$arch};
		
		opendir(STANDALONEDIR, $standalone) or die("Could not open standalone directory: $!\n");
    	while(my $file = readdir(STANDALONEDIR))
		{
			my $full_path_and_file = "$standalone/$file";

			# Go to the next file unless it is a directory
			next unless(-d "$full_path_and_file");

			# if a version was specified, make sure it matches
			if(defined($compilerversion))
			{
				if($file =~ m/$compiler_base_name-$compilerversion/)
				{
					$found_compiler = $full_path_and_file;
					last;
				}
			}
			# otherwise if no version was specified, just go for any match
			else
			{
				if($file =~ m/$compiler_base_name/)
				{
					$found_compiler = $full_path_and_file;
					last;
				}
			}
		}
		closedir(STANDALONEDIR);

		if(not defined $found_compiler)
		{
			die("Could not find compiler in directory:$standalone for arch:$arch\n");
		}



		chdir($targetdir) or die("Could not change directory to $targetdir: $!\n");
		my $arch_dir = $kArchToDirectoryNameMap{$arch};
		unless(-e $arch_dir or mkdir $arch_dir)
		{
			die("Unable to create $arch_dir: $!\n");
		}
		chdir($arch_dir) or die("Could not change directory to $arch_dir: $!\n");

		my $arch_flag = "-DANDROID_ABI=$arch";
		my $android_standalone_toolchain = "-DANDROID_STANDALONE_TOOLCHAIN=$found_compiler";
#		print("arch_flag: $arch_flag\n");
		print("Generating $arch\n");

		my $openal_lib_dir = $openalbasedir . "/libs/" . $arch . "/libopenal.so";
		my $openal_lib_dir_option = "-DOPENAL_LIBRARY=" . $openal_lib_dir;

		my $initial_cache = "$sourcedir/android/CMakeSupport/$kCMakeBootStrapCacheFile";

		
		print("Executing: $cmake $toolchain $android_standalone_toolchain -DBLURRR_SDK_PATH=$blurrr_sdk_path $arch_flag -C $initial_cache $libsdir $buildtype, $openal_include_dir_option $openal_lib_dir_option @remaining_options $sourcedir\n");
		my $error_status = system($cmake, $toolchain, $android_standalone_toolchain, 
			"-DBLURRR_SDK_PATH=$blurrr_sdk_path", 
			$arch_flag, 
			"-DBLURRR_CMAKE_ANDROID_REAL_BINARY_DIR=$targetdir",
			"-C", $initial_cache, 
			$libsdir, $buildtype, $openal_include_dir_option, $openal_lib_dir_option, @remaining_options, $sourcedir);
		if($error_status != 0)
		{
			die "Invoking CMake failed: $?\n";
		}

		if($should_build)
		{
			print("Building $arch\n");
			$error_status = system("make");
			if($error_status != 0)
			{
				die "Invoking make failed: $?\n";
			}
		}
	}

	return;

}


sub helpmenu()
{
	my $basename = basename($0);
	print "Convenience script for generating and building CMake based projects for Android (multiple architectures).\n\n";
	print "Convention:\n";
	print "You have created standalone toolchains and placed them in a directory called standalone under the \$ANDROID_NDK_ROOT:\n";
	print "\$ANDROID_NDK_ROOT/standalone/\n";
	print "\tarm-linux-androideabi-4.6/\n";
	print "\tx86-4.6/\n";

	print "Usage: perl $basename [-h | -help] --sourcedir=<path to source> --targetdir=<path to build dir> --toolchain=<CMake toolchain file> [--standalone=<standalone root directory>] [--cmake=<CMake exectuable>] [--buildtype=<None|Debug|Release*|RelWithDebInfo|MinSizeRel>] [<other flags passed to CMake>]\n";

	print "Options:\n";
	print "  -h or -help                              Brings up this help display.\n";
	print "  --sourcedir=<path to source>             Path to the source code directory.\n";
	print "  --targetdir=<path to build directory>    Path to where the CMake projects will be generated. Will be created if doesn't exist.\n";
	print "  --openalbasedir=<path to OpenAL dir>     Path to where the OpenAL base directory is.\n";
	print "  --toolchain=<toolchain file>             Path to and file of the CMake toolchain to use.\n";
	print "  --libsdir=<path where libs are copied>   (Optional) Path where the built libs are placed. Default is same as the targetdir.\n";
	print "  --standalone=<standalone root directory> (Optional) Allows you to specify the path to the standalone directory containing your compilers. Default looks in \$ANDROID_NDK_ROOT/standalone.\n";
	print "  --compilerversion=<version>              (Optional) Allows you to specify the version number (4.6) of the compiler to disambiguate if you have multiple versions.\n";
	print "  --cmake=<CMake executable>               (Optional) Allows you to specify the path and file to the CMake executable.\n";
	print "  --buildtype=<build type>                 (Optional) The CMake Build Type. Default is Release.\n";
	print "  --[no]build                              (Optional) Specifies whether make should be invoked.\n";
	print "  --blurrrsdkpath=<path>   	   		      Path to the SDK to use, e.g. ~/Blurrr/Libraries/Android/SDK/Lua_f32_i32. You may define this in an environmental variable. This acts as an overrride to BLURRR_ROOT.\n";
	print "\n";
	print "Example Usage:\n";
	print "$basename --sourcedir=../Chipmunk2D/ --targetdir=. --toolchain=~/Source/HG/android-cmake/toolchain/android.toolchain.cmake\n";

	return;
}

sub home_dir() 
{
	return File::Glob::bsd_glob("~");
}

sub expand_tilde($)
{
	my $path = shift;
	my $home_dir = home_dir();

	$path =~ s/^~/$home_dir/;
	return $path;
}

sub absolute_path($)
{
	my $file = shift;
	return Cwd::abs_path(expand_tilde($file));
}


# Subroutine to extract and process command line parameters
sub extract_parameters()
{
	my %params = (
		h => \(my $hflag = 0),
		help => \(my $helpflag = 0),
		sourcedir => \(my $sourcedir),
		openalbasedir => \(my $openalbasedir),
		targetdir => \(my $targetdir),
		libsdir => \(my $libsdir),
		toolchain => \(my $toolchain),
		compilerversion => \(my $compilerversion),
		buildtype => \(my $buildtype = "Release"),
		standalone => \(my $standalone),
		build => \(my $should_build = 1),
		blurrrsdkpath => \(my $blurrr_sdk_path), # acts as an override for blurrr_sdk_root
		cmake => \(my $cmake)
       );

	# Call Library function which will extract and remove all switches and
	# their corresponding values.
	# These parameters will be removed from @ARGV
	my $errorval = &GetOptions(\%params, "h", "help",
					"blurrrsdkpath=s",
					"sourcedir=s",
					"openalbasedir=s",
					"targetdir=s",
					"libsdir=s",
					"toolchain=s",
					"compilerversion=s",
					"buildtype=s",
					"standalone=s",
					"build!",
					"cmake=s"
	); 
	# the exclaimation point allows for the negation
	# of the switch (i.e. -nobackup/-nobody is a switch)

	# Error value should have returned 1 if nothing went wrong
	# Otherwise, an unlisted parameter was specified.
	if($errorval !=1)
	{
		# Expecting GetOptions to state error.

		print "Exiting Program...\n";
		exit 0;
	}

	if( ($hflag == 1) || ($helpflag == 1) ) 
	{
		helpmenu();
		exit 0;
	}

	if(not defined($sourcedir))
	{
		helpmenu();
		exit 1;
	}

	if(not defined($targetdir))
	{
		helpmenu();
		exit 2;
	}

	if(not defined($toolchain))
	{
		helpmenu();
		exit 3;
	}

	if(not defined($libsdir))
	{
		$libsdir = $targetdir;
	}

	if(not defined($standalone))
	{
		$standalone = $ENV{"ANDROID_NDK_ROOT"};
		if(not defined($standalone))
		{
			print("standalone root directory not specified or environmental variable ANDROID_NDK_ROOT not defined\n");
			helpmenu();
			exit 4;
		}
		else
		{
			$standalone = $standalone . "/standalone";
		}
	}



	if(not defined($cmake))
	{
		$cmake = "cmake";
	}
	else
	{
		$cmake = absolute_path($cmake);	
	}
	# Convert to absolute paths because we will be changing directories which will break relative paths.
	$sourcedir = absolute_path($sourcedir);

	# We need to create the target directory if it doesn't exist before calling absolute_path() on it.
	unless(-e $targetdir or make_path($targetdir))
	{
		die("Unable to create $targetdir: $!\n");
	}

	$targetdir = absolute_path($targetdir);
	$standalone = absolute_path($standalone);

	if(not defined($openalbasedir))
	{
		$openalbasedir = "./openal-soft";
	}
	$openalbasedir = absolute_path($openalbasedir);

	# Change the strings to be in the form we need to pass to CMake.
	$toolchain = "-DCMAKE_TOOLCHAIN_FILE=" . absolute_path($toolchain);
	$libsdir = "-DLIBRARY_OUTPUT_PATH_ROOT=" . absolute_path($libsdir);
	$buildtype = "-DCMAKE_BUILD_TYPE=$buildtype";

	if(not defined($blurrr_sdk_path))
	{
		$blurrr_sdk_path = $ENV{"BLURRR_SDK_PATH"};
		if(not defined($blurrr_sdk_path))
		{
			$blurrr_sdk_path = undef;
		}
	}
	else
	{
		$blurrr_sdk_path = absolute_path($blurrr_sdk_path);
	}



	# This can be optimized out, but is left for clarity. 
	# GetOptions has removed all found options so anything left in @ARGV is "remaining".
	my @remaining_options = @ARGV;

	my @sorted_options = ($targetdir, $standalone, $compilerversion, $should_build, $cmake, $toolchain, $libsdir, $buildtype, $sourcedir, $openalbasedir, $blurrr_sdk_path, @remaining_options);
	
	return @sorted_options;
}


