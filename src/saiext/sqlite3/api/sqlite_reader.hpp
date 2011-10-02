
/** @file sqlite_reader.hpp declarations of the class for Sqlite3 enviroment */

#ifndef SQLITE_READER_HPP
#define SQLITE_READER_HPP

#include <map>
#include "../sqlite3_import.h"
#include <squirrel.h>
#include "../../../stdafx.h"
#include "../../../misc/countedptr.hpp"
#include "../../../core/string_compare_type.hpp"
#include "../../../sai/api/sai_object.hpp"
#include "sqlite_conn.hpp"
#include "sqlite_command.hpp"

class SqliteReader : public SAIObject {

public:
	static const char *GetClassName() { return "SqliteReader"; }

private:
	class SqliteCommand *cmd;

	typedef std::map<const char *, int, StringCompare> NameLookup; 
	NameLookup nameLookup;

	/*
	** Helper method to require opened datareader
	*/
	void requireOpened();

	/*
	** Init name lookup table
	*/
	void initNameLookup();
public:
	SqliteReader(SqliteCommand *cmd);
	~SqliteReader();

	/*
	** Close datareader
	*/
	void Close();

	/*
	** Read from datareader
	*/
	bool Read();

	/*
	** Get integer value from specified result set index
	*/
	int GetInt(int colIdx);

	/*
	** Get integer value from specified result set column name
	*/
	int GetIntName(const char* colName);

	/*
	** Get double value from specified result set index
	*/
	double GetDouble(int colIdx);

	/*
	** Get double value from specified result set column name
	*/
	double GetDoubleName(const char* colName);

	/*
	** Get string value from specified result set index
	*/
	const char* GetString(int colIdx);

	/*
	** Get string value from specified result set column name
	*/
	const char* GetStringName(const char* colName);

	/*
	** Get result set index for specified column name
	*/
	int GetIndex(const char* colName);

	/*
	** SQ helper function to index result set
	*/
	SQInteger _get(HSQUIRRELVM vm);
};

#endif /* SQLITE_READER_HPP */
