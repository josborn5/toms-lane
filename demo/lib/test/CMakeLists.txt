message("demo tests start")

add_subdirectory(../../../std-lib-functions ./std-lib-functions-output)

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

