// Author: TuotuoXP
#pragma once


struct ZipDataInput
{
	const BYTE *pZipMem;
	int iZipSize;
	ZipDataInput() : pZipMem(NULL), iZipSize(0) {}
	ZipDataInput(const BYTE *p, int i) : pZipMem(p), iZipSize(i) {}
};


class CUnZipOP
{


#define ZIP_ENTRY_END			0x08074b50
#define ZIP_END_SIGN			0x06054b50
public:

	CUnZipOP();
	virtual ~CUnZipOP();

	// 返回值表示这个文件是否计入文件个数统计（ProcessedFileCount）
	virtual bool OnEachFile(LPCSTR lpszFileName, const BYTE *pCompressedData, DWORD dwOriginalFileSize,
		DWORD dwCompressedFileSize, DWORD dwCRC, bool bUsingStoreMode) = 0;

	typedef std::pair<const BYTE *, const BYTE*> ZipFileHeadEndPair;
	const std::stack<ZipFileHeadEndPair> ZipCompressedSize(const BYTE *pZipHeaderData, const BYTE *pZipEnd);
	const BYTE *ZipEntryEndBYTE(const BYTE *pZipCurrentPos, const BYTE *pZipEndPos);
	void DoUnzip(const ZipDataInput &zipData);

	int GetProcessedFileCount() const { return m_iProcessedFile; }
	void StopProcessing() { m_bWorking = false; }

protected:

	bool DoUncompress(const BYTE *pCompressedData, BYTE *pOutUncompressedData, DWORD dwOriginalFileSize,
		DWORD dwCompressedFileSize, DWORD dwCRC, bool bIgnoreCRC);

private:

	int m_iProcessedFile;
	bool m_bWorking;
};

//////////////////////////////////////////////////////////////////////////

class CZipDataInput_FromFile : public ZipDataInput
{

public:

	explicit CZipDataInput_FromFile(LPCWSTR lpszFileName);
	~CZipDataInput_FromFile();

private:

	HANDLE m_hFile;
	HANDLE m_hZipFileMapping;
};

