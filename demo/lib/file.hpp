void* asset_interface_open(const char* filename);

char* asset_interface_read_line(const void* asset, char* string_buffer, unsigned int string_buffer_size);

void asset_interface_close(void* asset);

