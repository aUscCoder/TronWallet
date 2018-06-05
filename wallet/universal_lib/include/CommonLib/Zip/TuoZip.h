// Author: TuotuoXP
#pragma once
#include "../../Universal.h"
#include "TuoUnZip.h"


struct ZipHeaderData;
struct ZipFooterData;


struct ZipFileData
{
	HANDLE hFile;
	int iCurrentFileSize;
	struct ZipFooterInfo
	{
		ZipFooterData *pZipFooterData;
		int iLength;
	};
	std::vector<ZipFooterInfo> ZipFooterInfoVec;
};

struct ZipBufData
{
	const void *pData;
	int iLen;
	std::string strFileName;
};

typedef std::list<ZipBufData> ZipBufDataList;


namespace Universal
{
	namespace ZipUtil
	{
		UNIVERSAL_LIB_API ZipFileData* CreateZipFile(LPCWSTR lpszZipFile);
		UNIVERSAL_LIB_API ZipFileData* CreateZipFileFromZipFile(LPCWSTR lpszZipFile);
		bool AddRawDataToZip(ZipFileData *pZipData, const ZipRawData *pUsingRawData);
		UNIVERSAL_LIB_API bool AddMemoryToZip(ZipFileData *pZipData, LPCSTR lpszName, const void *pData, int iLen);
		UNIVERSAL_LIB_API bool AddFileToZip(ZipFileData *pZipData, LPCSTR lpszName, LPCWSTR lpszFilePath);
		UNIVERSAL_LIB_API void CloseZipFile(ZipFileData *pZipData);

		UNIVERSAL_LIB_API void AddMemoriesToMemory(BYTE **ppData, int *pOutBufLen, const ZipBufDataList &zipBuf);

		UNIVERSAL_LIB_API bool ReplaceFilesInZip(LPCWSTR lpszZipFile, const ZipBufDataList &zipBuf);
	}
}
