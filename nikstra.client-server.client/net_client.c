#include <stdio.h>
#include "net_client.h"

ClientConnection* new_NetClient(char* ip_address, int port)
{
	ClientConnection* connection = malloc(sizeof(ClientConnection));
	if (connection != NULL)
	{
		strcpy_s(connection->ip_address, 15, ip_address);
		connection->port = port;
		memset(connection->buffer, 0, BUFFER_SIZE); /* TODO: Should memory be initialized here? */
		connection->buffer_pos = -1;
	}
	return connection;
}

int NetClient_connect(ClientConnection* connection)
{
	WSADATA wsa;
	struct sockaddr_in server;
	int recv_count;
	const char* READY = "READY\r\n";

	memset(connection->buffer, 0, BUFFER_SIZE);

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		sprintf_s(connection->message, MSG_SIZE, "Failed. Error code: %d", WSAGetLastError());
		return -1;
	}

	if ((connection->socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		sprintf_s(connection->message, MSG_SIZE, "Could not create socket: %d", WSAGetLastError());
		return -1;
	}

	server.sin_addr.s_addr = inet_addr(connection->ip_address);
	server.sin_family = AF_INET;
	server.sin_port = htons(connection->port);

	if (connect(connection->socket, (struct sockaddr*) & server, sizeof(server)) < 0)
	{
		sprintf_s(connection->message, MSG_SIZE, "Could not connect to server: %d", WSAGetLastError());
		return -1;
	}

	if ((recv_count = recv(connection->socket, connection->buffer, strlen(READY), 0)) == SOCKET_ERROR)
	{
		sprintf_s(connection->message, MSG_SIZE, "Receive failed: %d", WSAGetLastError());
		return -1;
	}

	if(strcmp(connection->buffer, READY) != 0)
	{
		sprintf_s(connection->message, MSG_SIZE, "Server not ready.");
		return -1;
	}

	return 0;
}

int NetClient_query(ClientConnection* connection, char* query)
{
	char command[BUFFER_SIZE];
	sprintf_s(command, BUFFER_SIZE, "QUERY:%s\r\n", query);

	if (send(connection->socket, command, strlen(command), 0) < 0)
	{
		sprintf_s(connection->message, MSG_SIZE, "Send failed: %d", WSAGetLastError());
		return -1;
	}

	return 0;
}

char* NetClient_read_line(ClientConnection* connection)
{
	int read_count = 0;
	char* found_newline;
	char buf[BUFFER_SIZE];

	if (connection->buffer_pos < 0)
	{
		read_count = fill_buffer(connection);
	}

	/*found_newline = strstr(connection->buffer, "\r\n");*/
	while (found_newline = strstr(connection->buffer, "\r\n") != NULL)
	{
		/* TODO: Calculate what to copy */
		strncpy_s(buf, BUFFER_SIZE, connection->buffer[connection->buffer_pos], found_newline);
		connection->buffer_pos = found_newline - connection->buffer + 2;
	}
	/*
	else
	{
		strncpy_s(buf, BUFFER_SIZE, connection->buffer[connection->buffer_pos], );
	}
	*/
	return "Not implemented";
}

char* NetClient_read_status(ClientConnection* connection)
{
	return connection->message;
}

void NetClient_free(ClientConnection* connection)
{
	closesocket(connection->socket);
	WSACleanup();
	free(connection);
}

static int fill_buffer(ClientConnection* connection)
{
	int recv_count;

	memset(connection->buffer, 0, BUFFER_SIZE);
	if ((recv_count = recv(connection->socket, connection->buffer, BUFFER_SIZE, 0)) == SOCKET_ERROR)
	{
		sprintf_s(connection->message, MSG_SIZE, "Receive failed: %d", WSAGetLastError());
		return -1;
	}

	return recv_count;
}