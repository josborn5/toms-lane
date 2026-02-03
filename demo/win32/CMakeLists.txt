cmake_minimum_required(VERSION 3.28.0)

project(demo-app-win32 VERSION 1.0)

add_subdirectory(../../application ./application-output)

add_subdirectory(../src ./src-output)

add_executable(
	demo-win32 WIN32
	demo-win32.cpp
)

target_link_libraries(
	demo-win32
	demo
	toms-lane-platform
	toms-lane-win32-application
)

