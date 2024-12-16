
void print_error(const char * context);

HANDLE open_serial_port(const char * device, uint32_t baud_rate);

int write_port(HANDLE port, uint8_t * buffer, size_t size);

SSIZE_T read_port(HANDLE port, uint8_t * buffer, size_t size);
