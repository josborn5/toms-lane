cmake_minimum_required(VERSION 4.0.3)

project(toms-lane VERSION 1.0)


add_library(
	toms-lane-win32-application STATIC
	src/application/win32/win32-tests.cpp
	src/application/win32/win32-input.cpp
	src/application/win32/win32-window.cpp
	src/application/win32/win32-file.cpp
	src/application/win32/win32-time.cpp
	src/application/win32/win32-console.cpp
	src/application/win32/win32-winmmsound.cpp
	src/application/win32/win32-memory.cpp
	src/application/win32/win32-application.cpp
	src/application/win32/win32-text.cpp
)


add_library(
	toms-lane-platform STATIC
	src/platform/toms-lane-platform.cpp
	src/platform/font.cpp
	src/platform/transform.cpp
)

add_executable(
	toms-lane-platform-test
	src/platform/toms-lane-platform.tests.cpp
)

target_link_libraries(
	toms-lane-platform-test
	toms-lane-platform
	toms-lane-win32-application
)

