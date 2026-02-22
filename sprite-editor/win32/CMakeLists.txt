cmake_minimum_required(VERSION 3.28.0)

project(sprite-editor-win32 VERSION 1.0)

add_subdirectory(../../application ./application-output)

add_subdirectory(../lib ./lib-output)

add_executable(
	sprite-editor-win32 WIN32
	sprite-editor-win32.cpp
	render.cpp
)

target_link_libraries(
	sprite-editor-win32
	sprite-editor
	toms-lane-platform
	toms-lane-win32-application
)


