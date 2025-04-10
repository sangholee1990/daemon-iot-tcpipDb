#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <iostream>
#include <string.h>
#include "Configure.h"
using namespace std;
struct SOCKET_INFO{
	size_t server_fd;
	socklen_t sock_len;
};
struct SOCKET_INFO_CLIENT{
	size_t client_fd;
	sockaddr_in client_addr;
};

class TCPService{
private:
public:
	SOCKET_INFO Init(uint16_t port);
	SOCKET_INFO_CLIENT Listen(size_t server_fd, socklen_t sock_len);
	void close(size_t client_fd);
	int32_t receive(size_t client_fd, uint8_t *recv_buffer);
	int32_t write(size_t client_fd, uint8_t *write_buffer, size_t len);
	int32_t execute(size_t client_fd, uint16_t msg_id);
};
