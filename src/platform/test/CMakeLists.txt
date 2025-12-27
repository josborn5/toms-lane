cmake_minimum_required(VERSION 3.31.0)

project(platform VERSION 1.0)

add_subdirectory(../ ./cmake-build/platform-output)


message("platform test context:")
message("CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR}")
message("CMAKE_CURRENT_LIST_DIR=${CMAKE_CURRENT_LIST_DIR}")


function(generate_platform_asset input_file output_file)
	file(REMOVE "${CMAKE_CURRENT_LIST_DIR}/assets/${output_file}")
	add_custom_command(
		OUTPUT "${CMAKE_CURRENT_LIST_DIR}/assets/${output_file}"
		COMMAND xxd -i "${input_file}" > "${output_file}"
		DEPENDS "${CMAKE_CURRENT_LIST_DIR}/assets/${input_file}"
		WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/assets"
	)
endfunction()
generate_platform_asset(font-mono.tlsf font-mono.cpp)
generate_platform_asset(test.bmp test.cpp)
generate_platform_asset(bitmap-4bit.bmp bitmap-4bit.cpp)
generate_platform_asset(monochrome.bmp monochrome.cpp)
generate_platform_asset(player-m.bmp player-m.cpp)
generate_platform_asset(9x9-1bit.bmp 9x9-1bit.cpp)
generate_platform_asset(8x8-1bit.bmp 8x8-1bit.cpp)
generate_platform_asset(7x7-1bit.bmp 7x7-1bit.cpp)
generate_platform_asset(6x6-1bit.bmp 6x6-1bit.cpp)
generate_platform_asset(5x5-1bit.bmp 5x5-1bit.cpp)
generate_platform_asset(4x4-1bit.bmp 4x4-1bit.cpp)

add_executable(
	toms-lane-platform-test
	toms-lane-platform.tests.cpp
	assets/test.cpp
	assets/bitmap-4bit.cpp
	assets/monochrome.cpp
	assets/player-m.cpp
	assets/9x9-1bit.cpp
	assets/8x8-1bit.cpp
	assets/7x7-1bit.cpp
	assets/6x6-1bit.cpp
	assets/5x5-1bit.cpp
	assets/4x4-1bit.cpp
)
target_link_libraries(
	toms-lane-platform-test
	toms-lane-platform
)


