#ifndef SQLITE3_IMPORT_H
#define SQLITE3_IMPORT_H

// stdcall for sqlite DLL exports
#define SQLITE_API __declspec(dllimport) 
#define SQLITE_CALL __cdecl 

#include <sqlite3.h>

#endif /* SQLITE3_IMPORT_H */