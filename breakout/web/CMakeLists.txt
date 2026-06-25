cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

project(tl-window)

add_subdirectory(../lib ./lib-output)

add_executable(tl-window
    tl-window.cpp
)

target_compile_options(tl-window PRIVATE
    --target=wasm32-unknown-unknown
    -fno-exceptions
    -nostdlib
)

target_link_libraries(
	tl-window
	tl-platform-core-web
	breakout-web
	tl-font-web
)

target_link_options(tl-window PRIVATE
    --target=wasm32-unknown-unknown
    -nostdlib
    -Wl,--no-entry
    -Wl,--allow-undefined
    -Wl,--export=tl_main
    -Wl,--export=tl_tick
    -Wl,--export=tl_pixels
    -Wl,--export=tl_set_mouse
    -Wl,--export=memory
)

set_target_properties(tl-window PROPERTIES
    SUFFIX ".wasm"
)
