cmake_minimum_required(VERSION 4.0.3)

project(toms-lane VERSION 1.0)

add_library(
	toms-lane-win32-application STATIC
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
target_link_libraries(
	toms-lane-win32-application
	user32
	winmm
	gdi32
)


add_executable(
	toms-lane-win32-application-tests
	src/application/win32/win32-tests.cpp
)
target_link_libraries(
	toms-lane-win32-application-tests
	toms-lane-win32-application
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

#[[
add_executable(
	breakout-win32
	src/breakout/breakout.cpp
	src/breakout/render.cpp
	src/breakout/update_state.cpp
)
target_link_libraries(
	breakout-win32
	toms-lane-platform
	toms-lane-win32-application
)
]]
