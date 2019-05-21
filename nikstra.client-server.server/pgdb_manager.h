#ifndef PGDB_MANAGER_H
#define PGDB_MANAGER_H

#include <WinSock2.h>
#include <libpq-fe.h>

typedef struct sock_send
{
	SOCKET socket;
	int(__stdcall* send_fptr)(SOCKET, const char*, int, int);
} SockSend;


void do_exit(PGconn*);
PGconn* connect_db(char*);
void create_database(char*, char*);
void query_database(char*, char*, char*);
void import_csv_file(char*, char*, char*, char*);
//void filter_database(char*, char*);
void get_db_records(char* filter, char* connection_string, SockSend* sock_send);

#endif /* PGDB_MANAGER_H */
