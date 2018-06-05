//Author: TuotuoXP
#pragma once
#include "../../Universal.h"
#include "cppdom/cppdom.h"


class XMLUtilContext;


namespace Universal
{
	namespace XMLUtil
	{
		UNIVERSAL_LIB_API bool CreateXMLParserContext(LPCWSTR lpszFileName, cppdom::Document **ppDoc);
		UNIVERSAL_LIB_API bool CreateXMLParserContext(LPCSTR lpszData, cppdom::Document **ppDoc);
		UNIVERSAL_LIB_API void ReleaseXMLParserContext(cppdom::Document *pDoc);
		UNIVERSAL_LIB_API std::string XMLEscape(const std::string& strData);
	}
}


class XMLUtilContext
{

public:

	XMLUtilContext() : m_pDoc(NULL) {}
	~XMLUtilContext()
	{
		if (m_pDoc)
			Universal::XMLUtil::ReleaseXMLParserContext(m_pDoc);
	}


	cppdom::Document *m_pDoc;	// NOLINT (3.3.3 方便使用)
};
