add_library(
	toms-lane-win32-application STATIC
	win32/win32-input.cpp
	win32/win32-window.cpp
	win32/win32-file.cpp
	win32/win32-time.cpp
	win32/win32-console.cpp
	win32/win32-winmmsound.cpp
	win32/win32-memory.cpp
	win32/win32-text.cpp
)
target_link_libraries(
	toms-lane-win32-application
	user32
	winmm
	gdi32
)

add_executable(
	toms-lane-win32-application-tests
	win32/win32-tests.cpp
)
target_link_libraries(
	toms-lane-win32-application-tests
	toms-lane-win32-application
)

