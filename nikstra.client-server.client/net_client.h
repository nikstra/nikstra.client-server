#ifndef NET_CLIENT_H
#define NET_CLIENT_H

#include <winsock.h>

#pragma comment(lib,"ws2_32.lib")
#define BUFFER_SIZE 1024
#define MSG_SIZE 200

typedef struct client_connection
{
	char ip_address[16];
	int port;
	SOCKET socket;
	char buffer[BUFFER_SIZE];
	int buffer_pos;
	char message[MSG_SIZE];
} ClientConnection;

ClientConnection* new_NetClient(char* ip_address, int port);
int NetClient_connect(ClientConnection* connection);
int NetClient_query(ClientConnection* connection, char* query);
char* NetClient_read_line(ClientConnection* connection);
char* NetClient_read_status(ClientConnection* connection);
void NetClient_free(ClientConnection* connection);

#endif /* !NET_CLIENT_H */
