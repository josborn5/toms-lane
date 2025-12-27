cmake_minimum_required(VERSION 3.31.0)

project(toms-lane VERSION 1.0)

add_subdirectory(src/application)
add_subdirectory(src/platform)

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
	file(REMOVE "${CMAKE_SOURCE_DIR}/src/obby-1/assets/${name}.cpp")
	add_custom_command(
		OUTPUT "${CMAKE_SOURCE_DIR}/src/obby-1/assets/${name}.cpp"
		COMMAND xxd -i "${name}.bmp" > "${name}.cpp"
		DEPENDS "${CMAKE_SOURCE_DIR}/src/obby-1/assets/${name}.bmp"
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/src/obby-1/assets"
	)
endfunction()

generate_obby_1_asset("brick")
generate_obby_1_asset("checkpoint")
generate_obby_1_asset("player")

add_library(
	obby-1
	src/obby-1/obby-1.cpp
	src/obby-1/assets/brick.cpp
	src/obby-1/assets/checkpoint.cpp
	src/obby-1/assets/player.cpp
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
	file(REMOVE "${CMAKE_SOURCE_DIR}/src/obby-2/assets/${name}.cpp")
	add_custom_command(
		OUTPUT "${CMAKE_SOURCE_DIR}/src/obby-2/assets/${name}.cpp"
		COMMAND xxd -i "${name}.bmp" > "${name}.cpp"
		DEPENDS "${CMAKE_SOURCE_DIR}/src/obby-2/assets/${name}.bmp"
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/src/obby-2/assets"
	)
endfunction()

generate_obby_2_asset("background")
generate_obby_2_asset("obby2-player")

add_library(
	obby-2
	src/obby-2/obby-2.cpp
	src/obby-2/assets/background.cpp
	src/obby-2/assets/obby2-player.cpp
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
add_subdirectory(src/demo)


######## sound-test ########
add_executable(
	sound-test WIN32
	src/sound-test/sound-test.cpp
)
target_link_libraries(
	sound-test
	toms-lane-win32-application
)


