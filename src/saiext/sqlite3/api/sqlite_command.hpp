/** @file sqlite_command.hpp declarations of the class for Sqlite3 enviroment */

#ifndef SQLITE_COMMAND_HPP
#define SQLITE_COMMAND_HPP

#include "../sqlite3_import.h"
#include <squirrel.h>
#include "../../../stdafx.h"
#include "../../../misc/countedptr.hpp"
#include "sqlite_conn.hpp"
#include "sqlite_reader.hpp"

class SqliteCommand : public SAIObject {
	friend class SqliteReader;

public:
	static const char *GetClassName() { return "SqliteCommand"; }

private:
	class SqliteConn *conn; 
	sqlite3_stmt *stmt;
	unsigned int refs;
	int argc;

	/*
	** Safe way to get (possibly finalized) statement
	*/
	sqlite3_stmt* getStatement();

	/*
	** Resolve parameter index by name
	*/
	int resolveParameter(const char* paramName);

	/*
	** Helper function for execute scalar
	*/
	void SqliteCommand::executeScalarInternal();

public:
	SqliteCommand(class SqliteConn* conn, const char* sql);
	~SqliteCommand();

	/*
	** Reset prepared statement (variables values not reset, but command can be executed again)
	*/
	void Reset();

	/*
	** Reset prepared statment parameter values
	*/
	void ClearBindings();

	/*
	** Close and release statement
	*/
	void Finalize();

	/*
	** Bind value for int parameter
	*/
	void BindInt(int paramNumber, int value);

	/*
	** Bind value for int parameter by name
	*/
	void BindIntName(const char* paramName, int value);

	/*
	** Bind value for double parameter
	*/
	void BindDouble(int paramNumber, double value);

	/*
	** Bind value for double parameter by name
	*/
	void BindDoubleName(const char* paramName, double value);

	/*
	** Bind null value for parameter
	*/
	void BindNull(int paramNumber);

	/*
	** Bind null value for parameter by name
	*/
	void BindNullName(const char* paramName);

	/*
	** Bind value for string parameter
	*/
	void BindString(int paramNumber, const char* value);

	/*
	** Bind value for string parameter by name
	*/
	void BindStringName(const char* paramName, const char* value);

	/*
	** Execute query that does not return any results
	*/
	void ExecuteNonQuery();

	/*
	** Execute query that returns single integer value
	*/
	int ExecuteInt();

	/*
	** Execute query that returns single string value
	*/
	const char* ExecuteString();

	/*
	** Execute query that returns single double value
	*/
	double ExecuteDouble();

	/*
	** Execute query data reader
	*/
	SqliteReader* ExecuteReader();
};

#endif /* SQLITE_COMMAND_HPP */
