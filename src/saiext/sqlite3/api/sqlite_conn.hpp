/** @file sqlite_conn.hpp declarations of the class for Sqlite3 enviroment */

#ifndef SQLITE_CONN_HPP
#define SQLITE_CONN_HPP

#include "../sqlite3_import.h"
#include <squirrel.h>
#include "../../../stdafx.h"
#include "../../../misc/countedptr.hpp"
#include "../../../sai/api/sai_object.hpp"
#include "sqlite_command.hpp"

class SqliteConn : public SAIObject {
	friend class SqliteCommand;
	friend class SqliteReader;

public:
	static const char *GetClassName() { return "SqliteConn"; }

private:	
	bool stmtsFinalized; 
	sqlite3 *db;

	/*
	** Ensure the database is opened
	*/
	void requireOpenDb();

public:
	SqliteConn();
	~SqliteConn();

	/*
	** Open sqlite3 database
	*/
	void Open(const char *db);

	/*
	** Close database (and optionally finalize all prepared statements)
	*/
	void Close(bool finalizeStatements);

	/*
	** Create commmand in current connection
	*/
	SqliteCommand* CreateCommand(const char* sql);

	/*
	** Finalize all prepared statements
	*/
	void FinalizeStatements();

	/*
	** Execute sql statement
	*/
	void Exec(const char* sql);

	/*
	** Get last identity value
	*/
	int GetLastAutoId();
};

#endif /* SQLITE_CONN_HPP */
