cmake_minimum_required(VERSION 3.31.0)

project(obby-1 VERSION 1.0)

add_subdirectory(../platform ./platform-output)
add_subdirectory(../application ./application-output)

message("obby-1 context:")
message("CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR}")
message("CMAKE_CURRENT_LIST_DIR=${CMAKE_CURRENT_LIST_DIR}")

function(generate_obby_1_asset name)
	file(REMOVE "${CMAKE_CURRENT_LIST_DIR}/assets/${name}.cpp")
	add_custom_command(
		OUTPUT "${CMAKE_CURRENT_LIST_DIR}/assets/${name}.cpp"
		COMMAND xxd -i "${name}.bmp" > "${name}.cpp"
		DEPENDS "${CMAKE_CURRENT_LIST_DIR}/assets/${name}.bmp"
		WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/assets/"
	)
endfunction()

generate_obby_1_asset("brick")
generate_obby_1_asset("checkpoint")
generate_obby_1_asset("player")

add_library(
	obby-1
	src/obby-1.cpp
	assets/brick.cpp
	assets/checkpoint.cpp
	assets/player.cpp
)
add_executable(
	obby-1-win32 WIN32
	src/obby-1-win32.cpp
)
target_link_libraries(
	obby-1-win32
	obby-1
	toms-lane-platform
	toms-lane-win32-application
)


