///
/// @author chenjianjun
/// @brief 注册表操作函数
///
#pragma once
#include "../../Universal.h"


namespace Universal
{
	namespace Reg
	{
		struct BINARY
		{
			BYTE* pData;
			DWORD cbData;

			BINARY(BYTE* p = NULL, DWORD cb = 0) : pData(p), cbData(cb) {}
			~BINARY()
			{
				if (pData)
					delete[] pData;
			}

			bool operator == (const BINARY& bin) const // NOLINT
			{
				if (!pData && !bin.pData)
					return true;

				if (!pData || !bin.pData)
					return false;

				return cbData == bin.cbData && memcmp(pData, bin.pData, cbData) == 0;
			}
		};

		///
		/// @brief 读取注册表字符串
		/// @param hKeyRoot HKEY_CLASSES_ROOT 或 HKEY_CURRENT_USER 或 HKEY_LOCAL_MACHINE HKEY_USERS 或 HKEY_CURRENT_CONFIG
		/// @param lpszSubKey 注册表路径
		/// @param lpszValue 注册表中要读取的键
		/// @return 返回字符串数据
		///
		UNIVERSAL_LIB_API std::string GetString(HKEY hKeyRoot, LPCSTR lpszSubKey, LPCSTR lpszValue);

		///
		/// @brief 读取注册表字符串
		/// @param hKeyRoot HKEY_CLASSES_ROOT 或 HKEY_CURRENT_USER 或 HKEY_LOCAL_MACHINE HKEY_USERS 或 HKEY_CURRENT_CONFIG
		/// @param lpszSubKey 注册表路径
		/// @param lpszValue 注册表中要读取的键
		/// @return 返回字符串数据
		///
		UNIVERSAL_LIB_API std::wstring GetString(HKEY hKeyRoot, LPCWSTR lpszSubKey, LPCWSTR lpszValue);

		///
		/// @brief 读取注册表DWORD
		/// @param hKeyRoot HKEY_CLASSES_ROOT 或 HKEY_CURRENT_USER 或 HKEY_LOCAL_MACHINE HKEY_USERS 或 HKEY_CURRENT_CONFIG
		/// @param lpszSubKey 注册表路径
		/// @param lpszValue 注册表中要读取的键
		/// @return 返回DWORD
		///
		UNIVERSAL_LIB_API DWORD GetDWORD(HKEY hKeyRoot, LPCWSTR lpszSubKey, LPCWSTR lpszValue, DWORD dwDefault = 0);

		///
		///	@brief 获取注册表的二进制数据
		/// @param hKeyRoot HKEY_CLASSES_ROOT 或 HKEY_CURRENT_USER 或 HKEY_LOCAL_MACHINE HKEY_USERS 或 HKEY_CURRENT_CONFIG
		/// @param lpszSubKey 注册表路径
		/// @param lpszValue 注册表中要读取的键
		/// @param binResult 返回结果
		/// @return 是否成功
		///
		UNIVERSAL_LIB_API bool GetBinary(HKEY hKeyRoot, LPCWSTR lpszSubKey, LPCWSTR lpszValue, BINARY& binResult);

		UNIVERSAL_LIB_API bool GetBinary(HKEY hKeyRoot, LPCWSTR lpszSubKey, LPCWSTR lpszValue, BYTE** ppData, DWORD* pcbData);

		UNIVERSAL_LIB_API bool GetBinary(HKEY hKeyRoot, LPCWSTR lpszSubKey, LPCWSTR lpszValue, BYTE* pData, DWORD* pcbData);

		///
		/// @brief
		///
		UNIVERSAL_LIB_API bool EnumKey(HKEY hKeyRoot, LPCWSTR lpszSubKey, std::vector<std::wstring>& vecResult);

		///
		/// @brief
		///
		UNIVERSAL_LIB_API bool EnumValue(HKEY hKeyRoot, LPCWSTR lpszSubKey, std::vector<std::wstring>& vecResult);

		///
		/// @brief 设置注册表字符串值
		/// @param hKeyRoot HKEY_CLASSES_ROOT 或 HKEY_CURRENT_USER 或 HKEY_LOCAL_MACHINE HKEY_USERS 或 HKEY_CURRENT_CONFIG
		/// @param lpszSubKey 注册表路径
		/// @param lpszValue 注册表中要设置的键
		/// @param strData 要设置的字符串
		/// @return 是否成功
		///
		UNIVERSAL_LIB_API bool SetString(HKEY hKeyRoot, LPCWSTR lpszSubKey, LPCWSTR lpszValue, const std::wstring& strData);

		///
		/// @brief
		/// @param hKeyRoot HKEY_CLASSES_ROOT 或 HKEY_CURRENT_USER 或 HKEY_LOCAL_MACHINE HKEY_USERS 或 HKEY_CURRENT_CONFIG
		/// @param lpszSubKey 注册表路径
		/// @param lpszValue 注册表中要设置的键
		/// @return 是否成功
		///
		UNIVERSAL_LIB_API bool SetString(HKEY hKeyRoot, LPCSTR lpszSubKey, LPCSTR lpszValue, const std::string& strData);

		///
		/// @brief
		///
		UNIVERSAL_LIB_API bool SetDWORD(HKEY hKeyRoot, LPCWSTR lpszSubKey, LPCWSTR lpszValue, DWORD dwData);

		///
		/// @brief
		///
		bool SetBinary(HKEY hKeyRoot, LPCWSTR lpszSubKey, LPCWSTR lpszValue, const BINARY& binData);

		UNIVERSAL_LIB_API bool SetBinary(HKEY hKeyRoot, LPCWSTR lpszSubKey, LPCWSTR lpszValue, void* pData, DWORD cbData);

		///
		/// @brief
		///
		bool SetBinaryWithType(HKEY hKeyRoot, LPCWSTR lpszSubKey, LPCWSTR lpszValue, const BINARY& binData, DWORD dwType);

		///
		/// @brief
		///
		UNIVERSAL_LIB_API bool CreateKey(HKEY hKeyRoot, LPCWSTR lpszSubKey, HKEY* pOut = NULL);

		///
		/// @brief
		///
		UNIVERSAL_LIB_API bool CreateKey(HKEY hKeyRoot, LPCSTR lpszSubKey, HKEY* pOut = NULL);

		///
		/// @brief
		///
		UNIVERSAL_LIB_API bool HasValue(HKEY hKeyRoot, LPCWSTR lpszSubKey, LPCWSTR lpszValue);

		///
		/// @brief
		///
		UNIVERSAL_LIB_API bool HasKey(HKEY hKeyRoot, LPCWSTR lpszSubKey);

		///
		/// @brief
		///
		UNIVERSAL_LIB_API bool DeleteValue(HKEY hKeyRoot, LPCWSTR lpszSubKey, LPCWSTR lpszValue);

		///
		/// @brief
		///
		UNIVERSAL_LIB_API bool DeleteKey(HKEY hKeyRoot, LPCWSTR lpszSubKey);

		///
		/// @brief
		///
		UNIVERSAL_LIB_API bool ImportRegFromFile(LPCWSTR lpszRegFile, bool bDeleteExists = false);

	}
}
