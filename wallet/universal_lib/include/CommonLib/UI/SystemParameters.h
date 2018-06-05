// Author: ChenJianjun
#pragma once


namespace Universal
{
	namespace UI
	{
		namespace SystemParameters
		{
			UINT GetWheelScrollLines();
			DWORD GetMenuShowDelay();
			BOOL IsComboBoxAnimationEnabled();
			BOOL IsMenuAnimationEnabled();
			BOOL IsMenuFadeEnabled();
			BOOL IsWindowAnimationEnabled();
		}
	}
}

//////////////////////////////////////////////////////////////////////////
inline UINT Universal::UI::SystemParameters::GetWheelScrollLines()
{
	UINT uWheelLines = 0;
	::SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &uWheelLines, 0);
	return uWheelLines;
}

inline DWORD Universal::UI::SystemParameters::GetMenuShowDelay()
{
	DWORD dwDelay = 0;
	::SystemParametersInfo(SPI_GETMENUSHOWDELAY, 0, &dwDelay, 0);
	return dwDelay;
}

inline BOOL Universal::UI::SystemParameters::IsMenuFadeEnabled()
{
	BOOL bEnableFade = FALSE;
	::SystemParametersInfo(SPI_GETMENUFADE, NULL, &bEnableFade, 0);
	return Universal::UI::SystemParameters::IsMenuAnimationEnabled() && bEnableFade;
}

inline BOOL Universal::UI::SystemParameters::IsWindowAnimationEnabled()
{
	ANIMATIONINFO ani;
	::SystemParametersInfo(SPI_GETANIMATION, 0, &ani, 0);
	return ani.iMinAnimate;
}

inline BOOL Universal::UI::SystemParameters::IsComboBoxAnimationEnabled()
{
	BOOL bAniEnabled = FALSE;
	::SystemParametersInfo(SPI_GETMENUFADE, NULL, &bAniEnabled, 0);
	return bAniEnabled;
}

inline BOOL Universal::UI::SystemParameters::IsMenuAnimationEnabled()
{
	BOOL bAniEnabled = FALSE;
	::SystemParametersInfo(SPI_GETMENUANIMATION, NULL, &bAniEnabled, 0);
	return bAniEnabled;
}
