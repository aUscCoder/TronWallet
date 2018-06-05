// Author: guanlingfeng
#pragma once
#include "json2.h"


namespace JsonUtil
{
	inline bool asObjectGetString(const Json::JsonValue *pVal, const std::wstring &strKey, std::wstring &strValue)
	{
		if (pVal)
		{
			const Json::JsonObject *pObj = pVal->asObject();
			if (pObj)
			{
				Json::JsonValue *pValue = pObj->get(strKey);
				if (pValue)
				{
					return pValue->asString(strValue);
				}
			}
		}
		return false;
	}

	inline bool asObjectGetNumber(const Json::JsonValue *pVal, const std::wstring &strKey, double &dValue)
	{
		if (pVal)
		{
			const Json::JsonObject *pObj = pVal->asObject();
			if (pObj)
			{
				const Json::JsonValue *pValue = pObj->get(strKey);
				if (pValue)
				{
					return pValue->asNumber(&dValue);
				}
			}
		}
		return false;
	}

	inline bool asObjectGetNumber(const Json::JsonValue *pVal, const std::wstring &strKey, int *iValue)
	{
		if (pVal)
		{
			const Json::JsonObject *pObj = pVal->asObject();
			if (pObj)
			{
				Json::JsonValue *pValue = pObj->get(strKey);
				if (pValue)
				{
					return pValue->asNumber(iValue);
				}
			}
		}
		return false;
	}

	inline bool asObjectGetNumber(const Json::JsonValue *pVal, const std::wstring &strKey, unsigned int *uiValue)
	{
		if (pVal)
		{
			const Json::JsonObject *pObj = pVal->asObject();
			if (pObj)
			{
				Json::JsonValue *pValue = pObj->get(strKey);
				if (pValue)
				{
					return pValue->asNumber(uiValue);
				}
			}
		}
		return false;
	}

	inline bool asObjectGetNumber(const Json::JsonValue *pVal, const std::wstring &strKey, long *lValue)
	{
		if (pVal)
		{
			const Json::JsonObject *pObj = pVal->asObject();
			if (pObj)
			{
				Json::JsonValue *pValue = pObj->get(strKey);
				if (pValue)
				{
					return pValue->asNumber(lValue);
				}
			}
		}
		return false;
	}

	inline bool asObjectGetNumber(const Json::JsonValue *pVal, const std::wstring &strKey, unsigned long *ulValue)
	{
		if (pVal)
		{
			const Json::JsonObject *pObj = pVal->asObject();
			if (pObj)
			{
				Json::JsonValue *pValue = pObj->get(strKey);
				if (pValue)
				{
					return pValue->asNumber(ulValue);
				}
			}
		}
		return false;
	}

	inline bool asObjectGetBoolean(const Json::JsonValue *pVal, const std::wstring &strKey, bool &bValue)
	{
		if (pVal)
		{
			const Json::JsonObject *pObj = pVal->asObject();
			if (pObj)
			{
				Json::JsonValue *pValue = pObj->get(strKey);
				if (pValue)
				{
					return pValue->asBoolean(&bValue);
				}
			}
		}
		return false;
	}

	inline bool asObjectGetArray(const Json::JsonValue *pVal, const std::wstring &strKey, Json::JsonArray* &pArrValue)
	{
		if (pVal)
		{
			const Json::JsonObject *pObj = pVal->asObject();
			if (pObj)
			{
				Json::JsonValue *pValue = pObj->get(strKey);
				if (pValue)
				{
					return pValue->asArray(&pArrValue);
				}
			}
		}
		return false;
	}

	inline bool asObjectGetObject(const Json::JsonValue *pVal, const std::wstring &strKey, Json::JsonObject* &pObjValue)
	{
		if (pVal)
		{
			const Json::JsonObject *pObj = pVal->asObject();
			if (pObj)
			{
				Json::JsonValue *pValue = pObj->get(strKey);
				if (pValue)
				{
					return pValue->asObject(&pObjValue);
				}
			}
		}
		return false;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline bool asArrayGetString(const Json::JsonValue *pVal, const size_t &index, std::wstring &strValue)
	{
		if (pVal)
		{
			const Json::JsonArray *pArr = pVal->asArray();
			if (pArr)
			{
				Json::JsonValue *pValue = pArr->get(index);
				if (pValue)
				{
					return pValue->asString(strValue);
				}
			}
		}
		return false;
	}

	inline bool asArrayGetNumber(const Json::JsonValue *pVal, const size_t &index, double &dValue)
	{
		if (pVal)
		{
			const Json::JsonArray *pArr = pVal->asArray();
			if (pArr)
			{
				Json::JsonValue *pValue = pArr->get(index);
				if (pValue)
				{
					return pValue->asNumber(&dValue);
				}
			}
		}
		return false;
	}

	inline bool asArrayGetNumber(const Json::JsonValue *pVal, const size_t &index, int &iValue)
	{
		if (pVal)
		{
			const Json::JsonArray *pArr = pVal->asArray();
			if (pArr)
			{
				Json::JsonValue *pValue = pArr->get(index);
				if (pValue)
				{
					return pValue->asNumber(&iValue);
				}
			}
		}
		return false;
	}

	inline bool asArrayGetNumber(const Json::JsonValue *pVal, const size_t &index, unsigned int &uiValue)
	{
		if (pVal)
		{
			const Json::JsonArray *pArr = pVal->asArray();
			if (pArr)
			{
				Json::JsonValue *pValue = pArr->get(index);
				if (pValue)
				{
					return pValue->asNumber(&uiValue);
				}
			}
		}
		return false;
	}

	inline bool asArrayGetNumber(const Json::JsonValue *pVal, const size_t &index, long &lValue)
	{
		if (pVal)
		{
			const Json::JsonArray *pArr = pVal->asArray();
			if (pArr)
			{
				Json::JsonValue *pValue = pArr->get(index);
				if (pValue)
				{
					return pValue->asNumber(&lValue);
				}
			}
		}
		return false;
	}

	inline bool asArrayGetNumber(const Json::JsonValue *pVal, const size_t &index, unsigned long &ulValue)
	{
		if (pVal)
		{
			const Json::JsonArray *pArr = pVal->asArray();
			if (pArr)
			{
				Json::JsonValue *pValue = pArr->get(index);
				if (pValue)
				{
					return pValue->asNumber(&ulValue);
				}
			}
		}
		return false;
	}

	inline bool asArrayGetBoolean(const Json::JsonValue *pVal, const size_t &index, bool &bValue)
	{
		if (pVal)
		{
			const Json::JsonArray *pArr = pVal->asArray();
			if (pArr)
			{
				Json::JsonValue *pValue = pArr->get(index);
				if (pValue)
				{
					return pValue->asBoolean(&bValue);
				}
			}
		}
		return false;
	}

	inline bool asArrayGetArray(const Json::JsonValue *pVal, const size_t &index, Json::JsonArray* &arrValue)
	{
		if (pVal)
		{
			const Json::JsonArray *pArr = pVal->asArray();
			if (pArr)
			{
				Json::JsonValue *pValue = pArr->get(index);
				if (pValue)
				{
					return pValue->asArray(&arrValue);
				}
			}
		}
		return false;
	}

	inline bool asArrayGetObj(const Json::JsonValue *pVal, const size_t &index, Json::JsonObject* &objValue)
	{
		if (pVal)
		{
			const Json::JsonArray *pArr = pVal->asArray();
			if (pArr)
			{
				Json::JsonValue *pValue = pArr->get(index);
				if (pValue)
				{
					return pValue->asObject(&objValue);
				}
			}
		}
		return false;
	}

	namespace SimpleJsonObj
	{
		template<typename char_type>
		inline void push_string(const char_type *lpStr, se::basic_stringBuffer<char_type> &sbDst)
		{
			for (; *lpStr != 0; ++lpStr)
			{
				switch (*lpStr)
				{
				case '\b': sbDst << '\\' << 'b'; break;
				case '\f': sbDst << '\\' << 'f'; break;
				case '\n': sbDst << '\\' << 'n'; break;
				case '\r': sbDst << '\\' << 'r'; break;
				case '\t': sbDst << '\\' << 't'; break;
				case '\\': sbDst << '\\' << '\\'; break;
				case '"': sbDst << '\\' << '"'; break;
				default: sbDst << *lpStr; break;
				}
			}
		}

		template<typename char_type>
		inline void Begin(se::basic_stringBuffer<char_type> &sbJson)
		{
			sbJson << '{';
		}

		template<typename char_type, typename VT>
		inline bool AddField(se::basic_stringBuffer<char_type> &sbJson, const char_type *lpName, VT v)
		{
			sbJson << '"' << lpName << '"' << ':' << v << ',';
			return true;
		}

		template<typename char_type>
		inline bool AddField(se::basic_stringBuffer<char_type> &sbJson, const char_type *lpName, const char_type *v)
		{
			sbJson << '"' << lpName << '"' << ':' << '"';
			push_string(v, sbJson);
			sbJson << '"' << ',';
			return true;
		}

		template<typename char_type>
		inline void End(se::basic_stringBuffer<char_type> &sbJson)
		{
			if (sbJson.length() > 1)
				sbJson.crop(1); // 把最后的','去掉
			sbJson << '}';
		}
	} // SimpleJsonObj
}
