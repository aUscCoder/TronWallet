// author: TuotuoXP
#pragma once
#include "../../Universal.h"


typedef std::map<std::string, std::string> IniDataMap;
typedef std::map<std::string, IniDataMap> IniSectionMap;


namespace Universal
{
	namespace INIParser
	{
		UNIVERSAL_LIB_API void ParseINIFile(LPCWSTR lpszFile, IniSectionMap &iniSectionMap);
		UNIVERSAL_LIB_API void ParseINIData(LPCSTR lpszData, int iLen, IniSectionMap &iniSectionMap);
	}
}
