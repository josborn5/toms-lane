add_library(
	toms-lane-win32-application STATIC
	src/win32/win32-input.cpp
	src/win32/win32-window.cpp
	src/win32/win32-file.cpp
	src/win32/win32-time.cpp
	src/win32/win32-console.cpp
	src/win32/win32-winmmsound.cpp
	src/win32/win32-memory.cpp
	src/win32/win32-text.cpp
)
target_link_libraries(
	toms-lane-win32-application
	user32
	winmm
	gdi32
)


