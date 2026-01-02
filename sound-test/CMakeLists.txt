cmake_minimum_required(VERSION 3.31.0)

project(sound-test VERSION 1.0)

add_subdirectory(../application ./application-output)

add_executable(
	sound-test WIN32
	sound-test.cpp
)
target_link_libraries(
	sound-test
	toms-lane-win32-application
)


