// Author: wujian
#pragma once
#include "../../Universal.h"


struct ConfBase
{
	HANDLE hMapping;
	DWORD dwLength;
	DWORD dwVersion;
	__int64 iMaxServerId;
};


struct ConfData : public ConfBase
{
protected:
	mutable const BYTE *pData;
	bool bAutoDelete;
public:

	ConfData() : bAutoDelete(false), pData(NULL) { hMapping = NULL; dwLength = dwVersion = 0; iMaxServerId = 0; }
	ConfData(const ConfData &b) { *this = b; }
	ConfData& operator =(const ConfData &b)
	{
		memcpy(this, &b, sizeof(ConfData));
		pData = NULL;
		return *this;
	}
	~ConfData() { Free(); }
	void Free(bool bForce = false)
	{
		if (pData)
		{
			::UnmapViewOfFile(pData);
			pData = NULL;
		}
		if ((bForce || bAutoDelete) && hMapping)
		{
			::CloseHandle(hMapping);
			hMapping = NULL;
		}
	}

	const BYTE* GetDataPtr() const
	{
		if (pData == NULL && hMapping)
			pData = (const BYTE*)::MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
		return pData;
	}
};

typedef std::map<std::string, ConfData> SogouConfMap;

namespace Universal
{
	namespace SogouConf
	{
		UNIVERSAL_LIB_API bool ExtractAllData(LPCWSTR lpszSogouConfFile, SogouConfMap &confData, bool bCalcMaxServerID);
		UNIVERSAL_LIB_API bool ExtractSingleData(LPCWSTR lpszSogouConfFile, LPCSTR lpszConfName, ConfData &conf);

		UNIVERSAL_LIB_API void GenerateSogouConf(LPCWSTR lpszSogouConfFile, const SogouConfMap &confData);
	}
}
