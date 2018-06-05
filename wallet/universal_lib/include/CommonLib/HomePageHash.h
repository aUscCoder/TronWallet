// Author: ChenJianjun
#pragma once
#include "String/Convert.h"
#include "CypherHash/CRC.h"
#include "String/StringHelper.h"


inline int GenericHomePageHashCode(const std::string& strHID, int iHomePageType, const std::wstring& strUserDefinedURL)
{
	se::stringbuf sb;
	sb << strHID << " " << iHomePageType;
	if (iHomePageType == 3)	// HomePageType_UserDefined
		sb << WToA(ToLower(strUserDefinedURL));

	return crc32(0, (const unsigned char*)sb.str(), sb.length());
}

