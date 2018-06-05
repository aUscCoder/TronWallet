#pragma once

#define MESSAGE_NUMBER_SCOPE_BEGIN(name) enum name	\
	{															\
	WM_##name##_MESSAGE_BEGIN = WM_##name##_MESSAGE_SCOPE_BEGIN,

#define CHECK_MESSAGE_SCOPE_RANGE(name) extern int ____message__range__check__##name##__[WM_##name##_MESSAGE_END - WM_##name##_MESSAGE_LAST];

#define MESSAGE_NUMBER_SCOPE_END(name) \
	WM_##name##_MESSAGE_LAST,\
	WM_##name##_MESSAGE_END = WM_##name##_MESSAGE_SCOPE_END,\
	};CHECK_MESSAGE_SCOPE_RANGE(name)

#define DEFINE_MESSAGE_SCOPE(name,range) WM_##name##_MESSAGE_SCOPE_BEGIN,\
	WM_##name##_MESSAGE_SCOPE_END = WM_##name##_MESSAGE_SCOPE_BEGIN + range - 1,
