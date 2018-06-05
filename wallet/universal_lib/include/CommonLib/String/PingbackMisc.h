// Author: WuJian
#pragma once
#include "../../Universal.h"
#include "Convert.h"

namespace Universal
{
	namespace PingbackMisc
	{
		UNIVERSAL_LIB_API std::string PingbackEncrypt(LPCSTR lpszUrl, DWORD dwLength);


		// 用 AES + RSA 加密一个 HTTP 请求

		// lpszURL : 不带 ? 和 GET 参数的 url path，如 http://ping.ie.sogou.com/ping.gif
		// lpszParam : GET 参数
		// lpPostData : POST 参数

		// 加密步骤：
		// a. 令原始的 HTTP 请求中的三部分：不带 GET 参数的 url path 为 A， GET 参数为 B， POST 数据为 C
		// b. 对 B 或 C，先做 zip 压缩
		// c. 分别对 A 或压缩后的 B 或 C，先用 AES 算法对他们进行加密（ AES 密钥随机产生），加密结果为 U 或 G 或 P
		// d. 对上一步中使用的 AES 密钥，使用 RSA 算法进行加密，加密结果为 UK 或 GK 或 PK

		// 在传输时：
		// 将请求发送至 http://cloud.ie.sogou.com/secret.php（暂定），
		// 不带 GET 参数，带上 POST 数据，其格式为 u=<U>&uk=<UK>&g=<G>&gk=<GK>&p=<P>&pk=<PK>
		// 其中，u/g/p/uk/gk/pk 为上一步中生成的 U/G/P/UK/GK/PK 做 base64 编码后的结果，u 和 uk、g 和 gk、p 和 pk 各自同时出现。

		// 返回 0 表示做了加密，加密结果在传入的两个引用中
		// 返回 各种非零值 表示加密失败了，此时还是应该用原始的参数去发请求

		UNIVERSAL_LIB_API int EncryptHttpRequest(LPCSTR lpszURL, LPCSTR lpszParam,
												 const BYTE *lpPostData, DWORD dwPostDataLength,
												 se::stringbuf &sbNewUrl, se::stringbuf &sbNewPostData);

		//
		// [WuJian] 这个函数用于解密加密墙返回的内容
		//	lpResponse是返回的Body
		//	dwResponseSize是返回的BodyLength
		//	dwBodySize是解密后的Body大小
		//	返回值是解密后的Body
		//
		UNIVERSAL_LIB_API BYTE* DecryptHttpRequest(BYTE *lpResponse, DWORD dwResponseSize, DWORD *dwBodySize);
	}
}