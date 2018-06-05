// Author: ChenJianjun
#pragma once


typedef void (*ContextCallback)(LPARAM lParam);
struct IContextCallback
{
	virtual ~IContextCallback() {};
	virtual void OnContextDestroy() = 0;
};
