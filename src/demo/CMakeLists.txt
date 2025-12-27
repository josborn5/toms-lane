# https://www.codegenes.net/blog/how-can-i-build-a-c-project-with-multiple-interdependent-subdirectories/

cmake_minimum_required(VERSION 3.31.0)

project(demo VERSION 1.0)


add_subdirectory(../platform ./platform-output)
add_subdirectory(../application ./application-output)

function(generate_demo_asset name)
	file(REMOVE "${CMAKE_SOURCE_DIR}/src/demo/assets/${name}.cpp")
	add_custom_command(
		OUTPUT "${CMAKE_SOURCE_DIR}/src/demo/assets/${name}.cpp"
		COMMAND xxd -i "${name}" > "${name}.cpp"
		DEPENDS "${CMAKE_SOURCE_DIR}/src/demo/assets/${name}"
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/src/demo/assets"
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
	assets/teapot.obj.cpp
)

add_executable(
	demo-tests
	render.tests.cpp
	math.tests.cpp
	camera.tests.cpp
)
add_executable(
	demo-win32 WIN32
	demo-win32.cpp
)

target_link_libraries(
	demo-tests
	demo
	toms-lane-platform
)
target_link_libraries(
	demo-win32
	demo
	toms-lane-platform
	toms-lane-win32-application
)

