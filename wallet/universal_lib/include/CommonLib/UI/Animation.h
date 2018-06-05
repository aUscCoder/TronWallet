// Author: ChenJianjun
#pragma once
#include "../../Universal.h"


class CPngAni;
namespace Universal
{
	namespace UI
	{
		namespace Ani
		{
			UNIVERSAL_LIB_API const CPngAni* LoadBuffer(BYTE *pData, DWORD dwLen);
			UNIVERSAL_LIB_API const CPngAni* LoadRes(HINSTANCE hInstance, UINT nResourceID, LPCWSTR pResourceType);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

class CPngAni
{
public:

	~CPngAni();

	CPngAni* Copy();

	CSize GetSize();
	int GetFrameCount();
	int GetDuration(int iFrameIndex);

	void Draw(CDCHandle dc, const RECT& rc, int iFrameIndex, BYTE cAlpha = 255);

private:

	friend const CPngAni* Universal::UI::Ani::LoadBuffer(BYTE*, DWORD);
	struct AnimationInfo;

	CPngAni() {}
	CPngAni(AnimationInfo* pAni): m_pAni(pAni) {}
	CPngAni(const CPngAni&) {}
	CPngAni& operator=(const CPngAni& that) { return *this; }

	struct AnimationInfo
	{
		HBITMAP hBitmap;
		int iBMWidth;
		int iBMHeight;
		int iFrameCount;
		int iDuration[1];
	};

	AnimationInfo* m_pAni;
};
