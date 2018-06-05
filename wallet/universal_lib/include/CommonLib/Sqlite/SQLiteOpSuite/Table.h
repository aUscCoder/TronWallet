// Author: WuJian
#pragma once
#include "Deleter.h"
#include "Inserter.h"
#include "Selector.h"
#include "Updater.h"


class CTableBase
{
public:

	static std::string MakeCreateColumnStr(const ColumnDescriptor &cd)
	{
		se::stringbuf sb;
		sb << cd.lpszColumnName << cd.GetTypeStr();
		if (cd.dwAttribute & CA_PrimaryKey)
			sb << " PRIMARY KEY";
		if (cd.dwAttribute & CA_ASC)
			sb << " ASC";
		if (cd.dwAttribute & CA_AutoIncrement)
			sb << " AUTOINCREMENT";
		if (cd.dwAttribute & CA_Unique)
			sb << " UNIQUE";
		if (cd.dwAttribute & CA_WithDefualt)
			sb << " default " << cd.dwDefault;
		if (cd.dwAttribute & CA_WithSpecial)
			sb << ' ' << (LPCSTR)cd.dwDefault;
		if (cd.dwAttribute & CA_COLLATE)
			sb << " COLLATE";
		if (cd.dwAttribute & CA_NOCASE)
			sb << " NOCASE";
		return sb.str();
	}

	static void AppendCreateIndexStrImpl(se::stringbuf &sb, LPCSTR lpszTableName, const ColumnDescriptor &cd)
	{
		sb << "CREATE INDEX IF NOT EXISTS " << lpszTableName << '_' << cd.lpszColumnName << "_index" << " ON " << lpszTableName
			<< "(" << cd.lpszColumnName << ");";
	}

	static std::string GetCreateTableStrImpl(LPCSTR lpszTableName, ColumnDescriptor* pColumns, int iColCount, LPCSTR lpszDBName = "")
	{
		se::stringbuf sb;
		sb << "CREATE TABLE IF NOT EXISTS ";
		if (lpszDBName[0] != '\0')
			sb << lpszDBName << '.';
		sb << lpszTableName << " (";
		for (int i = 0; i < iColCount; i++)
			sb << MakeCreateColumnStr(pColumns[i]) << ',';
		sb.crop(1);
		sb << ");";

		for (int i = 0; i < iColCount; i++)
			if (pColumns[i].dwAttribute & CA_WithIndex)
				AppendCreateIndexStrImpl(sb, lpszTableName, pColumns[i]);

		return sb.str();
	}
	
	static std::string GetDropTableStrImpl(LPCSTR lpszTableName, LPCSTR lpszDBName = "")
	{
		se::stringbuf sb;
		sb << "DROP TABLE IF EXISTS ";
		if (lpszDBName[0] != '\0')
			sb << lpszDBName << '.';
		sb << lpszTableName;
		return sb.str();
	}

	static std::string GetRepairTableIndexStringImpl(LPCSTR lpszTableName, ColumnDescriptor* pColumns, int iColCount, LPCSTR lpszDBName = "")
	{
		se::stringbuf sb;
		for (int i = 0; i < iColCount; ++i)
			if (pColumns[i].dwAttribute & CA_WithIndex)
				AppendCreateIndexStrImpl(sb, lpszTableName, pColumns[i]);

		return sb.str();
	}

	CTableBase() {}
	~CTableBase() {}
};



//////////////////////////////////////////////////////////////////////////



#define BEGIN_TABLE_IMPL(table_name_in_db, table_class) \
	__declspec(selectany) class class_##table_class : public CTableBase \
	{ \
	private: \
		LPCSTR m_lpszTableName; \
	public: \
		class_##table_class() : m_lpszTableName(table_name_in_db) {} \
		class_##table_class(LPCSTR lpszTableName) : m_lpszTableName(lpszTableName) {} \
		LPCSTR GetTableName() { Get(); return m_lpszTableName; } \
		bool IsSyncTable() const { return false; }

#define BEGIN_SYNC_TABLE_IMPL(table_name_in_db, table_class) \
	__declspec(selectany) class class_##table_class : public CTableBase \
	{ \
	private: \
		LPCSTR m_lpszTableName; \
	public: \
		class_##table_class() : m_lpszTableName(table_name_in_db) {} \
		class_##table_class(LPCSTR lpszTableName) : m_lpszTableName(lpszTableName) {} \
		LPCSTR GetTableName() { Get(); return m_lpszTableName; } \
		bool IsSyncTable() const { return true; }



#define DEFINE_COLUMN_NAME(...) \
	private: \
		/* 私有内嵌类，用于把描述table列下标的enum的定义封装起来，不跟Column的同名成员冲突 */ \
		class TableEnumClass \
		{ \
		public: \
			enum TableColumnEnum \
			{ \
				__VA_ARGS__, \
				TableColumnCount \
			}; \
		}; \
	public: \
		/* 对table的列使用Column类型而不使用普通的enum是为了在其他场所做检查， */ \
		/* 防止误传入普通的int */ \
		Column __VA_ARGS__; \
		ColumnDescriptor* Get() \
		{ \
			static ColumnDescriptor columns[TableEnumClass::TableColumnCount] = {}; \
			static bool bInited = false; \
			if (!bInited) \
			{ \

#define DEFINE_SYNC_COLUMN_NAME(...) \
	DEFINE_COLUMN_NAME(dirty, __VA_ARGS__) \
	DEFINE_COLUMN(dirty, CT_Int, CA_WithDefualt | CA_WithIndex | CA_DirtyFlag, 1)


#define DEFINE_COLUMN(name, type, attribute, def) \
				columns[TableEnumClass::name].lpszColumnName = #name; \
				columns[TableEnumClass::name].eType = type; \
				columns[TableEnumClass::name].dwAttribute = attribute; \
				columns[TableEnumClass::name].dwDefault = (DWORD_PTR)def; \
				name.iColumnValue = TableEnumClass::name;



#define END_TABLE(table_class) \
				bInited = true; \
			} \
			return columns; \
		} \
		std::string GetCreateTableStr(LPCSTR lpszDBName = "") \
		{ \
			return CTableBase::GetCreateTableStrImpl(GetTableName(), Get(), TableEnumClass::TableColumnCount, lpszDBName); \
		} \
		std::string GetDropTableStr(LPCSTR lpszDBName = "") \
		{ \
			return CTableBase::GetDropTableStrImpl(GetTableName(), lpszDBName); \
		} \
		TableDescriptor GetTablePtr() \
		{ \
			TableDescriptor td = { GetTableName(), Get(), TableEnumClass::TableColumnCount, IsSyncTable() }; \
			return td; \
		} \
		std::string GetRepairTableIndexStr(LPCSTR lpszDBName = "") \
		{ \
			return CTableBase::GetRepairTableIndexStringImpl(GetTableName(), Get(), TableEnumClass::TableColumnCount, lpszDBName); \
		} \
	}  T_##table_class;


#define BEGIN_TABLE(table_name_in_db, alias) \
	BEGIN_TABLE_IMPL(table_name_in_db, T_##alias)

#define BEGIN_SYNC_TABLE(table_name_in_db, alias) \
	BEGIN_SYNC_TABLE_IMPL(table_name_in_db, T_##alias)


#define MAKE_ANOTHER_TABLE_INSTANCE_IMPL(table_name_in_db, alias, new_alias) \
	__declspec(selectany) class_##alias T_##new_alias(table_name_in_db);

#define MAKE_ANOTHER_TABLE_INSTANCE(table_name_in_db, alias, new_alias) \
	MAKE_ANOTHER_TABLE_INSTANCE_IMPL(table_name_in_db, T_##alias, new_alias)


#define TBL(x)			(x.GetTablePtr())

//////////////////////////////////////////////////////////////////////////
