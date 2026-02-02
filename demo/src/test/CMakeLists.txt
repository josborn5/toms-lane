message("demo tests start")

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
)

message("demo tests end")

