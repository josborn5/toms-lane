cmake_minimum_required(VERSION 3.28.0)

project(obby-1-win32 VERSION 1.0)

add_subdirectory(../../application ./application-output)

add_subdirectory(../lib ./lib-output)

add_executable(
	obby-1-win32 WIN32
	obby-1-win32.cpp
)

target_link_libraries(
	obby-1-win32
	obby-1
	toms-lane-platform
	toms-lane-win32-application
)

