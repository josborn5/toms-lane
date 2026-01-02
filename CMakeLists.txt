cmake_minimum_required(VERSION 3.31.0)

project(toms-lane VERSION 1.0)

add_subdirectory(application)
add_subdirectory(src/platform)

######## sprite-editor ########
######## breakout ########

######## obby-1 ########

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

######## sound-test ########
add_executable(
	sound-test WIN32
	src/sound-test/sound-test.cpp
)
target_link_libraries(
	sound-test
	toms-lane-win32-application
)


