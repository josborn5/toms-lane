cmake_minimum_required(VERSION 3.31.0)

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


function(generate_platform_asset input_file output_file)
	file(REMOVE "${CMAKE_SOURCE_DIR}/src/platform/${output_file}")
	add_custom_command(
		OUTPUT "${CMAKE_SOURCE_DIR}/src/platform/${output_file}"
		COMMAND xxd -i "${input_file}" > "${output_file}"
		DEPENDS "${CMAKE_SOURCE_DIR}/src/platform/${input_file}"
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/src/platform"
	)
endfunction()
generate_platform_asset(font-mono.tlsf font-mono.cpp)
generate_platform_asset(test.bmp test.cpp)

add_library(
	toms-lane-platform STATIC
	src/platform/toms-lane-platform.cpp
	src/platform/font-mono.cpp
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


######## sprite-editor ########
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


######## breakout ########
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


######## obby-1 ########
function(generate_obby_1_asset name)
	file(REMOVE "${CMAKE_SOURCE_DIR}/src/obby-1/${name}.cpp")
	add_custom_command(
		OUTPUT "${CMAKE_SOURCE_DIR}/src/obby-1/${name}.cpp"
		COMMAND xxd -i "${name}.bmp" > "${name}.cpp"
		DEPENDS "${CMAKE_SOURCE_DIR}/src/obby-1/${name}.bmp"
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/src/obby-1"
	)
endfunction()

generate_obby_1_asset("brick")
generate_obby_1_asset("checkpoint")
generate_obby_1_asset("player")

add_library(
	obby-1
	src/obby-1/obby-1.cpp
	src/obby-1/brick.cpp
	src/obby-1/checkpoint.cpp
	src/obby-1/player.cpp
)
add_executable(
	obby-1-win32 WIN32
	src/obby-1/obby-1-win32.cpp
)
target_link_libraries(
	obby-1-win32
	obby-1
	toms-lane-platform
	toms-lane-win32-application
)


######## obby-2 ########
function(generate_obby_2_asset name)
	file(REMOVE "${CMAKE_SOURCE_DIR}/src/obby-2/${name}.cpp")
	add_custom_command(
		OUTPUT "${CMAKE_SOURCE_DIR}/src/obby-2/${name}.cpp"
		COMMAND xxd -i "${name}.bmp" > "${name}.cpp"
		DEPENDS "${CMAKE_SOURCE_DIR}/src/obby-2/${name}.bmp"
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/src/obby-2"
	)
endfunction()

generate_obby_2_asset("background")
generate_obby_2_asset("obby2-player")

add_library(
	obby-2
	src/obby-2/obby-2.cpp
	src/obby-2/background.cpp
	src/obby-2/obby2-player.cpp
)
add_executable(
	obby-2-win32 WIN32
	src/obby-2/obby-2-win32.cpp
)
target_link_libraries(
	obby-2-win32
	obby-2
	toms-lane-platform
	toms-lane-win32-application
)

######## demo ########
add_library(
	demo
	src/demo/demo.cpp
)
add_executable(
	demo-win32 WIN32
	src/demo/demo-win32.cpp
)
target_link_libraries(
	demo-win32
	demo
	toms-lane-platform
	toms-lane-win32-application
)
configure_file(src/demo/teapot.obj Debug/teapot.obj COPYONLY)



######## sound-test ########
add_executable(
	sound-test WIN32
	src/sound-test/sound-test.cpp
)
target_link_libraries(
	sound-test
	toms-lane-win32-application
)


######## driving ########
add_executable(
	driving WIN32
	src/driving/driving.cpp
)
target_link_libraries(
	driving
	toms-lane-platform
	toms-lane-win32-application
)
configure_file(src/driving/test.bmp Debug/test.bmp COPYONLY)

