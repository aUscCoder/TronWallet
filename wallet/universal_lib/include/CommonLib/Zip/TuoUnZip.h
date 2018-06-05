// Author: TuotuoXP
#pragma once
#include "../../Universal.h"


typedef std::map<std::string, std::pair<BYTE*, int>> UnzipFileData;
typedef std::map<std::string, std::pair<HANDLE, int>> UnzipFileMapping;

struct AutoDeleteUnzipFileData : UnzipFileData
{
	~AutoDeleteUnzipFileData()
	{
		Destroy();
	}
	void Destroy()
	{
		for (iterator it = begin(); it != end(); it++)
			if (it->second.first)
				delete it->second.first;
		clear();
	}
};

struct AutoDeleteUnzipFileMapping : UnzipFileMapping
{
	~AutoDeleteUnzipFileMapping()
	{
		Destroy();
	}
	void Destroy()
	{
		for (iterator it = begin(); it != end(); it++)
			if (it->second.first)
				::CloseHandle(it->second.first);
		clear();
	}
};


struct ZipRawData
{
	int iCompressedSize;
	int iOriginalSize;
	BYTE *pSrcData;
	bool bUsingStoreMode;
	DWORD dwCRC;
	std::string strFileName;
};
typedef std::list<ZipRawData> ZipRawDataList;


namespace Universal
{
	namespace ZipUtil
	{
		UNIVERSAL_LIB_API void Uncompress(const BYTE *pSrcData, int iCompressedSize, int iOriginalSize,
			bool bUseStoreMode, BYTE *pOutputData, DWORD *pdwCRC);

		// 返回指定文件，需要把想返回的文件名提前写到UnzipFileData/UnzipFileMapping中
		UNIVERSAL_LIB_API int UnzipSpecificFilesFromFileToMemoryOrMapping(LPCWSTR lpszZipFile, UnzipFileData &data, UnzipFileMapping &dataMapping);
		// 下面两个函数返回的buffer需要delete
		UNIVERSAL_LIB_API bool GetSpecificFileFromZip(LPCWSTR lpszFileName, BYTE **pOutputData, int *pOutputBufferLen, LPCSTR lpszSpecificFile);
		UNIVERSAL_LIB_API bool GetSpecificFileFromZipMemory(const BYTE *pZipMem, int iZipSize, BYTE **pOutputData, int *pOutputBufferLen,
			LPCSTR lpszSpecificFile);
		
		// 解压zip到目录
		UNIVERSAL_LIB_API int UnzipFileToDir(LPCWSTR lpszZipFile, LPCWSTR lpszTargetDir, bool bIgnoreCRC = false);
		UNIVERSAL_LIB_API int UnZipMemoryToDir(const BYTE *pZipMem, int iZipSize, LPCWSTR lpszTargetDir, bool bIgnoreCRC = false);
		// 如果需要自动删除buffer，请使用AutoDeleteUnzipFileData
		UNIVERSAL_LIB_API int UnzipFromFileToMemory(LPCWSTR lpszZipFile, UnzipFileData &data);
		UNIVERSAL_LIB_API int UnzipFromFileToMapping(LPCWSTR lpszZipFile, UnzipFileMapping &data);
		UNIVERSAL_LIB_API int UnzipFromMemoryToMemory(const BYTE *pZipMem, int iZipSize, UnzipFileData &data);
		UNIVERSAL_LIB_API int UnzipFromMemoryToMapping(const BYTE *pZipMem, int iZipSize, UnzipFileMapping &data);

		// 只把数据分离出来，不做解压
		UNIVERSAL_LIB_API int DumpRawDataFromFile(LPCWSTR lpszZipFile, ZipRawDataList &data);
	}
}
