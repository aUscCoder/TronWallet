// Author: wujian
#pragma once
#include "../../Universal.h"


// TEA加密算法,CBC模式
// pKey为16byte
// 输入:pInBuf为需加密的明文部分(Body),nInBufLen为pInBuf长度;
// 输出:pOutBuf为密文格式,pOutBufLen为pOutBuf的长度是8byte的倍数,至少应预留nInBufLen+17;
// 密文格式:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)
UNIVERSAL_LIB_API void SogouSymmetryEncrypt(const BYTE* pInBuf, int nInBufLen, const BYTE* pKey, BYTE* pOutBuf, int *pOutBufLen);


// TEA解密算法,CBC模式
// pKey为16byte
// 输入:pInBuf为密文格式,nInBufLen为pInBuf的长度是8byte的倍数; *pOutBufLen为接收缓冲区的长度,特别注意*pOutBufLen应预置接收缓冲区的长度!
// 输出:pOutBuf为明文(Body),pOutBufLen为pOutBuf的长度,至少应预留nInBufLen-10;
// 返回值:如果格式正确返回TRUE;
UNIVERSAL_LIB_API bool SogouSymmetryDecrypt(const BYTE* pInBuf, int nInBufLen, const BYTE* pKey, BYTE* pOutBuf, int *pOutBufLen);


//////////////////////////////////////////////////////////////////////////

// 下面这个两个函数的返回值需要free，可以保证绝对不会返回NULL

UNIVERSAL_LIB_API void* SogouSymmetryEncrypt(const void *pInBuf, int iInBufLen, const void *pKey, int &iOutBufLen);
UNIVERSAL_LIB_API void* SogouSymmetryDecrypt(const void *pInBuf, int iInBufLen, const void *pKey, int &iOutBufLen);
