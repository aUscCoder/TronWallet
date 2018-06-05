// Author: TuotuoXP
#pragma once
#include <Windows.h>


enum lockenum { lkNormal = 0, lkPrealloc, lkDeleted };
#define _STREAM_LOCKS   16      /* Table of stream locks            */

#define _LAST_STREAM_LOCK  (_STREAM_LOCKS+_IOB_ENTRIES-1) /* Last stream lock */

#define _TOTAL_LOCKS        (_LAST_STREAM_LOCK+1)

#define NUM_STD_FILE_LOCKS     3

#ifdef _DEBUG
#define NUM_NON_PREALLOC_LOCKS 4
#else  /* _DEBUG */
#define NUM_NON_PREALLOC_LOCKS 5
#endif  /* _DEBUG */

#define NUM_PREALLOC_LOCKS		(_STREAM_LOCKS + NUM_STD_FILE_LOCKS - NUM_NON_PREALLOC_LOCKS)

extern "C" CRITICAL_SECTION lclcritsects_crossmodule[NUM_PREALLOC_LOCKS] = {};


extern "C" struct {
	PCRITICAL_SECTION lock;
	lockenum kind;
	int need_cross_module;
} _locktable_crossmodule[_TOTAL_LOCKS] = {
	{ NULL, lkPrealloc }, /* 0  == _SIGNAL_LOCK      */
	{ NULL, lkPrealloc }, /* 1  == _IOB_SCAN_LOCK    */
	{ NULL, lkNormal   }, /* 2  == _TMPNAM_LOCK      - not preallocated */
	{ NULL, lkPrealloc }, /* 3  == _CONIO_LOCK       */
	{ NULL, lkPrealloc, 1 }, /* 4  == _HEAP_LOCK        */
	{ NULL, lkNormal   }, /* 5  == _UNDNAME_LOCK     - not preallocated */
	{ NULL, lkPrealloc }, /* 6  == _TIME_LOCK        */
	{ NULL, lkPrealloc }, /* 7  == _ENV_LOCK         */
	{ NULL, lkPrealloc }, /* 8  == _EXIT_LOCK1       */
	{ NULL, lkNormal   }, /* 9  == _POPEN_LOCK       - not preallocated */
	{ NULL, lkPrealloc }, /* 10 == _LOCKTAB_LOCK     */
	{ NULL, lkNormal   }, /* 11 == _OSFHND_LOCK      - not preallocated */
	{ NULL, lkPrealloc }, /* 12 == _SETLOCALE_LOCK   */
	{ NULL, lkPrealloc }, /* 13 == _MB_CP_LOCK       */
	{ NULL, lkPrealloc }, /* 14 == _TYPEINFO_LOCK    */
#ifdef _DEBUG
	{ NULL, lkPrealloc }, /* 15 == _DEBUG_LOCK       */
#else  /* _DEBUG */
	{ NULL, lkNormal },   /* 15 == _DEBUG_LOCK       */
#endif  /* _DEBUG */

	{ NULL, lkPrealloc }, /* 16 == _STREAM_LOCKS+0 - stdin  */
	{ NULL, lkPrealloc }, /* 17 == _STREAM_LOCKS+1 - stdout */
	{ NULL, lkPrealloc }, /* 18 == _STREAM_LOCKS+2 - stderr */
	/*      { NULL, lkNormal   }, /* ... */
};


#ifdef _DEBUG

extern "C" _CrtMemBlockHeader * _pFirstBlock = NULL;
extern "C" _CrtMemBlockHeader * _pLastBlock = NULL;

#pragma comment(linker, "/EXPORT:__pFirstBlock")
#pragma comment(linker, "/EXPORT:__pLastBlock")

#pragma comment(linker, "/EXPORT:_lclcritsects_crossmodule")
#pragma comment(linker, "/EXPORT:__locktable_crossmodule")

#endif
