#include <stdio.h>
#include <WinSock2.h>
#include <ws2tcpip.h> 
#include <Windows.h>
#include "pgdb_manager.h"
//vcpkg install libpq:x86-windows
#pragma comment(lib, "ws2_32.lib")

#define CONNECTION_STRING "dbname=client_server_data user=efcore password=efcore"
#define BUF_LEN 100

BOOL starts_with(const char* string, const char* part)
{
	return strncmp(part, string, strlen(part)) == 0;
}

DWORD WINAPI client_thread(LPVOID lpParam)
{
	char receive_buf[BUF_LEN];
	char send_buf[BUF_LEN];
	char query_word[BUF_LEN];
	int result = 0;

	SockSend ss;
	SOCKET client_socket = (SOCKET)lpParam;
	
	fprintf(stderr, "Thread created.\n");
	strcpy_s(send_buf, BUF_LEN, "READY\r\n");
	send(client_socket, send_buf, strlen(send_buf) + 1, 0);

	while (1)
	{
		memset(receive_buf, 0, BUF_LEN);
		memset(send_buf, 0, BUF_LEN);
		memset(query_word, 0, BUF_LEN);

		result = recv(client_socket, receive_buf, BUF_LEN, 0);
		if (result == 0)
		{
			fprintf(stderr, "Socket closed\n");
			closesocket(client_socket);
			ExitThread(0);
		}
		else if(result < 0)
		{
			fprintf(stderr, "recv failed: %d\n", WSAGetLastError());
			closesocket(client_socket);
			ExitThread(0);
		}

		if (starts_with(receive_buf, "QUERY:"))
		{
			//receive_buf[result] = '\0';
			printf("RESULT: %d\n", result);
			printf("STRLEN: %d\n", strlen(receive_buf));
			strncpy_s(query_word, BUF_LEN, receive_buf + 6, result - 6);
			printf("QUERY_WORD: %s\n", query_word);
			ss.socket = client_socket;
			ss.send_fptr = send;
			get_db_records(query_word, CONNECTION_STRING, &ss);
		}
		else if (starts_with(receive_buf, "EXIT"))
		{
			fprintf(stderr, "Exiting thread\n");
			strcpy_s(send_buf, BUF_LEN, "OK\r\n");
			send(client_socket, send_buf, strlen(send_buf) + 1, 0);
			closesocket(client_socket);
			ExitThread(0);
		}
		else
		{
			fprintf(stderr, "Unsupported command: %s\n", receive_buf);
			strcpy_s(send_buf, BUF_LEN, "UNSUPPORTED\r\n");
			send(client_socket, send_buf, strlen(send_buf) + 1, 0);
		}

		//strcpy_s(receive_buf, BUF_LEN, "");
		//strcpy_s(send_buf, BUF_LEN, "");
	}
}

int main(int argc, char *argv[])
{
	WSADATA wsa;
	SOCKET s;
	SOCKET new_socket;
	struct sockaddr_in server;
	struct sockaddr_in client;
	int c;
	char ipstr[INET_ADDRSTRLEN];
	//char* message;
	DWORD thread;
	//PGconn* conn;
#if false
	int lib_ver = PQlibVersion();
	printf("Version of libpq: %d\n", lib_ver);
	
	//conn = connect_db("user=efcore dbname=core3mvc password=efcore");
	//char* csv_file = "C:\\Users\\Niklas\\Google Drive\\Synced files\\Ansökningar\\Intervju\\Props Utility Solutions\\data.csv";
	//import_csv_file(csv_file, "client_server_data", "efcore", "efcore");
	//query_database("date", "data_value", "dbname=client_server_data user=efcore password=efcore");
	/* filter_database("%tm%", "dbname=client_server_data user=efcore password=efcore"); */
	//filter_database("%TM210733%", "dbname=client_server_data user=efcore password=efcore");

	return 0;
	//if (PQstatus(conn) == CONNECTION_BAD) {

	//	fprintf(stderr, "Connection to database failed: %s\n",
	//		PQerrorMessage(conn));
	//	do_exit(conn);
	//}

	int ver = PQserverVersion(conn);
	char* db_name = PQdb(conn);

	printf("Server version: %d\nDB name: %s\n", ver, db_name);

	PGresult* res;
	//res = PQexec(conn, "CREATE DATABASE client_server_data");

	//if (PQresultStatus(res) != PGRES_COMMAND_OK) {

	//	printf("Failed to create database: %s\n", PQerrorMessage(conn));
	//	PQclear(res);
	//	do_exit(conn);
	//}

	res = PQexec(conn, "SELECT * FROM \"AspNetUsers\" LIMIT 5");

	if (PQresultStatus(res) != PGRES_TUPLES_OK) {

		printf("No data retrieved\n");
		PQclear(res);
		do_exit(conn);
	}

	int rows = PQntuples(res);

	for (int i = 0; i < rows; i++) {

		printf("%s %s %s\n", PQgetvalue(res, i, 0),
			PQgetvalue(res, i, 1), PQgetvalue(res, i, 2));
	}

	PQclear(res);
	PQfinish(conn);
	
	
	return 0;

#endif
	/*****************************************************************/
	fprintf(stderr, "\nInitialising Winsock...");

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
	}

	fprintf(stderr, "Initialised.\n");

	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		fprintf(stderr, "Could not create socket: %d", WSAGetLastError());
		return 1;
	}

	fprintf(stderr, "Socket created\n");

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(8888);

	if (bind(s, (struct sockaddr*) & server, sizeof(server)) == SOCKET_ERROR)
	{
		fprintf(stderr, "Bind failed with error code: %d", WSAGetLastError());
		return 1;
	}

	fprintf(stderr, "Bind done.\n");

	listen(s, 3);

	fprintf(stderr, "Waiting for incomming connections...");

	c = sizeof(struct sockaddr_in);
	while (1)
	{
		new_socket = accept(s, (struct sockaddr*) & client, & c);
		if (new_socket == INVALID_SOCKET)
		{
			fprintf(stderr, "Accept failed with error code: %d", WSAGetLastError());
			return 1;
		}

		inet_ntop(AF_INET, &client.sin_addr, (PSTR)ipstr, sizeof(ipstr));
		fprintf(stderr, "Connection accepted from %s:%d.\n", ipstr, ntohs(client.sin_port));

		CreateThread(NULL, 0, client_thread, (LPVOID)new_socket, 0, &thread);
		/*
		message = "Message from server\n";
		typedef int (__stdcall* send_fptr)(SOCKET, const char*, int, int);
		//send_fptr ss = send;
		SockSend ss;
		ss.socket = new_socket;
		ss.send_fptr = send;
		get_db_records("%TM%", "dbname=client_server_data user=efcore password=efcore", &ss);
		//filter_database("%TM210733%", "dbname=client_server_data user=efcore password=efcore", &ss);

		//send(new_socket, message, strlen(message), 0);
		*/
	}
	
	getchar();

	closesocket(s);
	WSACleanup();

	return 0;
}