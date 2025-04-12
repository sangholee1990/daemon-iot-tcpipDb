#include "TCPService.h"

SOCKET_INFO TCPService::Init(uint16_t port)
{
	// TCP Server 초기화 함수
	size_t socket_fd = 0;
	socket_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1)
	{
		spdlog::error("TCP 소켓 생성 실패 : 비정상 종료");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in server_addr;
	memset(&server_addr, 0x00, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(port);

	// 소켓에 주소 할당
	if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		spdlog::error("TCP 소켓 바인딩 실패 : 비정상 종료", socket_fd);
		exit(EXIT_FAILURE);
	}

	spdlog::info("TCP 소켓 생성 : {}", port);

	SOCKET_INFO info = {
		socket_fd,
		sizeof(server_addr)};

	return info;
}

SOCKET_INFO_CLIENT TCPService::Listen(size_t server_fd, socklen_t sock_len)
{
	uint16_t client_number = 0;
	sockaddr_in client_addr;

	if (listen(server_fd, MAX_CLIENT_NO) == -1)
	{
		spdlog::error("클라이언트 연결 대기 실패");
	}

	spdlog::info("클라이언트 연결 대기중");

	client_number = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&sock_len);

	if (client_number == -1)
	{
		spdlog::error("클라이언트 연결 실패");
	}

	spdlog::info("신규 클라이언트 연결 완료 : {}", client_number);

	SOCKET_INFO_CLIENT info = {client_number, client_addr};
	return info;
}

void TCPService::close(size_t client_fd)
{
	close(client_fd);
	spdlog::info("클라이언트 소켓 해제 : {}", client_fd);
}

int32_t TCPService::receive(size_t client_fd, uint8_t *recv_buffer)
{
	int32_t recv_length = 0;
	recv_length = recv(client_fd, recv_buffer, RECV_BUFFER_SIZE, 0);
	
	// spdlog::info("데이터 수신 완료 : {}", recv_length);

	return recv_length;
}

int32_t TCPService::write(size_t client_fd, uint8_t *write_buffer, size_t len)
{
	int32_t sent_len = send(client_fd, write_buffer, len, 0);

	// spdlog::info("데이터 송신 완료 : {}", sent_len);

	return sent_len;
}

int32_t TCPService::execute(size_t client_fd, uint16_t msg_id)
{
	uint8_t buffer[10] = {0};
	buffer[0] = 0xFF;
	buffer[1] = (msg_id >> 8) & 0xFF;
	buffer[2] = msg_id & 0xFF;
	buffer[3] = 0;
	uint16_t checksum = ~(buffer[0] + buffer[1] + buffer[2] + buffer[3]);
	buffer[4] = (checksum >> 8) & 0xFF;
	buffer[5] = checksum & 0xFF;
	int32_t sent_len = send(client_fd, buffer, 6, 0);
	
	return sent_len;
}