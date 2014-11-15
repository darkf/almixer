#!/bin/sh

mkdir buildandroidcmake
cd buildandroidcmake
rm -rf luaal
../AndroidCMake/genproj_android.pl --blurrrsdkpath=~/Source/Blurrr/Release/Blurrr_Apple_DP1/Libraries/Android/SDK/Lua_f32_i32  --standalonetoolchainroot=$ANDROID_NDK_ROOT/standalone ..
./make_ndk.sh
mv dist luaal

