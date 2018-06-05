// Author: WuJian
#pragma once
#include "../../Universal.h"

namespace Universal
{
	namespace EncryptWall
	{
		struct WallKey;

		// 用 AES + RSA 加密一个 HTTP 请求

		// lpszURL : 不带 ? 和 GET 参数的 url path，如 http://ping.ie.sogou.com/ping.gif
		// lpszParam : GET 参数
		// lpPostData : POST 参数
		// 如果bIgnoreKey为True，接口就不返回Key了
		// 返回的Key自己delete掉

		UNIVERSAL_LIB_API WallKey* EncryptHttpRequest(LPCSTR lpszUrl, LPCSTR lpszGetParam,
			const BYTE *lpPostData, DWORD dwPostDataLength, se::stringbuf &sbNewUrl, se::stringbuf &strNewPostData);

		//
		// [WuJian] 这个函数用于解密加密墙返回的内容
		//	SessionID是加密的时候返回的，自己留着
		//	lpResponse是返回的Body
		//	dwResponseSize是返回的BodyLength
		//	dwBodySize是解密后的Body大小
		//	返回值是解密后的Body
		//
		UNIVERSAL_LIB_API BYTE* DecryptHttpRequest(const WallKey* pKey, BYTE *lpResponse, DWORD dwResponseSize, DWORD *dwBodySize);

		UNIVERSAL_LIB_API void FreeKey(WallKey *pKey);
	}
}


