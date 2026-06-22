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

function(add_wasm_library NAME)
	add_library(${NAME} ${ARGN})
	target_compile_options(
		${NAME} PRIVATE
	    -fno-exceptions
		-ffreestanding
	    -nostdlib
	)
	target_link_options(
		${NAME} PRIVATE
	    --target=wasm32-unknown-unknown
		-nostdlib
	    -Wl,--no-entry
	    -Wl,--allow-undefined
	)
endfunction()

function (add_nostdlib_library NAME)
	add_library(${NAME} ${ARGN})
	target_compile_options(
		${NAME} PRIVATE
	    -fno-exceptions
		-ffreestanding
	    -nostdlib
	)
	target_link_options(
		${NAME} PRIVATE
		-nostdlib
	)
endfunction()

add_nostdlib_library(
	tl-render STATIC
	lib/software-rendering.cpp
	lib/software-rendering.sprite.cpp
)
add_wasm_library(
	tl-render-web STATIC
	lib/software-rendering.cpp
	lib/software-rendering.sprite.cpp
)



add_library(
	toms-lane-platform STATIC
	lib/toms-lane-platform.cpp
	assets/font-mono.cpp
	lib/font.cpp
	lib/transform.cpp
	lib/software-rendering.cpp
	lib/software-rendering.sprite.cpp
)

message("platform context end")

