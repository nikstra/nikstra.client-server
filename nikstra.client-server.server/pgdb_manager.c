#include <stdlib.h>
#include <string.h>
#include "pgdb_manager.h"

const PG_MAX_IDENTIFYER_LEN = 63;

void do_exit(PGconn* conn)
{
	PQfinish(conn);
	exit(1);
}

PGconn* connect_db(char* connection_string)
{
	PGconn* conn;

	conn = PQconnectdb(connection_string);
	if (PQstatus(conn) == CONNECTION_BAD)
	{
		fprintf(stderr, "Connection to database failed: %s\n",
			PQerrorMessage(conn));
		do_exit(conn);
	}
	
	return conn;
}

void create_database(char* db_name, char* connection_string)
{
	PGconn* conn;
	PGresult* res;
	char* sql;
	char* sql_args[] = { db_name };

	conn = connect_db(connection_string);
	sql = "CREATE DATABASE client_server_data";
	res = PQexec(conn, sql);
	//res = PQexecParams(conn, sql, 1, NULL, sql_args, NULL, NULL, 0);

	if (PQresultStatus(res) != PGRES_COMMAND_OK)
	{
		printf("Failed to create database: %s\n", PQerrorMessage(conn));
		PQclear(res);
		do_exit(conn);
	}
	PQfinish(conn);
}

void create_table(PGconn* conn)
{
	PGresult* res;
	char* sql = "CREATE TABLE data_value("
		"date DATE,"
		"externalid VARCHAR(18),"
		"number VARCHAR(16) PRIMARY KEY,"
		"constant DECIMAL(4,3),"
		"digits INT,"
		"decimals INT)";
		//" CONSTRAINT data_value_key PRIMARY KEY(externalid))";

	res = PQexec(conn, "DROP TABLE IF EXISTS data_value");

	if (PQresultStatus(res) != PGRES_COMMAND_OK)
	{
		printf("Failed to drop table: %s\n", PQerrorMessage(conn));
		do_exit(conn);
	}

	res = PQexec(conn, sql);

	if (PQresultStatus(res) != PGRES_COMMAND_OK)
	{
		printf("Failed to create table: %s\n", PQerrorMessage(conn));
		PQclear(res);
		do_exit(conn);
	}
}

void import_csv_file(char* filename, char* db_name, char* user, char* pass)
{
	PGconn* conn;
	PGresult* res;
	char sql[1000];
	char* sql_args[] = { filename };
	char connection_string[100];

	//conn = connect_db(sprintf("user=%s password=%s", PQuser(conn), PQpass(conn)));
	sprintf_s(connection_string, 100, "user=%s password=%s dbname=%s", user, pass, "postgres");
	create_database(db_name, connection_string);
	//PQfinish(conn);
	sprintf_s(connection_string, 100, "user=%s password=%s dbname=%s", user, pass, db_name);
	conn = connect_db(connection_string);
	create_table(conn);

	sprintf_s(sql, 1000, "COPY data_value(date, externalid, number, constant, digits, decimals)"
		"FROM '%s' DELIMITER '\t' CSV HEADER", filename);

	res = PQexecParams(conn, sql, 1, NULL, sql_args, NULL, NULL, 0);

	if (PQresultStatus(res) != PGRES_COMMAND_OK)
	{
		printf("Failed to import csv file: %s\n", PQerrorMessage(conn));
		PQclear(res);
		do_exit(conn);
	}
}
#if false
void filter_database(char* filter, char* connection_string)
{
	PGconn* conn;
	PGresult* res;
	char sql[1000];

	conn = connect_db(connection_string);
	printf("Using connection string: %s\n", connection_string);
	// SELECT * FROM data_value WHERE to_char(date, 'YYYY-MM-DD') LIKE '%2006%' OR externalid LIKE '%2006%' OR number LIKE '%2006%' OR constant = 2006.000000 OR digits = 2006 OR decimals = 2006;
	sprintf_s(sql, 1000,
		"SELECT * FROM data_value WHERE "
		"to_char(date, 'YYYY-MM-DD') ILIKE '%s' OR "
		"externalid ILIKE '%s' OR "
		"number ILIKE '%s' OR "
		"constant::char LIKE '%s' OR "
		"digits::char LIKE '%s' OR "
		"decimals::char LIKE '%s'",
		filter, filter, filter, filter, filter, filter);
	//sprintf_s(sql, 1000,
	//	"SELECT * FROM data_value WHERE "
	//		"date LIKE '%%%s%%' OR "
	//		"externalid LIKE '%%%s%%' OR "
	//		"number LIKE '%%%s%%' OR "
	//		"constant LIKE '%%%f%%' OR "
	//		"digits LIKE '%%%d%%' OR "
	//		"decimals LIKE '%%%d%%'",
	//	filter, filter, filter, atof(filter), atoi(filter), atoi(filter));
	res = PQexec(conn, sql);

	if (PQresultStatus(res) != PGRES_TUPLES_OK) {

		printf("No data retrieved\n");
		PQclear(res);
		return;
	}

	int rows = PQntuples(res);

	/* TODO: How do I return this? */
	for (int i = 0; i < rows; i++) {

		printf("%s %s %s %s %s %s\n",
			PQgetvalue(res, i, 0),
			PQgetvalue(res, i, 1),
			PQgetvalue(res, i, 2),
			PQgetvalue(res, i, 3),
			PQgetvalue(res, i, 4),
			PQgetvalue(res, i, 5));
	}

	PQclear(res);
	PQfinish(conn);
}
#endif
void get_db_records(char* filter, char* connection_string, SockSend* sock_send)
{
	PGconn* conn;
	PGresult* res;
	char* str_buf;
	char sql[1000];
	char row[1000];

	conn = connect_db(connection_string);
	printf("Using connection string: %s\n", connection_string);
	// SELECT * FROM data_value WHERE to_char(date, 'YYYY-MM-DD') LIKE '%2006%' OR externalid LIKE '%2006%' OR number LIKE '%2006%' OR constant = 2006.000000 OR digits = 2006 OR decimals = 2006;
	sprintf_s(sql, 1000,
		"SELECT * FROM data_value WHERE "
			"to_char(date, 'YYYY-MM-DD') ILIKE '%s' OR "
			"externalid ILIKE '%s' OR "
			"number ILIKE '%s' OR "
			"constant::char LIKE '%s' OR "
			"digits::char LIKE '%s' OR "
			"decimals::char LIKE '%s'",
		filter, filter, filter, filter, filter, filter);
	res = PQexec(conn, sql);

	if (PQresultStatus(res) != PGRES_TUPLES_OK) {

		printf("No data retrieved\n");
		PQclear(res);
		return;
	}

	int rows = PQntuples(res);

	str_buf = "DATABEGIN\r\n";
	sock_send->send_fptr(sock_send->socket, str_buf, strlen(str_buf) + 1, 0);

	for (int i = 0; i < rows; i++) {

		sprintf_s(row, 1000, "%10s%18s%16s%5s%11s%11s\r\n",
			PQgetvalue(res, i, 0),
			PQgetvalue(res, i, 1),
			PQgetvalue(res, i, 2),
			PQgetvalue(res, i, 3),
			PQgetvalue(res, i, 4),
			PQgetvalue(res, i, 5));

		sock_send->send_fptr(sock_send->socket, row, strlen(row) + 1, 0);
	}
	
	str_buf = "DATAEND\r\n";
	sock_send->send_fptr(sock_send->socket, str_buf, strlen(str_buf) + 1, 0);

	PQclear(res);
	PQfinish(conn);
}

void query_database(char* values, char* table, char* connection_string)
{
	PGconn* conn;
	PGresult* res;
	char sql[1000];
	char* sql_args[] = { values, table };

	conn = connect_db(connection_string);
	printf("Using connection string: %s\n", connection_string);
	//sql = "SELECT $1 FROM $2";
	//res = PQexecParams(conn, sql, 2, NULL, sql_args, NULL, NULL, 0);
	//sql = "SELECT * FROM data_value LIMIT 5";
	sprintf_s(sql, 1000, "SELECT %s FROM %s LIMIT 5", values, table);
	res = PQexec(conn, sql);

	//if (PQresultStatus(res) != PGRES_COMMAND_OK) {

	//	printf("Database query failed: %s\n", PQerrorMessage(conn));
	//	PQclear(res);
	//	do_exit(conn);
	//}

	if (PQresultStatus(res) != PGRES_TUPLES_OK) {

		printf("No data retrieved\n");
		PQclear(res);
		return;
	}

	int rows = PQntuples(res);

	/* TODO: How do I return this? */
	for (int i = 0; i < rows; i++) {

		printf("%s %s %s %s %s %s\n",
			PQgetvalue(res, i, 0),
			PQgetvalue(res, i, 1),
			PQgetvalue(res, i, 2),
			PQgetvalue(res, i, 3),
			PQgetvalue(res, i, 4),
			PQgetvalue(res, i, 5));
	}

	PQclear(res);
	PQfinish(conn);
}

/*****************************************************************************/
void create_databaseX(char* db_name, char* connection_string)
{
	PGconn* conn;
	PGresult* res;
	char command[80] = "CREATE DATABASE ";

	conn = connect_db(connection_string);
	strncat_s(command, strlen(command), db_name, PG_MAX_IDENTIFYER_LEN);
	res = PQexec(conn, command);

	if (PQresultStatus(res) != PGRES_COMMAND_OK) {

		printf("Failed to create database: %s\n", PQerrorMessage(conn));
		PQclear(res);
		do_exit(conn);
	}
}
