// Author : WuJian

#pragma once
#include "../CppSQLite3.h"
#include "../../IPC/DataSerializer.h"


enum ColumnType
{
	CT_Int = 0,
	CT_Tinyint,
	CT_Int64,
	CT_UInt,
	CT_Varchar,
	CT_String,
	CT_Datetime,
	CT_Blob,

	CT_Count
};


enum ColumnAttribute
{
	CA_PrimaryKey = 0x01,
	CA_AutoIncrement = 0x02,
	CA_ASC = 0x04,
	CA_DESC = 0x08,
	CA_WithDefualt = 0x10,
	CA_WithSpecial = 0x20,
	CA_Syncable = 0x40,
	CA_WithIndex = 0x80,
	CA_DirtyFlag = 0x100,
	CA_Unique = 0x200,
	CA_COLLATE = 0x400,
	CA_NOCASE = 0x800,
};


struct ColumnDescriptor
{
	int iCheck;
	LPCSTR lpszColumnName;
	ColumnType eType;
	DWORD dwAttribute;
	int dwDefault;				// default值可能是负数，所以用int

	std::string GetDValueStr(const _D &d) const;
	UNIVERSAL_LIB_API std::string GetTypeStr() const;
};

class CConditionTree;
class CConditionTreeWithAssign;

struct Column
{
	int iColumnValue;
	CConditionTreeWithAssign operator =(const CConditionTree &);
};

struct TableDescriptor
{
	LPCSTR lpszTableName;
	ColumnDescriptor* pColumns;
	int iColumnCount;
	bool bSyncable;

	LPCSTR GetCName(int i) const { return pColumns[i].lpszColumnName; }
};

struct _Blob
{
	_Blob(const void *pData, int iLen) : m_pData(pData), m_iLen(iLen), m_bIsBinary(true) {}
	_Blob(LPCSTR lpszStr) : m_pData(lpszStr), m_bIsBinary(false) { m_iLen = strlen(lpszStr); }
	const void* GetBinaryArg() const { return m_pData; }
	int GetBinaryArgLength() const { return m_iLen; }
	LPCSTR GetStrAArg() const { return (LPCSTR)m_pData; }
	int GetStrALength() const { return m_iLen; }
	bool IsBinary() const { return m_bIsBinary; }

	bool m_bIsBinary;
	const void *m_pData;
	int m_iLen;
};

typedef std::vector<std::pair<int, _Blob>> BlobVec;

struct UserDefinedFunctionData;
typedef void (*SQLite_Func)(sqlite3_context* ctx, int nArg, sqlite3_value** pp);
typedef std::map<std::string, const UserDefinedFunctionData*> SQLiteFuncMap;
//typedef std::map<SQLite_Func, int /* arg count */> SQLiteFuncMap;
