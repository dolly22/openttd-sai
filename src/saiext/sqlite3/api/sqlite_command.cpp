/** @file sqlite_command.cpp Implementation of SqliteCommand. */

#include "sqlite_command.hpp"
#include "../../../string_func.h"

SqliteCommand::SqliteCommand(SqliteConn* conn, const char* sql) : conn(conn), refs(0) {
	conn->requireOpenDb();

	// prepare statement
	const char *tail=NULL;
	if (sqlite3_prepare_v2(conn->db, sql, -1, &this->stmt, &tail) != SQLITE_OK)
		ReportError(sqlite3_errmsg(conn->db));

	this->argc=sqlite3_column_count(this->stmt);
}

SqliteCommand::~SqliteCommand() {
	sqlite3_finalize(getStatement());
}

void SqliteCommand::Reset()
{
	if (sqlite3_reset(getStatement()) != SQLITE_OK)
		ReportError(sqlite3_errmsg(conn->db));
}

void SqliteCommand::ClearBindings()
{
	if (sqlite3_clear_bindings(getStatement()) != SQLITE_OK)
		ReportError(sqlite3_errmsg(conn->db));
}

void SqliteCommand::Finalize()
{
	if (sqlite3_finalize(getStatement()) != SQLITE_OK)
		ReportError(sqlite3_errmsg(conn->db));
	this->stmt = NULL;
}

void SqliteCommand::BindInt(int paramNumber, int value)
{
	if (sqlite3_bind_int(getStatement(), paramNumber, value) != SQLITE_OK)
		ReportError(sqlite3_errmsg(conn->db));
}

void SqliteCommand::BindIntName(const char* paramName, int value)
{
	BindInt(resolveParameter(paramName), value);
}

void SqliteCommand::BindDouble(int paramNumber, double value)
{
	if (sqlite3_bind_double(getStatement(), paramNumber, value) != SQLITE_OK)
		ReportError(sqlite3_errmsg(conn->db));
}

void SqliteCommand::BindDoubleName(const char* paramName, double value)
{
	BindDouble(resolveParameter(paramName), value);
}

void SqliteCommand::BindNull(int paramNumber)
{
	if (sqlite3_bind_null(getStatement(), paramNumber) != SQLITE_OK)
		ReportError(sqlite3_errmsg(conn->db));
}

void SqliteCommand::BindNullName(const char* paramName)
{
	BindNull(resolveParameter(paramName));
}

void SqliteCommand::BindString(int paramNumber, const char* value)
{
	if (sqlite3_bind_text(getStatement(), paramNumber, value, -1, SQLITE_TRANSIENT) != SQLITE_OK)
		ReportError(sqlite3_errmsg(conn->db));
}

void SqliteCommand::BindStringName(const char* paramName, const char* value)
{
	BindString(resolveParameter(paramName), value);
}

void SqliteCommand::ExecuteNonQuery()
{
	switch(sqlite3_step(getStatement())) {
		case SQLITE_ROW:
			// we are not expecting any rows when execute non query...
			Reset();
			return;
		case SQLITE_DONE:
			return;
		case SQLITE_ERROR:
			ReportError(sqlite3_errmsg(conn->db));
		default:
			ReportError("Error occured while executing query.");
	}
}

int SqliteCommand::ExecuteInt()
{
	executeScalarInternal();
	return sqlite3_column_int(getStatement(), 0);
}

const char* SqliteCommand::ExecuteString()
{
	executeScalarInternal();
	return (const char *)sqlite3_column_text(getStatement(), 0);
}

double SqliteCommand::ExecuteDouble()
{
	executeScalarInternal();
	return sqlite3_column_double(getStatement(), 0);
}

SqliteReader* SqliteCommand::ExecuteReader()
{
	return new SqliteReader(this);
}

void SqliteCommand::executeScalarInternal()
{
	switch(sqlite3_step(getStatement())) {	
		case SQLITE_ROW:
			return;
		case SQLITE_ERROR:
			ReportError(sqlite3_errmsg(conn->db));
		default:
			ReportError("Error occured while executing query.");
	}
}

int SqliteCommand::resolveParameter(const char* paramName)
{
	int paramIdx = sqlite3_bind_parameter_index(getStatement(), paramName);

	if (paramIdx == 0) {
		char message[1024];
		seprintf(message, lastof(message), "Parameter '%s' not found.", paramName);
		ReportError(message);
	}
	return paramIdx;
}

sqlite3_stmt* SqliteCommand::getStatement()
{
	// get statement the safe way, if statements were finalized on connection level
	// access exception would be very possible
	if (this->conn->stmtsFinalized)
		return NULL;

	return this->stmt;
}



