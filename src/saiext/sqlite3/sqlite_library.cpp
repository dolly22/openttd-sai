#include "../../../stdafx.h"
#include "../../../debug.h"

#include <squirrel.h>
#include "../../../script/squirrel.hpp"
#include "../../../script/squirrel_helper.hpp"
#include "../../../script/squirrel_class.hpp"
#include "../../../script/squirrel_std.hpp"

#include "api/sqlite_conn.hpp.sq"
#include "api/sqlite_command.hpp.sq"
#include "api/sqlite_reader.hpp.sq"

void SQSqliteLibrary_Register(Squirrel *engine)
{
	SQSqliteConn_Register(engine);
	SQSqliteCommand_Register(engine);
	SQSqliteReader_Register(engine);
}

