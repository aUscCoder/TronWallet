///
/// @author chenjianjun
///
#pragma once
#include "../../Universal.h"
#include "see-sqlite3.h"

UNIVERSAL_LIB_API void SQLite_UTCTime(sqlite3_context* ctx, int nArg, sqlite3_value** pp);
UNIVERSAL_LIB_API void SQLite_UTCTimeElapsed(sqlite3_context* ctx, int nArg, sqlite3_value** pp);
UNIVERSAL_LIB_API void SQLite_GUID(sqlite3_context* ctx, int nArg, sqlite3_value** pp);
UNIVERSAL_LIB_API void SQLite_DATETIME(sqlite3_context* ctx, int nArg, sqlite3_value** pp);


namespace sqlite3_api
{
	int open(const char* zFilename, sqlite3** ppDb);
	int open_v2(const char* filename, sqlite3** ppDb, int flags, const char* zVfs);
	int rekey(sqlite3* db, const void* pKey, int nKey);
	int key(sqlite3* db, const void* pKey, int nKey);
	int close(sqlite3* db);

	int create_scalar_function(sqlite3 *db, const char *zFunctionName, int nArg, int eTextRep, void *pData, void (*xFunc)(sqlite3_context*,int,sqlite3_value**));
	int create_aggregate_function(sqlite3 *db, const char *zFunctionName, int nArg, int eTextRep, void *pData, void (*xStep)(sqlite3_context*,int,sqlite3_value**), void (*xFinal)(sqlite3_context*));

	int prepare(sqlite3* db, const char* zSql, int nByte, sqlite3_stmt** ppStmt, const char** pzTail);
	int prepare_v2(sqlite3* db, const char* zSql, int nByte, sqlite3_stmt** ppStmt, const char** pzTail);
	int step(sqlite3_stmt* pStmt);
	int reset(sqlite3_stmt *pStmt);
	int finalize(sqlite3_stmt* pStmt);
	int exec(sqlite3* db, const char* sql, int (*callback)(void*,int,char**,char**), void* pArg, char** errmsg);
	int changes(sqlite3* db);
	sqlite3_int64 last_insert_rowid(sqlite3* db);

	UNIVERSAL_LIB_API int value_int(sqlite3_value* p);
	UNIVERSAL_LIB_API __int64	value_int64(sqlite3_value* p);
	UNIVERSAL_LIB_API const void* value_blob(sqlite3_value* p);
	UNIVERSAL_LIB_API double value_double(sqlite3_value* p);
	UNIVERSAL_LIB_API char* value_text(sqlite3_value* p);
	UNIVERSAL_LIB_API int value_bytes(sqlite3_value* p);

	UNIVERSAL_LIB_API void result_int(sqlite3_context* ctx, int iVal);
	UNIVERSAL_LIB_API void result_int64(sqlite3_context* ctx, sqlite3_int64 iVal);
	UNIVERSAL_LIB_API void result_null(sqlite3_context* ctx);
	UNIVERSAL_LIB_API void result_text(sqlite3_context* ctx, const char* z, int n, void(*xDel)(void*));
	UNIVERSAL_LIB_API void result_zeroblob(sqlite3_context* ctx, int n);
	UNIVERSAL_LIB_API void result_blob(sqlite3_context* ctx, const void* z, int n, void(*xDel)(void*));
	UNIVERSAL_LIB_API void result_double(sqlite3_context* ctx, double rVal);
	UNIVERSAL_LIB_API void result_error_text(sqlite3_context* ctx, const char* z);
	UNIVERSAL_LIB_API void result_error_code(sqlite3_context* ctx, int code);

	const char* column_name(sqlite3_stmt* pStmt, int N);
	int column_count(sqlite3_stmt* pStmt);
	int column_type(sqlite3_stmt* pStmt, int iCol);
	int column_bytes(sqlite3_stmt* pStmt, int iCol);
	int column_int(sqlite3_stmt*, int iCol);
	sqlite3_int64 column_int64(sqlite3_stmt* pStmt, int iCol);
	double column_double(sqlite3_stmt* pStmt, int iCol);
	const char* column_text(sqlite3_stmt* pStmt, int iCol);
	const void *column_blob(sqlite3_stmt* pStmt, int iCol);
	
	int bind_blob(sqlite3_stmt* pStmt, int i, const void* zData, int nData, void(*xDel)(void*));
	int bind_int64(sqlite3_stmt* pStmt, int i, sqlite3_int64 iValue);
	int bind_int(sqlite3_stmt* pStmt, int i, int iValue);
	int bind_double(sqlite3_stmt* pStmt, int i, double fValue);
	int bind_text(sqlite3_stmt* pStmt, int i, const char* zData, int nData, void(*xDel)(void*));
	int bind_text_static(sqlite3_stmt* pStmt, int i, const char* zData, int nData);
	int bind_null(sqlite3_stmt*, int i);
	int clear_bindings(sqlite3_stmt* pStmt);

	const char *errmsg(sqlite3* pdb);
	int busy_timeout(sqlite3* db, int ms);
	void interrupt(sqlite3* db);

	char* vmprintf(const char *zFormat, va_list ap);
	UNIVERSAL_LIB_API void* malloc(int iSize);
	UNIVERSAL_LIB_API void free(void *p);
};