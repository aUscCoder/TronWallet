// Author: WuJian
#pragma once
#include "Types.h"

class CSelectResult
{
public:
	CSelectResult(const TableDescriptor &td) : m_pQ(NULL), m_pSelectOrder(NULL), m_td(td) {} // NOLINT (3.1.3 这个是需要的)
	UNIVERSAL_LIB_API ~CSelectResult();

	void Bind(CSQLite3Query *p, int *pSelectOrder);

	inline bool eof() { return m_pQ ? m_pQ->eof() : true; }
	inline void nextRow() { m_pQ->nextRow(); }

	inline bool IsValidSelectIndex(int i)
	{
		ATLASSERT(m_pSelectOrder[i] != -1);
		return m_pSelectOrder[i] != -1;
	}

	inline LPCSTR getStringField(Column i)
	{
		ATLASSERT(m_td.pColumns[i.iColumnValue].eType == CT_String || m_td.pColumns[i.iColumnValue].eType == CT_Varchar);
		
		if (IsValidSelectIndex(i.iColumnValue))
			return m_pQ->getStringField(m_pSelectOrder[i.iColumnValue]);

		return "";
	}

	inline int getIntField(Column i)
	{
		ATLASSERT(m_td.pColumns[i.iColumnValue].eType == CT_Int || m_td.pColumns[i.iColumnValue].eType == CT_Tinyint
			|| m_td.pColumns[i.iColumnValue].eType == CT_UInt);

		if (IsValidSelectIndex(i.iColumnValue))
			return m_pQ->getIntField(m_pSelectOrder[i.iColumnValue]);
		return 0;
	}

	inline __int64 getInt64Field(Column i)
	{
		ATLASSERT(m_td.pColumns[i.iColumnValue].eType == CT_Int64);

		if (IsValidSelectIndex(i.iColumnValue))
			return m_pQ->getInt64Field(m_pSelectOrder[i.iColumnValue]);
		return 0;
	}
	
	inline _Blob getBlobField(Column i)
	{
		ATLASSERT(m_td.pColumns[i.iColumnValue].eType == CT_Blob);
		SQLite3BLOB blobData;
		
		if (IsValidSelectIndex(i.iColumnValue))
		{
			blobData = m_pQ->getBlobField(m_pSelectOrder[i.iColumnValue]);
			return _Blob(blobData.pBlob ? blobData.pBlob : "", blobData.nSize);
		}
	    return _Blob("", 0);
	}

private:

	CSQLite3Query *m_pQ;
	TableDescriptor m_td;
	int *m_pSelectOrder;
};

using CSelectResultAutoRelease = std::unique_ptr<CSelectResult>;