#include <sys/socket.h> //Comunicación mediante sockets.
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "tap_server.h"

int init_server_socket(uint16_t port)
{
    //Crear el extremo sin dirección, AF_INET = IPv4, SOCK_STREAM = TCP
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
        return server_fd;
    int opt = 1;
    int err = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (err == -1)
    {
        perror("setsockopt");
        close(server_fd);
		fprintf(stderr, "TME_WAIT error \n");//fix
		return err;
	}
    
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(port);

    err = bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (err == -1)
    {
		perror("bind");
        close(server_fd);
		fprintf(stderr, "Failed to bind socket to address\n");
		return err;
	}
    // is closed by default 
    err = listen(server_fd, 128); //128 = backlog
    if (err == -1)
    {
        perror("listen");
        close(server_fd);
		fprintf(stderr, "Failed to put socket in passive mode\n");
		return err;
	}
    return server_fd;
}