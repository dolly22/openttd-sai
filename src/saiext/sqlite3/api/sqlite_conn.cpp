/** @file sqlite_conn.cpp Implementation of SqliteConn. */

#include "sqlite_conn.hpp"

SqliteConn::SqliteConn() {
	this->db = NULL;
	this->stmtsFinalized = false;
}

SqliteConn::~SqliteConn() {
	if(this->db) 
		sqlite3_close(this->db);
}


void SqliteConn::requireOpenDb()
{
	if (!this->db)
		ReportError("Database is not opened.");
}

void SqliteConn::Open(const char *db) {
	if (sqlite3_open_v2(db, &this->db, SQLITE_OPEN_READWRITE, NULL) != SQLITE_OK)
		ReportError(sqlite3_errmsg(this->db));
	stmtsFinalized = false;
}

void SqliteConn::Close(bool finalizeStatements = true) {
	if(this->db) {
		if (finalizeStatements)
			FinalizeStatements();

		if (sqlite3_close(this->db) != SQLITE_OK)
			ReportError(sqlite3_errmsg(this->db));
		this->db = NULL;
	};
}

void SqliteConn::FinalizeStatements()
{
	sqlite3_stmt *pStmt;

	while((pStmt = sqlite3_next_stmt(db, 0))!=0 ){
		if (sqlite3_finalize(pStmt) != SQLITE_OK)
			ReportError(sqlite3_errmsg(this->db));
	}
	stmtsFinalized = true;
}


void SqliteConn::Exec(const char* sql)
{
	requireOpenDb();

	if (sqlite3_exec(this->db, sql, NULL, NULL, NULL) != SQLITE_OK)
		ReportError(sqlite3_errmsg(this->db));
}

SqliteCommand* SqliteConn::CreateCommand(const char* sql)
{
	requireOpenDb();

	return new SqliteCommand(this, sql);
}

int SqliteConn::GetLastAutoId()
{
	requireOpenDb();
	return sqlite3_last_insert_rowid(this->db);
}





