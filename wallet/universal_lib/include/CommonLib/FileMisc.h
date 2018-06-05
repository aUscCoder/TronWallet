// Author: TuotuoXP
#pragma once
#include "../Universal.h"


typedef std::list<WIN32_FIND_DATAW> EnumResultList;
#define ED_FILE		1
#define ED_DIRECTORY	2

namespace Universal
{
	namespace FileMisc
	{
		UNIVERSAL_LIB_API bool IsFileExist(LPCWSTR lpszFile, bool bIncludeDirectory = false/* 是否连目录也算 */);
		UNIVERSAL_LIB_API bool IsDirectory(LPCWSTR lpszPath);
		UNIVERSAL_LIB_API bool IsPathValid(LPCWSTR lpszPath);
		UNIVERSAL_LIB_API bool IsPathWritable(LPCWSTR lpszPath, bool bIsCreateDir = false/* 是否在当前目录创建文件夹 */); // 路径存在的情况下
		UNIVERSAL_LIB_API bool CreateDirectory(LPCWSTR lpszDir);
		UNIVERSAL_LIB_API bool RemoveDirectory(LPCWSTR lpszDir);
		UNIVERSAL_LIB_API void EnumDirectory(LPCWSTR lpszEnumStr, EnumResultList& result, DWORD dwEnumFlag = ED_FILE | ED_DIRECTORY);

		UNIVERSAL_LIB_API std::wstring NormalizePath(LPCWSTR lpszPath);
		UNIVERSAL_LIB_API std::wstring GetSpecialPath(int nFolder);
		UNIVERSAL_LIB_API HICON GetSpecialPathIcon(int nFolder);
		UNIVERSAL_LIB_API std::wstring ExtractFileName(LPCWSTR lpszFullPathName);
		UNIVERSAL_LIB_API std::wstring GetValidFileName(LPCWSTR lpszFileName);

		UNIVERSAL_LIB_API bool GetFileVersion(LPCWSTR lpszFileName, VerInfo &ver);
		UNIVERSAL_LIB_API bool GetFileTime(LPCWSTR lpszFileName, LPFILETIME lpCreationTime, LPFILETIME lpLastAccessTime, LPFILETIME lpLastWriteTime);
		UNIVERSAL_LIB_API bool SetFileTime(LPCWSTR lpszFileName, LPFILETIME lpCreationTime, LPFILETIME lpLastAccessTime, LPFILETIME lpLastWriteTime);

		UNIVERSAL_LIB_API UINT GetInvalidCharPosInTitle(const std::wstring &wstr);

		UNIVERSAL_LIB_API LPSTR GetFileContent(LPCWSTR lpszFilePath, DWORD *pdwSize = NULL);
		UNIVERSAL_LIB_API bool SetFileContent(LPCWSTR lpszFilePath, const void *lpData, int iSize);
		UNIVERSAL_LIB_API std::wstring PathCombine(const wchar_t* pszDir, const wchar_t* pszFile);

		UNIVERSAL_LIB_API std::wstring GetFileExtensionName(const std::wstring &strFileName);
		UNIVERSAL_LIB_API bool FileBelongsToTypes(const std::wstring &strFileName, const std::vector<std::wstring> &vecTypes);
		UNIVERSAL_LIB_API DWORD GetFileSize32(const std::wstring &strFilePath);

		/*!
		* 判断某目录是否为另一目录的子目录
		* 支持传入相对路径, 如果目录自身进行判断, 返回值为 true
		*
		* \lpszSub 子目录
		* \lpszParent 父目录
		*
		*/
		UNIVERSAL_LIB_API bool IsSubFolder(LPCWSTR lpszSub, LPCWSTR lpszParent);
	}
}
