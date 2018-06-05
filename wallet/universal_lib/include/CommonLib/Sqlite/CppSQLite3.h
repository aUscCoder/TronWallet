// Author: wujian
#pragma once
#include "../String/StringHelper.h"
#include "sqlite3.h"


// 传出的密钥长度为23, szDest最小长度为23
extern "C" void* GenSQLite3Key(char* szDest, const unsigned short* lpszUser);

// 需要用户实现这个函数
extern "C" void GetSQLiteTempPath(LPWSTR szTempPath, int iLength);

#define IN_RANGE(val, vMin, vMax) ((val) >= (vMin) && (val) <= (vMax))


class CSQLite3Buffer
{
public:

	CSQLite3Buffer() : mpBuf(NULL) {}
	~CSQLite3Buffer() { clear(); }

	operator const char*() { return mpBuf; } // NOLINT (3.3.2 这个是需要的)

	UNIVERSAL_LIB_API const char* format(const char* szFormat, ...);
	UNIVERSAL_LIB_API void clear();

private:

	char* mpBuf;
};


class CSQLite3Column
{
public:

	CSQLite3Column() : mpStmt(NULL), mnCols(0), mpDB(NULL) {}
	CSQLite3Column(sqlite3* pDB, sqlite3_stmt* pStmt);
	virtual ~CSQLite3Column() { finalize(); }

	CSQLite3Column& operator = (const CSQLite3Column& rCol);
	
	int count() { return mnCols; }

	UNIVERSAL_LIB_API void finalize();
	UNIVERSAL_LIB_API bool fieldExists(const char* szField);

private:

	sqlite3* mpDB;
	sqlite3_stmt* mpStmt;
	int mnCols;
};


struct SQLite3BLOB
{
	const void* pBlob;
	int nSize;
};

class CSQLite3Query
{
public:

	CSQLite3Query() : mpStmt(NULL), mbEof(true), mnCols(0), mpDB(NULL) {}

	CSQLite3Query(sqlite3* pDB, sqlite3_stmt* pStmt, bool bEof);
	virtual ~CSQLite3Query()
	{
		finalize();
	}

	CSQLite3Query& operator = (const CSQLite3Query& rQuery);

	int numFields() { return mpStmt ? mnCols : 0; }

	int fieldIndex(const char* szField);

	UNIVERSAL_LIB_API int fieldDataType(int nCol);

	UNIVERSAL_LIB_API int getIntField(int nField, int nNullValue = 0);
	UNIVERSAL_LIB_API int getIntField(const char* szField, int nNullValue = 0);

	UNIVERSAL_LIB_API __int64 getInt64Field(int nField, __int64 nNullValue = 0);

	UNIVERSAL_LIB_API const char* getStringField(int nField, const char* szNullValue = "");
	UNIVERSAL_LIB_API const char* getStringField(const char* szField, const char* szNullValue = "");

	UNIVERSAL_LIB_API double getFloatField(int nField, double fNullValue = 0.0);
	double getFloatField(const char* szField, double fNullValue = 0.0);

	UNIVERSAL_LIB_API SQLite3BLOB getBlobField(int nField, const void* pNullValue = NULL, int nSize = 0);
	UNIVERSAL_LIB_API SQLite3BLOB getBlobField(const char* szField, void* pNullValue = NULL, int nSize = 0);

	bool eof() { return !mpStmt || !mpDB || mbEof; }

	UNIVERSAL_LIB_API void nextRow();

	UNIVERSAL_LIB_API void finalize();

private:

	bool checkValid(int nField, int iDataType);

	sqlite3* mpDB;
	sqlite3_stmt* mpStmt;
	bool mbEof;
	int mnCols;
};


class CSQLite3DB;

class CSQLite3BindBlob
{
public:
	UNIVERSAL_LIB_API CSQLite3BindBlob(CSQLite3DB* pDB, const char* pszSQL);

	~CSQLite3BindBlob() { finalize(); }

	CSQLite3BindBlob& operator = (const CSQLite3BindBlob& obj)
	{
		finalize();
		mpDB = obj.mpDB;
		mpStmt = obj.mpStmt;
		const_cast<CSQLite3BindBlob&>(obj).mpDB = NULL;
		const_cast<CSQLite3BindBlob&>(obj).mpStmt = NULL;
		return *this;
	}

	bool isOk() { return mpDB && mpStmt; }

	UNIVERSAL_LIB_API void finalize();
	UNIVERSAL_LIB_API int bind(int nParam, const void* blobValue, int nLen);
	UNIVERSAL_LIB_API int bind(int nParam, __int64 i64Val);
	UNIVERSAL_LIB_API int bind(int nParam, int iVal);
	UNIVERSAL_LIB_API int bind(int nParam, double fVal);
	UNIVERSAL_LIB_API int bind(int nParam, const char* pszText, int nLen);
	UNIVERSAL_LIB_API int bindNull(int nParam);
	int bind_static(int nParam, const void* blobValue, int nLen);
	int bind_static(int nParam, const char* pszText, int nLen);
	UNIVERSAL_LIB_API int exec();
	UNIVERSAL_LIB_API bool exec(CSQLite3Query& qRst);
	UNIVERSAL_LIB_API bool prepare(const char* pszSQL);

private:
	sqlite3* mpDB;
	sqlite3_stmt* mpStmt;
};

class CSQLite3DB
{
public:

	CSQLite3DB() : mpDB(NULL), mnBusyTimeoutMs(60000), mnTransaction(0), mnVersion(-1), m_pszLastError("")
	{
	}

	virtual ~CSQLite3DB()
	{
		close();
	}

	sqlite3* getDBPtr() const { return mpDB; }

	UNIVERSAL_LIB_API bool open(const char* szFile, const char** pszError = NULL);

	bool open_readonly(const char* szFile, const char** pszError = NULL);

	UNIVERSAL_LIB_API bool openWithKey(const char* szFile, const void* pKey, int nLen, const char** pszError = NULL);

	UNIVERSAL_LIB_API void close();

	UNIVERSAL_LIB_API bool registerScalarFunction(const char* szFuncName, int nArg,
		void (*xFunc)(sqlite3_context*, int, sqlite3_value**),
		void* pData = NULL, int iTextEncode = SQLITE_UTF8);

	bool registerAggregateFunction(const char* szFuncName, int nArg,
		void (*xStep)(sqlite3_context*, int, sqlite3_value**),
		void (*xFinal)(sqlite3_context*),
		void* pData = NULL, int iTextEncode = SQLITE_UTF8);

	bool is_open() { return mpDB != NULL; }

	UNIVERSAL_LIB_API bool tableExists(const char* szTable);

	UNIVERSAL_LIB_API bool columnExists(const char* szTable, const char* szColumn);

	UNIVERSAL_LIB_API bool integrityCheck();

	UNIVERSAL_LIB_API bool quickCheck(std::string &strOut);

	UNIVERSAL_LIB_API void vacuum();

	UNIVERSAL_LIB_API int execDML(const char* szSQL);
	int execDML(se::stringbuf& sql) { return execDML(sql.str()); }

	UNIVERSAL_LIB_API bool execQuery(CSQLite3Query& qRst, const char* szSQL);
	inline bool execQuery(CSQLite3Query& qRst, se::stringbuf& sql)
	{
		return execQuery(qRst, sql.str());
	}

	UNIVERSAL_LIB_API bool execColumn(CSQLite3Column& col, const char* szTable);

	// 0 - begin ,  1 - commit   ,    2 - rollback
	UNIVERSAL_LIB_API bool execTransaction(int iCmd);

	bool inTransaction() { return mnTransaction > 0; }

	UNIVERSAL_LIB_API sqlite_int64 lastRowId();

	void interrupt();

	bool setKey(const void* pKey, int nLen);

	UNIVERSAL_LIB_API bool resetKey(const void* pKey, int nLen);

	UNIVERSAL_LIB_API bool ExportTo(CSQLite3DB *pTo);
	UNIVERSAL_LIB_API bool ImportFrom(CSQLite3DB *pFrom);

	void setBusyTimeout(int nMillisecs);

	void setVersion(int iVersion) { mnVersion = iVersion; }

	int getVersion() const { return mnVersion; }

	const char* getLastError() { return m_pszLastError; }
#ifdef _DEBUG
	int GetTransactionNum() { return mnTransaction; }
#endif

private:

	CSQLite3DB(const CSQLite3DB& db) : mpDB(db.mpDB), mnBusyTimeoutMs(60000) {}

	CSQLite3DB& operator = (const CSQLite3DB& db)
	{
		mpDB = db.mpDB;
		mnBusyTimeoutMs = 10000;
		return *this;
	}

	bool selectOk();

	sqlite3* mpDB;
	int mnBusyTimeoutMs;
	int mnTransaction;
	int mnVersion;
	const char* m_pszLastError;

	friend class CSQLite3Transaction;
	friend class CSQLite3BindBlob;
};


class CSQLite3Transaction
{
public:

	enum
	{
		BEGIN = 0,
		COMMIT = 1,
		ROLLBACK = 2,
	};

public:

	explicit CSQLite3Transaction(CSQLite3DB* pDB) : mpDB(pDB), mbExecute(false)
	{
		if (mpDB)
			mpDB->execTransaction(BEGIN);
	}

	~CSQLite3Transaction()
	{
		end();
	}

	void end(bool bCommit = true)
	{
		if (!mbExecute && mpDB)
		{
			if (bCommit)
				mpDB->execTransaction(COMMIT);
			else
				mpDB->execTransaction(ROLLBACK);
		}
		mbExecute = true;
	}

private:
	CSQLite3DB* mpDB;
	bool mbExecute;
};
