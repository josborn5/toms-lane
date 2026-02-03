# https://www.codegenes.net/blog/how-can-i-build-a-c-project-with-multiple-interdependent-subdirectories/

cmake_minimum_required(VERSION 3.28.0)

project(demo-app VERSION 1.0)

add_subdirectory(../../platform platform-output)

add_subdirectory(test)

message("demo-app context:")
message("CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR}")
message("CMAKE_CURRENT_LIST_DIR=${CMAKE_CURRENT_LIST_DIR}")

function(generate_demo_asset name)
	file(REMOVE "${CMAKE_CURRENT_LIST_DIR}/assets/${name}.cpp")
	add_custom_command(
		OUTPUT "${CMAKE_CURRENT_LIST_DIR}/assets/${name}.cpp"
		COMMAND xxd -i "${name}" > "${name}.cpp"
		DEPENDS "${CMAKE_CURRENT_LIST_DIR}/assets/${name}"
		WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/assets"
	)
endfunction()

generate_demo_asset("teapot.obj")

add_library(
	demo
	demo.cpp
	render.cpp
	asset.cpp
	camera.cpp
	math.cpp
	../assets/teapot.obj.cpp
)


