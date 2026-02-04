#include "./generated-assets.hpp"

static char* read_pointer = nullptr;
static unsigned int read_pointer_counter = 0;

static char* copy_to_char(char* from, char* to, const unsigned int to_length, const char end_char)
{
	if (read_pointer_counter == teapot_obj_len) {
		return nullptr;
	}

	unsigned int local_counter = 0;
	bool match_found = false;
	while (local_counter < to_length &&
			read_pointer_counter < teapot_obj_len &&
			!match_found)
	{
		*to = *from;
		match_found = (*from == end_char);

		from++;
		to++;
		local_counter += 1;
		read_pointer_counter += 1;
	}

	*to = '\0';

	return from;
}



void* asset_interface_open(const char* filename) {
	read_pointer = (char*)teapot_obj;
	read_pointer_counter = 0;
	return (void*)read_pointer;
}

char* asset_interface_read_line(const void* asset, char* string_buffer, unsigned int string_buffer_size) {
	read_pointer = copy_to_char(read_pointer, string_buffer, string_buffer_size, '\n');

	return read_pointer;
}

void asset_interface_close(void* asset) {
	read_pointer = nullptr;
	read_pointer_counter = 0;
}

