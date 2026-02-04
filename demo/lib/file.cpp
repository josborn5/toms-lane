#include <stdio.h>

void* asset_interface_open(const char* filename) {
	return (void*)fopen(filename, "r");
}

char* asset_interface_read_line(const void* asset, char* string_buffer, unsigned int string_buffer_size) {
	return fgets(string_buffer, string_buffer_size, (FILE*)asset);
}

void asset_interface_close(void* asset) {
	fclose((FILE*)asset);
}

