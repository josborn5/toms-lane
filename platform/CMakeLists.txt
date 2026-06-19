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
	tl-font STATIC
	lib/font.cpp
)
target_compile_options(
	tl-font PRIVATE
	-ffreestanding
)
target_link_options(
	tl-font PRIVATE
	-nostdlib
)

add_library(
	tl-assert STATIC
	lib/assert.cpp
)
target_compile_options(
	tl-assert PRIVATE
	-ffreestanding
)
target_link_options(
	tl-assert PRIVATE
	-nostdlib
)



add_library(
	toms-lane-platform STATIC
	lib/toms-lane-platform.cpp
	assets/font-mono.cpp
	lib/font.cpp
	lib/transform.cpp
)

message("platform context end")

