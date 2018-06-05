// Author: ChenJianjun
#pragma once


class CFPSCounter
{
public:

	void OnPaint();

	float GetFPS();
	int GetCount() { return m_paint.size(); }
	std::wstring GetFPSStr();

private:

	std::deque<__int64> m_paint;
};
