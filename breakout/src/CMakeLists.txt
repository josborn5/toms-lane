add_subdirectory(../../platform ./platform-output)
add_subdirectory(../../application ./application-output)

message("breakout context:")
message("CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR}")
message("CMAKE_CURRENT_LIST_DIR=${CMAKE_CURRENT_LIST_DIR}")

add_executable(
	breakout-win32 WIN32
	breakout-win32.cpp
)
add_library(
	breakout STATIC
	breakout.cpp
	render.cpp
	update_state.cpp
)
target_link_libraries(
	breakout-win32
	breakout
	toms-lane-platform
	toms-lane-win32-application
)


