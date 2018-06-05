// Author: wangdongwei
#pragma once
#include "../../Universal.h"

namespace UI
{
	namespace UIDebug
	{
		namespace Internal
		{
			bool IsTracePaint();
			bool IsTraceFocus();
			bool IsTraceFPS();
			int GetTraceHover();
			int GetAnimationSlowdownValue();
			int GetDpiValue();
		}

		class IUIDebugger	// NOLINT (3.1.4 不需要构造)
		{
		public:
			virtual ~IUIDebugger() {}
			virtual bool IsTracePaint() = 0;
			virtual bool IsTraceFocus() = 0;
			virtual bool IsTraceFPS() = 0;
			virtual int GetTraceHover() = 0;
			virtual int GetAnimationSlowdownValue() = 0;
			virtual int GetDpiValue() = 0;
		};

		UNIVERSAL_LIB_API void SetInitializer(IUIDebugger* pUIDebugger);
	}
}
