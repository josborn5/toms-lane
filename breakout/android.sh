config=Release

ANDROID_NDK=$HOME/Android/Sdk/ndk/28.2.13676358

cmake \
	-DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
	-DCMAKE_BUILD_TYPE=$config \
	-DANDROID_NDK=$ANDROID_NDK \
	-DANDROID_ABI= arm64-v8a \
	-DANDROID_PLATFORM=android-24 \
	-S android \
	-B android/cmake-build \
|| exit 1

cmake \
	--build android/cmake-build \
	--config $config \
|| exit 1

