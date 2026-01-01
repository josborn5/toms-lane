# https://www.codegenes.net/blog/how-can-i-build-a-c-project-with-multiple-interdependent-subdirectories/

cmake_minimum_required(VERSION 3.31.0)

project(demo-app VERSION 1.0)

add_subdirectory(../src/platform ./platform-output)
add_subdirectory(../application ./application-output)

add_subdirectory(src/test)

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
	src/demo.cpp
	src/render.cpp
	src/asset.cpp
	src/camera.cpp
	src/math.cpp
	assets/teapot.obj.cpp
)

add_executable(
	demo-win32 WIN32
	src/demo-win32.cpp
)

target_link_libraries(
	demo-win32
	demo
	toms-lane-platform
	toms-lane-win32-application
)

