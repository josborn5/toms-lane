message("platform context:")
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

add_library(
	tl-platform-core OBJECT
	lib/toms-lane-platform.cpp
	assets/font-mono.cpp
)
target_compile_options(
	tl-platform-core PRIVATE
    -fno-exceptions
	-ffreestanding
    -nostdlib
)
target_link_options(
	tl-platform-core PRIVATE
	-nostdlib
)

add_library(
	external-math-functions OBJECT
	lib/tl-math-functions.cpp
)

add_library(
	toms-lane-platform STATIC
	$<TARGET_OBJECTS:tl-platform-core>
	$<TARGET_OBJECTS:external-math-functions>
)

message("platform context end")

