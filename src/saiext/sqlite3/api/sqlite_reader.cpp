/** @file sqlite_reader.cpp Implementation of SqliteReader. */

#include "sqlite_reader.hpp"
#include "../../../string_func.h"

SqliteReader::SqliteReader(SqliteCommand *cmd) : cmd(cmd)
{
	++cmd->refs;
}

SqliteReader::~SqliteReader()
{
	this->Close();
}


bool SqliteReader::Read() {
	requireOpened();

	switch(sqlite3_step(this->cmd->getStatement())) {
		case SQLITE_ROW:
			return true;
		case SQLITE_DONE:
			return false;
		case SQLITE_ERROR:
			ReportError(sqlite3_errmsg(this->cmd->conn->db));
		default:
			ReportError("Error occured while reading from SqliteReader.");
	}
	return false;
}

void SqliteReader::Close() {
	if (this->cmd) {
		if(--this->cmd->refs==0)
			this->cmd->Reset();
		this->cmd = NULL;
	}
}

int SqliteReader::GetInt(int colIdx)
{
	requireOpened();
	return sqlite3_column_int(this->cmd->getStatement(), colIdx);
}

int SqliteReader::GetIntName(const char* colName)
{
	return GetInt(GetIndex(colName));
}

double SqliteReader::GetDouble(int colIdx)
{
	requireOpened();
	return sqlite3_column_double(this->cmd->getStatement(), colIdx);
}

double SqliteReader::GetDoubleName(const char* colName)
{
	return GetDouble(GetIndex(colName));
}

const char* SqliteReader::GetString(int colIdx)
{
	requireOpened();
	return (const char *)sqlite3_column_text(this->cmd->getStatement(), colIdx);
}

const char* SqliteReader::GetStringName(const char* colName)
{
	return GetString(GetIndex(colName));
}



SQInteger SqliteReader::_get(HSQUIRRELVM vm) {
	if (sq_gettype(vm, 2) != OT_STRING) 
		return SQ_ERROR;

	const SQChar* colName;
	sq_getstring(vm, 2, &colName);

	try {
		// advanced method does not wait for exception...
		int colIndex =  GetIndex(FS2OTTD(colName));
		const char* value = (const char*)sqlite3_column_text(this->cmd->getStatement(), colIndex);
		sq_pushstring(vm, OTTD2FS(value), -1);
	}	
	catch (SQInteger e)
	{
		return e;
	}
	return 1;
}


int SqliteReader::GetIndex(const char* colName)
{
	// lookups and stuff...
	if (nameLookup.empty())
		initNameLookup();

	NameLookup::iterator iter = this->nameLookup.find(colName);
	if (iter == this->nameLookup.end()) {
		// column was not found
		char msg[1024];
		seprintf(msg, lastof(msg), "Column '%s' was not found in datareader.", colName);
		ReportError(msg);
	} else {
		// found...
		return (*iter).second;
	}
	return -1;
}

void SqliteReader::requireOpened()
{
	if (!this->cmd)
		ReportError("SqliteReader is closed.");
}

void SqliteReader::initNameLookup()
{
	sqlite3_stmt* stmt = this->cmd->getStatement();

	for(int i=0; i < cmd->argc; i++)
	{
		const char* colName = sqlite3_column_name(stmt, i);
		if (colName != '\0')
		{
			nameLookup[colName] = i;
		}
	}
}
