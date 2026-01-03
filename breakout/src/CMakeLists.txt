message("breakout context:")
message("CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR}")
message("CMAKE_CURRENT_LIST_DIR=${CMAKE_CURRENT_LIST_DIR}")

add_library(
	breakout STATIC
	breakout.cpp
	render.cpp
	update_state.cpp
)

