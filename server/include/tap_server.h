#ifndef TAP_SERVER_H
# define TAP_SERVER_H


char *read_file_to_string(const char *filepath);

//socket
int init_server_socket(uint16_t port);
#endif