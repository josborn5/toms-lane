cmake_minimum_required(VERSION 3.28.0)

project(breakout VERSION 1.0)

add_subdirectory(../../platform ./platform-output)

message("breakout context:")
message("CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR}")
message("CMAKE_CURRENT_LIST_DIR=${CMAKE_CURRENT_LIST_DIR}")

function(generate_breakout_asset name)
	file(REMOVE "${CMAKE_CURRENT_LIST_DIR}/../assets/${name}.cpp")
	add_custom_command(
		OUTPUT "${CMAKE_CURRENT_LIST_DIR}/../assets/${name}.cpp"
		COMMAND xxd -i "${name}.bmp" > "${name}.cpp"
		DEPENDS "${CMAKE_CURRENT_LIST_DIR}/../assets/${name}.bmp"
		WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../assets/"
	)
endfunction()

generate_breakout_asset("brick")

add_library(
	breakout STATIC
	breakout.cpp
	render.cpp
	update_state.cpp
	../assets/brick.cpp
)

