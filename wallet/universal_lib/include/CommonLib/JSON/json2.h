// NOLINT (all)
// Author: lixin
#pragma once
#include "../../Universal.h"

namespace Json 
{
	class JsonValue;
	class JsonObject;
	class JsonArray;

	enum JsonValueType
	{
		JsonValueType_Void,
		JsonValueType_Null,
		JsonValueType_Bool,
		JsonValueType_Number,
		JsonValueType_String,
		JsonValueType_Object,
		JsonValueType_Array,
	};

	class JsonValue
	{
	public:
		virtual ~JsonValue() {}

		static JsonValue* null() {return new JsonValue(JsonValueType_Null);}
		UNIVERSAL_LIB_API static JsonValue* parseJson(const wchar_t* json, int iLen = -1);		
		UNIVERSAL_LIB_API static JsonValue* parseJson(const std::wstring &strJson);
		std::wstring toJsonString() const
		{
			se::wstringbuf sbJson;
			writeJSON(sbJson);
			return sbJson.str();
		}
		UNIVERSAL_LIB_API virtual void writeJSON(se::wstringbuf &output) const;


		JsonValueType type() const { return m_type; }

		bool isNull() const { return m_type == JsonValueType_Null; }

		virtual bool asBoolean(bool* output) const { return false; }
		virtual bool asNumber(double* output) const { return false; }
		virtual bool asNumber(int* output) const { return false; }
		virtual bool asNumber(long* output) const { return false; }
		virtual bool asNumber(unsigned long* output) const { return false; }
		virtual bool asNumber(unsigned int* output) const  {return false; }
		virtual bool asString(std::wstring &output) const { return false; }
		virtual bool asValue(JsonValue **output) { *output = this; return true; }
		virtual bool asObject(JsonObject **output) { return false; }
		virtual bool asObject(const JsonObject **output) const { return false; }
		virtual bool asArray(JsonArray **output) { return false; }
		virtual bool asArray(const JsonArray **output) const { return false; }
		virtual JsonObject* asObject() { return NULL; }
		virtual const JsonObject* asObject() const { return NULL; }
		virtual JsonArray* asArray() { return NULL; }
		virtual const JsonArray* asArray() const { return NULL; }

		virtual JsonValue * duplicate() const { return NULL; }

	protected:
		explicit JsonValue(JsonValueType type) : m_type(type) {}

	private:
		JsonValueType m_type;

	};

	class JsonBasicValue : public JsonValue
	{
	public:
		static JsonBasicValue* create(bool bValue) {return new JsonBasicValue(bValue);}
		static JsonBasicValue* create(int iValue) {return new JsonBasicValue(iValue);}
		static JsonBasicValue* create(double dValue) {return new JsonBasicValue(dValue);}

		virtual bool asBoolean(bool* output) const override
		{
			if (type() != JsonValueType_Bool)
				return false;
			*output = m_boolValue;
			return true;
		}

		virtual bool asNumber(double* output) const override
		{
			if (type() != JsonValueType_Number)
				return false;
			*output = m_doubleValue;
			return true;
		}

		virtual bool asNumber(int* output) const
		{
			if (type() != JsonValueType_Number)
				return false;
			*output = (int)m_doubleValue;
			return true;
		}

		virtual bool asNumber(long* output) const override
		{
			if (type() != JsonValueType_Number)
				return false;
			*output = (long)m_doubleValue;
			return true;
		}

		virtual bool asNumber(unsigned long* output) const override
		{
			if (type() != JsonValueType_Number)
				return false;
			*output = (unsigned long)m_doubleValue;
			return true;
		}

		virtual bool asNumber(unsigned int* output) const override
		{
			if (type() != JsonValueType_Number)
				return false;
			*output = (unsigned int)m_doubleValue;
			return true;
		}

		UNIVERSAL_LIB_API virtual void writeJSON(se::wstringbuf &output) const;

		virtual JsonValue *duplicate() const override
		{
			if (type() == JsonValueType_Number)
				return JsonBasicValue::create(m_doubleValue);
			else
				return JsonBasicValue::create(m_boolValue);
		}


	protected:
		explicit JsonBasicValue (int iVal) : JsonValue(JsonValueType_Number), m_doubleValue(iVal) {}
		explicit JsonBasicValue (double dVal) : JsonValue(JsonValueType_Number), m_doubleValue(dVal) {}
		explicit JsonBasicValue (bool bVal) : JsonValue(JsonValueType_Bool), m_boolValue(bVal) {}

	private:
		union
		{
			bool m_boolValue;
			double m_doubleValue;
		};

	};

	class JsonStringValue : public JsonValue
	{
	public:
		static JsonStringValue* create(LPCWSTR lpszVal) {return new JsonStringValue(lpszVal);}

		virtual bool asString(std::wstring &output) const override
		{
			TuoAssert(type() == JsonValueType_String);
			output = m_strValue;
			return true;
		}

		virtual JsonValue *duplicate() const override
		{
			return JsonStringValue::create(m_strValue.c_str());
		}

		UNIVERSAL_LIB_API virtual void writeJSON(se::wstringbuf &output) const;


	protected:
		explicit JsonStringValue (LPCWSTR lpszVal) : JsonValue(JsonValueType_String), m_strValue(lpszVal) {}

	private:
		std::wstring m_strValue;
	};

	class JsonObject : public JsonValue
	{
	public:
		static JsonObject *create() {return new JsonObject();}

		~JsonObject()
		{
			for (PropertyMap::iterator it = m_mapProperties.begin(); it != m_mapProperties.end(); ++it)
				delete it->second;
		}

		virtual bool asObject(JsonObject **output) override
		{
			TuoAssert(type() == JsonValueType_Object);
			*output = this;
			return true;
		}

		virtual bool asObject(const JsonObject **output) const override
		{
			TuoAssert(type() == JsonValueType_Object);
			*output = this;
			return true;
		}

		virtual JsonObject* asObject() override
		{
			TuoAssert(type() == JsonValueType_Object);
			return this;
		}

		virtual const JsonObject* asObject() const override
		{
			TuoAssert(type() == JsonValueType_Object);
			return this;
		}


		UNIVERSAL_LIB_API virtual JsonValue *duplicate() const;

		typedef std::map<std::wstring, JsonValue*> PropertyMap;
		typedef PropertyMap::iterator iterator;
		typedef PropertyMap::const_iterator const_iterator;

		void setValue(const std::wstring &strName, JsonValue *pValue) {	m_mapProperties[strName] = pValue; }
		void setBoolean(const std::wstring& name, bool bValue) { setValue(name, JsonBasicValue::create(bValue)); }
		void setNumber(const std::wstring& name, double dValue) { setValue(name, JsonBasicValue::create(dValue)); }
		void setString(const std::wstring& name, const std::wstring &strValue) { setValue(name, JsonStringValue::create(strValue.c_str())); }
		void setObject(const std::wstring& name, JsonObject *pObj) { setValue(name, pObj); }
		void setArray(const std::wstring& name, JsonArray *pArr);


		const_iterator find(const std::wstring& name) const {return m_mapProperties.find(name);}
		const_iterator begin() const {return m_mapProperties.begin();}
		const_iterator end() const {return m_mapProperties.end();}

		JsonValue* get(const std::wstring& name) const
		{
			PropertyMap::const_iterator it = m_mapProperties.find(name);
			if (it == m_mapProperties.end())
				return NULL;
			return it->second;
		}


		bool getBoolean(const std::wstring& name, bool* output) const
		{
			JsonValue *pVal = get(name);
			if (pVal)
				return pVal->asBoolean(output);
			return false;
		}

		bool getBoolean(const std::wstring& name, bool bDef = false) const
		{
			bool bRet = bDef;
			getBoolean(name, &bRet);
			return bRet;
		}

		bool getNumber(const std::wstring& name, double* output) const
		{
			JsonValue *pVal = get(name);
			if (pVal)
				return pVal->asNumber(output);
			return false;
		}

		bool getNumber(const std::wstring& name, int* output) const
		{
			JsonValue *pVal = get(name);
			if (pVal)
				return pVal->asNumber(output);
			return false;
		}

		double getNumber(const std::wstring& name, double dDef = 0.0) const
		{
			double dRet = dDef;
			getNumber(name, &dRet);
			return dRet;
		}

		bool getString(const std::wstring& name, std::wstring &output) const
		{
			JsonValue *pVal = get(name);
			if (pVal)
				return pVal->asString(output);
			return false;
		}

		std::wstring getString(const std::wstring& name, LPCWSTR lpszDefault = L"") const
		{
			std::wstring strRet(lpszDefault);
			getString(name, strRet);
			return strRet;
		}

		JsonObject* getObject(const std::wstring& name) const
		{
			JsonValue *pVal = get(name);
			if (pVal && pVal->type() == JsonValueType_Object)
				return static_cast<JsonObject *>(pVal);
			return NULL;
		}

		JsonArray* getArray(const std::wstring& name) const;


		UNIVERSAL_LIB_API virtual void writeJSON(se::wstringbuf &output) const;

	protected:
		explicit JsonObject () : JsonValue(JsonValueType_Object) {}

	private:
		PropertyMap m_mapProperties;
	};

	class JsonArray : public JsonValue
	{
	public:
		static JsonArray *create() {return new JsonArray();}

		~JsonArray() 
		{ 
			for (size_t i = 0; i < m_vecValues.size(); ++i)
				delete m_vecValues[i];
		}

		void pushValue(JsonValue *pValue) {m_vecValues.push_back(pValue);}
		void pushBoolean(bool bValue) {pushValue(JsonBasicValue::create(bValue));}
		void pushNumber(double dValue) {pushValue(JsonBasicValue::create(dValue));}
		void pushString(const std::wstring& strValue) {pushValue(JsonStringValue::create(strValue.c_str()));}
		void pushObject(JsonObject *pObj) {pushValue(pObj);}
		void pushArray(JsonArray *pArr) {pushValue(pArr);}
		size_t length() const { return m_vecValues.size(); }
		JsonValue* get(size_t index)
		{
			TuoAssert(index < m_vecValues.size());
			if (index < m_vecValues.size())
				return m_vecValues[index];
			return NULL;
		}

		JsonValue* get(size_t index) const 
		{
			TuoAssert(index < m_vecValues.size());
			if (index < m_vecValues.size())
				return m_vecValues[index];
			return NULL;
		}


		virtual bool asArray(JsonArray **output) override
		{
			TuoAssert(type() == JsonValueType_Array);
			*output = this;
			return true;
		}

		virtual bool asArray(const JsonArray **output) const override
		{
			TuoAssert(type() == JsonValueType_Array);
			*output = this;
			return true;
		}

		virtual JsonArray* asArray() override
		{
			TuoAssert(type() == JsonValueType_Array);
			return this;
		}

		virtual const JsonArray* asArray() const override
		{
			TuoAssert(type() == JsonValueType_Array);
			return this;
		}


		UNIVERSAL_LIB_API virtual void writeJSON(se::wstringbuf &output) const;

		UNIVERSAL_LIB_API virtual JsonValue *duplicate() const;


	protected:
		explicit JsonArray () : JsonValue(JsonValueType_Array) {}

	private:
		std::vector<JsonValue*> m_vecValues;
	};

	inline JsonValue* parse(const std::wstring &strJson) { return JsonValue::parseJson(strJson);}

	inline std::wstring stringify(JsonValue *pVal) { return pVal->toJsonString();}


	//////////////////////////////////////////////////////////////////////////
	//Other interfaces
	inline void JsonObject::setArray(const std::wstring& name, JsonArray *pArr) { setValue(name, pArr); }


	inline JsonArray* JsonObject::getArray( const std::wstring& name ) const
	{
		JsonValue *pVal = get(name);
		if (pVal && pVal->type() == JsonValueType_Array)
			return static_cast<JsonArray *>(pVal);
		return NULL;
	}
}



