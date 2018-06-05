// Author: WuJian
#pragma once
#include "Clause.h"
#include "Types.h"


//
// Oprator 各种操作的基类
//

class COperator
{
public:
	COperator(CSQLite3DB *pDB, const TableDescriptor &tab, LPCSTR lpszDBName = "");
	UNIVERSAL_LIB_API virtual ~COperator();

	inline LPCSTR GetSQL() const { return m_sb.str(); }
	inline LPCSTR GetTableName() const { return m_sbFullTableName.str(); }
	
	inline se::stringbuf& GetSB() { return m_sb; }
	inline BlobVec& GetBlobVec() { return m_vecBlobs; }
	inline SQLiteFuncMap& GetFuncMap() { return m_mapFuncs; }

	UNIVERSAL_LIB_API int Exec();				// 执行
	virtual void AfterExec() {}

	UNIVERSAL_LIB_API void PushExpr(CConditionTree &t);

protected:

	void Clear();
	void BeforeExec();
	int BindBlob(CSQLite3BindBlob &binder, int iPos, const _Blob &blob);

	UNIVERSAL_LIB_API bool IsSyncableColumnAffacted() const;
	UNIVERSAL_LIB_API bool IsDirtyFlagAffacted() const;

	void RegisterWhere(CConditionTree &t);
	UNIVERSAL_LIB_API void ImplWhere();

	virtual void MakeSQLStr() = 0;
	void RegisterUserDefinedFuncs();

protected:

	CSQLite3DB *m_pDB;
	se::stringbuf m_sbFullTableName;

	se::stringbuf m_sb;
	TableDescriptor m_td;
	std::set<int> m_AffactedColumns;

	BlobVec m_vecBlobs;
	std::vector<CConditionTree> m_vecValueExpr;
	SQLiteFuncMap m_mapFuncs;

	bool m_bWhere;
	CConditionTree m_where;

	//
	//  如果是select fav::id, fav::pid, fav::url from fav，那么
	//  ValueOrder[fav::id]=>0, ValueOrder[fav::pid]=>1, ValueOrder[fav::url]=>2
	//  KeyOrder[0]=>fav::id, KeyOrder[1]=>fav::pid, KeyOrder[2]=>fav::url
	//
	int m_iKeyCount;
	int* m_pValueOrder;
	int* m_pKeyOrder;
};
