cmake_minimum_required(VERSION 3.31)

project(breakout-app-android)

add_subdirectory(../lib ./lib-output)

add_library(breakout-android SHARED
	breakout-android.cpp
	${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
)

target_include_directories(
	breakout-android
	PRIVATE
	${ANDROID_NDK}/sources/android/native_app_glue
)

target_link_libraries(
	breakout-android
    android
#    native_app_glue
	log
)
