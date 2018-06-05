// Author: ChenJianjun
#pragma once

// BEGIN_NOLINT (4.5.66 这是数据)
// BEGIN_NOLINT (4.5.67 这是数据)
//
// tree button     [-|+]
//
extern BYTE g_cTreeButtonRes[];
extern int g_cTreeButtonResSize;
extern BYTE g_cTreeButtonTrippleRes[];
extern int g_cTreeButtonTrippleResSize;
#define TREE_BUTTON_IMAGE_FRAME_COUNT	2
#define TREE_BUTTON_IMAGE_EXPAND_FRAME 0
#define TREE_BUTTON_IMAGE_COLLAPSE_FRAME 1

//
// checkbox               unchecked                             checked                             mix
//				normal|hover|focus|pressed|disabled | normal|hover|focus|pressed|disabled | normal|hover|focus|pressed|disabled
//
extern BYTE g_cCheckBoxRes[];
extern int g_cCheckBoxResSize;

#define CHECKBOX_FRAME_COUNT 15
#define CHECKBOX_UNCHECKED_NORMAL 0
#define CHECKBOX_UNCHECKED_HOVER 1
#define CHECKBOX_UNCHECKED_FOCUS 2
#define CHECKBOX_UNCHECKED_PRESSED 3
#define CHECKBOX_UNCHECKED_DISABLED 4
#define CHECKBOX_CHECKED_NORMAL 5
#define CHECKBOX_CHECKED_HOVER 6
#define CHECKBOX_CHECKED_FOCUS 7
#define CHECKBOX_CHECKED_PRESSED 8
#define CHECKBOX_CHECKED_DISABLED 9
#define CHECKBOX_MIX_NORMAL 10
#define CHECKBOX_MIX_HOVER 11
#define CHECKBOX_MIX_FOCUS 12
#define CHECKBOX_MIX_PRESSED 13
#define CHECKBOX_MIX_DISABLED 14


//
// ScrollLeft		85 * 17
//			normal|hover|focus|pressed|disabled
//
extern BYTE g_cHScrollLeftRes[];
extern int g_cHScrollLeftResSize;
#define HSCROLLLEFT_FRAME_COUNT 5
#define HSCROLLLEFT_NORMAL 0
#define HSCROLLLEFT_HOVER 1
#define HSCROLLLEFT_FOCUS 2
#define HSCROLLLEFT_PRESSED 3
#define HSCROLLLEFT_DISABLED 4


//
// ScrollRight		85 * 17
//			normal|hover|focus|pressed|disabled
//
extern BYTE g_cHScrollRightRes[];
extern int g_cHScrollRightResSize;
#define HSCROLLRIGHT_FRAME_COUNT 5
#define HSCROLLRIGHT_NORMAL 0
#define HSCROLLRIGHT_HOVER 1
#define HSCROLLRIGHT_FOCUS 2
#define HSCROLLRIGHT_PRESSED 3
#define HSCROLLRIGHT_DISABLED 4


//
// HScrollThumb		35 * 17
//			normal|hover|focus|pressed|disabled
//
extern BYTE g_cHScrollThumbRes[];
extern int g_cHScrollThumbResSize;

#define HSCROLLTHUMB_FRAME_COUNT 5
#define HSCROLLTHUMB_NORMAL 0
#define HSCROLLTHUMB_HOVER 1
#define HSCROLLTHUMB_FOCUS 2
#define HSCROLLTHUMB_PRESSED 3
#define HSCROLLTHUMB_DISABLED 4


//
// HScrollThumbGripper		85 * 17
//			normal|hover|focus|pressed|disabled
//
extern BYTE g_cHScrollThumbGripperRes[];
extern int g_cHScrollThumbGripperResSize;

#define HSCROLLTHUMBGRIPPER_FRAME_COUNT 5
#define HSCROLLTHUMBGRIPPER_NORMAL 0
#define HSCROLLTHUMBGRIPPER_HOVER 1
#define HSCROLLTHUMBGRIPPER_FOCUS 2
#define HSCROLLTHUMBGRIPPER_PRESSED 3
#define HSCROLLTHUMBGRIPPER_DISABLED 4


//
// HScrollTrack		2 * 17
//			normal | pressed
//
extern BYTE g_cHScrollTrackRes[];
extern int g_cHScrollTrackResSize;
#define HSCROLLTRACK_FRAME_COUNT 2
#define HSCROLLTRACK_NORMAL 0
#define HSCROLLTRACK_HOVER 1
#define HSCROLLTRACK_FOCUS 1
#define HSCROLLTRACK_PRESSED 1
#define HSCROLLTRACK_DISABLED 0


//
// VScrollUp		85 * 17
//			normal|hover|focus|pressed|disabled
//
extern BYTE g_cVScrollUpRes[];
extern int g_cVScrollUpResSize;
#define VSCROLLUP_FRAME_COUNT 5
#define VSCROLLUP_NORMAL 0
#define VSCROLLUP_HOVER 1
#define VSCROLLUP_FOCUS 2
#define VSCROLLUP_PRESSED 3
#define VSCROLLUP_DISABLED 4


//
// VScrollDown		85 * 17
//			normal|hover|focus|pressed|disabled
//
extern BYTE g_cVScrollDownRes[];
extern int g_cVScrollDownResSize;
#define VSCROLLDOWN_FRAME_COUNT 5
#define VSCROLLDOWN_NORMAL 0
#define VSCROLLDOWN_HOVER 1
#define VSCROLLDOWN_FOCUS 2
#define VSCROLLDOWN_PRESSED 3
#define VSCROLLDOWN_DISABLED 4


//
// VScrollThumb		85 * 5
//			normal|hover|focus|pressed|disabled
//
extern BYTE g_cVScrollThumbRes[];
extern int g_cVScrollThumbResSize;
#define VSCROLLTHUMB_FRAME_COUNT 5
#define VSCROLLTHUMB_NORMAL 0
#define VSCROLLTHUMB_HOVER 1
#define VSCROLLTHUMB_FOCUS 2
#define VSCROLLTHUMB_PRESSED 3
#define VSCROLLTHUMB_DISABLED 4


//
// VScrollTrack		34 * 1
//			normal | pressed
//
extern BYTE g_cVScrollTrackRes[];
extern int g_cVScrollTrackResSize;
#define VSCROLLTRACK_FRAME_COUNT 2
#define VSCROLLTRACK_NORMAL 0
#define VSCROLLTRACK_HOVER 1
#define VSCROLLTRACK_FOCUS 1
#define VSCROLLTRACK_PRESSED 1
#define VSCROLLTRACK_DISABLED 0


//
// VScrollThumbGripper		85 * 17
//			normal|hover|focus|pressed|disabled
//
extern BYTE g_cVScrollThumbGripperRes[];
extern int g_cVScrollThumbGripperResSize;

#define VSCROLLTHUMBGRIPPER_FRAME_COUNT 5
#define VSCROLLTHUMBGRIPPER_NORMAL 0
#define VSCROLLTHUMBGRIPPER_HOVER 1
#define VSCROLLTHUMBGRIPPER_FOCUS 2
#define VSCROLLTHUMBGRIPPER_PRESSED 3
#define VSCROLLTHUMBGRIPPER_DISABLED 4


//
// BackgroundRes		35 * 20
//			normal|hover|pressed|disabled|focus-without-windowfocus
//
extern BYTE g_cBackgroundRes[];
extern int g_cBackgroundResSize;

#define BACKGROUND_FRAME_COUNT 5
#define BACKGROUND_NORMAL 0
#define BACKGROUND_HOVER 1
#define BACKGROUND_FOCUS 2
#define BACKGROUND_PRESSED 2
#define BACKGROUND_DISABLED 3
#define BACKGROUND_NOACTIVE_FOCUS 4


//
// StaticBackgroundRes		9 * 20
//			normal | gray | gray-sep
//
extern BYTE g_cStaticBackgroundRes[];
extern int g_cStaticBackgroundResSize;
#define STATICBACKGROUND_FRAME_COUNT 3
#define STATICBACKGROUND_NORMAL 0
#define STATICBACKGROUND_GRAY 1
#define STATICBACKGROUND_GRAY_SEP 2

// END_NOLINT
// END_NOLINT
