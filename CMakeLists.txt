cmake_minimum_required(VERSION 4.0.3)

project(toms-lane VERSION 1.0)


# paths are relative to the build dir (i.e. ./cmake-build
function(xxd input_path output_path)
add_custom_command(
	OUTPUT output_path
	COMMAND xxd -i input_path > output_path
	DEPENDS input_path
	COMMENT "Generatrion ${output_path} from ${input_path}"
)
endfunction()


add_library(
	toms-lane-win32-application STATIC
	src/application/win32/win32-input.cpp
	src/application/win32/win32-window.cpp
	src/application/win32/win32-file.cpp
	src/application/win32/win32-time.cpp
	src/application/win32/win32-console.cpp
	src/application/win32/win32-winmmsound.cpp
	src/application/win32/win32-memory.cpp
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


add_executable(
	sprite-editor-win32 WIN32
	src/sprite-editor/sprite-editor-win32.cpp
)
add_executable(
	sprite-editor-tests
	src/sprite-editor/sprite-editor.test.cpp
)
add_library(
	sprite-editor STATIC
	src/sprite-editor/commands.cpp
	src/sprite-editor/state.cpp
	src/sprite-editor/render.cpp
	src/sprite-editor/palettes.cpp
	src/sprite-editor/sprite-editor.cpp
	src/sprite-editor/utilities.cpp
	src/sprite-editor/transform.cpp
	src/sprite-editor/operations.cpp
)
target_link_libraries(
	sprite-editor-tests
	sprite-editor
)
target_link_libraries(
	sprite-editor-win32
	sprite-editor
	toms-lane-platform
	toms-lane-win32-application
)


add_executable(
	breakout-win32 WIN32
	src/breakout/breakout-win32.cpp
)
add_library(
	breakout STATIC
	src/breakout/breakout.cpp
	src/breakout/render.cpp
	src/breakout/update_state.cpp
)
target_link_libraries(
	breakout-win32
	breakout
	toms-lane-platform
	toms-lane-win32-application
)



xxd(../src/obby-1/brick.bmp ../src/obby-1/brick.cpp)
xxd(../src/obby-1/checkout.bmp ../src/obby-1/checkout.cpp)
xxd(../src/obby-1/player.bmp ../src/obby-1/player.cpp)
add_library(
	obby-1
	src/obby-1/brick.cpp
	src/obby-1/checkpoint.cpp
	src/obby-1/player.cpp
)
