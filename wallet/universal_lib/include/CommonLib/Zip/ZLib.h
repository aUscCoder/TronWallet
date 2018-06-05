// author: TuotuoXP
#pragma once
#include "../../Universal.h"


#define ZIP_HEADER				0x04034b50
#define ZIP_FOOTER				0x02014b50
#define COMPRESS_FLAG			8


#pragma pack(push, 1)


struct ZipHeaderData
{
	DWORD dwHeaderSign;			// 0x04034b50
	WORD wPKWareVersion;
	WORD wGlobalFlag;
	WORD wCompressType;			// 8表示是zlib压缩的，0表示直接存储了
	WORD wLastModifiedTime;
	WORD wLastModifiedDate;
	DWORD dwCRC32;
	DWORD dwCompressedSize;
	DWORD dwOriginalSize;
	WORD wFileNameLen;
	WORD wExtendedLen;
};


struct ZipFooterData
{
	DWORD dwFooterSign;			// 0x02014b50
	WORD wPKWareVersion;
	WORD wUnzipPKWareVersion;
	WORD wGlobalFlag;
	WORD wCompressType;
	WORD wLastModifiedTime;
	WORD wLastModifiedDate;
	DWORD dwCRC32;
	DWORD dwCompressedSize;
	DWORD dwOriginalSize;
	WORD wFileNameLen;
	WORD wExtendedLen;
	WORD wCommentLen;
	WORD wDiskNumber;
	WORD wInternalFileAttribute;
	DWORD dwExternalFileAttribute;
	DWORD dwOffset;
};


struct ZipFileEnd
{
	DWORD dwEndSign;			// 0x06054b50
	WORD wDiskNumber;
	WORD wDirBeginDiskNumber;
	WORD wTotalRecordInDisk;
	WORD wTotalRecord;
	DWORD dwDirSize;
	DWORD dwDirOffset;
	WORD wCommentLen;
};


#pragma pack(pop)


UNIVERSAL_LIB_API void zip_uncompress(const void *pSrc, int iSrcLen, void *pDest, int iDestLen, DWORD *pdwCRC);
UNIVERSAL_LIB_API int zip_compress(const void *pSrc, int iSrcLen, void *pDest, int iDestLen, DWORD &dwCRC);
