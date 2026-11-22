message("demo tests start")

add_subdirectory(../../../std-lib-functions ./cmake-build/std-lib-functions-output)
add_subdirectory(../../../platform ./cmake-build/platform-output)
add_subdirectory(../ ./cmake-build/demo-output)

add_executable(
	demo-tests
	render.tests.cpp
	math.tests.cpp
	camera.tests.cpp
)

target_link_libraries(
	demo-tests
	demo
	toms-lane-platform
	external-math-functions
)

message("demo tests end")

