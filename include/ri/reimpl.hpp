#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// This part of the DiscordMessenger project is designed to reimplement APIs missing
// in earlier versions of Windows, such as Windows NT 4.  It's basically like a polyfill.

// NOTE: This header is in "exinclude" (and its associated source in external/) because
// I want to access it easily from asio sources.

// Sometimes we will emulate them, sometimes we will simply make them do nothing

#ifndef SHGetFolderPath
# ifdef UNICODE
#  define SHGetFolderPath SHGetFolderPathW
# else
#  define SHGetFolderPath SHGetFolderPathA
# endif
#endif

#ifndef SHGetFileInfo
# ifdef UNICODE
#  define SHGetFileInfo SHGetFileInfoW
# else
#  define SHGetFileInfo SHGetFileInfoA
# endif
#endif

#ifndef Shell_NotifyIcon
# ifdef UNICODE
#  define Shell_NotifyIcon Shell_NotifyIconW
# else
#  define Shell_NotifyIcon Shell_NotifyIconA
# endif
#endif

#ifdef UNICODE
typedef struct _SHFILEINFOW SHFILEINFO;
typedef struct _NOTIFYICONDATAW NOTIFYICONDATA;
typedef struct _OSVERSIONINFOW OSVERSIONINFO;
#else
typedef struct _SHFILEINFOA SHFILEINFO;
typedef struct _NOTIFYICONDATAA NOTIFYICONDATA;
typedef struct _OSVERSIONINFOA OSVERSIONINFO;
#endif

#ifndef PathFileExists
# ifdef UNICODE
#  define PathFileExists PathFileExistsW
# else
#  define PathFileExists PathFileExistsA
# endif
#endif

#ifndef GetMonitorInfo
# ifdef UNICODE
#  define GetMonitorInfo GetMonitorInfoW
# else
#  define GetMonitorInfo GetMonitorInfoA
# endif
#endif

#ifndef VerifyVersionInfo
# ifdef UNICODE
#  define VerifyVersionInfo VerifyVersionInfoW
# else
#  define VerifyVersionInfo VerifyVersionInfoA
# endif
#endif

#ifndef LoadImage
# ifdef UNICODE
#  define LoadImage LoadImageW
# else
#  define LoadImage LoadImageA
# endif
#endif

#ifndef GetVersionEx
# ifdef UNICODE
#  define GetVersionEx GetVersionExW
# else
#  define GetVersionEx GetVersionExA
# endif
#endif

// To permit the same executable from running on all versions of Windows
// without sacrificing functionality, add gesture definitions here:
#if (WINVER < 0x0601)

#define WM_GESTURE       (281)
#define WM_GESTURENOTIFY (282)

#define GID_BEGIN        1
#define GID_END          2
#define GID_ZOOM         3
#define GID_PAN          4
#define GID_ROTATE       5
#define GID_TWOFINGERTAP 6
#define GID_PRESSANDTAP  7
#define GID_ROLLOVER     GID_PRESSANDTAP

typedef struct __GESTUREINFO
{
	UINT cbSize;
	DWORD dwFlags;
	DWORD dwID;
	HWND hwndTarget;
	POINTS ptsLocation;
	DWORD dwInstanceID;
	DWORD dwSequenceID;
	ULONGLONG ullArguments;
	UINT cbExtraArgs;
}
GESTUREINFO, *PGESTUREINFO;

DECLARE_HANDLE(HGESTUREINFO);

#endif

typedef struct _IMAGELIST* HIMAGELIST;

namespace ri
{
	// Initialize the reimplementations.  If available, use the Windows provided
	// functions instead.
	void InitReimplementation();

	// Kernel32
	ULONGLONG VerSetConditionMask(ULONGLONG ConditionMask, DWORD TypeMask, BYTE Condition);
	BOOL GetFileSizeEx(HANDLE hFile, PLARGE_INTEGER lpFileSize);
	BOOL SetFilePointerEx(HANDLE hFile, LARGE_INTEGER liDistanceToMove, PLARGE_INTEGER lpNewFilePointer, DWORD dwMoveMethod);
	BOOL RegisterWaitForSingleObject(PHANDLE phNewWaitObject, HANDLE hObject, WAITORTIMERCALLBACK Callback, PVOID Context, ULONG dwMilliseconds, ULONG dwFlags);
	BOOL UnregisterWaitEx(HANDLE WaitHandle, HANDLE CompletionEvent);
	BOOL VerifyVersionInfo(LPOSVERSIONINFOEX pVersionInfo, DWORD typeMask, DWORDLONG conditionMask);
	BOOL InitializeCriticalSectionAndSpinCount(LPCRITICAL_SECTION lpCriticalSection, DWORD dwSpinCount);
	BOOL TryEnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
	LONG InterlockedExchangeAdd(LONG* Addend, LONG Value);
	DWORD QueueUserAPC(PAPCFUNC pfnAPC, HANDLE hThread, ULONG_PTR dwData);
	BOOL GetVersionEx(LPOSVERSIONINFO lpVersionInformation);

	// Msimg32
	bool HaveMsImg();
	BOOL GradientFill(HDC hdc, PTRIVERTEX trivertex, ULONG nvertex, PVOID pmesh, ULONG nmesh, ULONG ulmode);
	BOOL AlphaBlend(HDC hdc, int xod, int yod, int wd, int hd, HDC hdcs, int xos, int yos, int ws, int hs, BLENDFUNCTION bf);
	BOOL TransparentBlt(HDC hdc, int xod, int yod, int wd, int hd, HDC hdcs, int xos, int yos, int ws, int hs, UINT crt);

	// Shell32
	// NOTE: This is allowed to manifest as a macro.  We only define the version we actually use.
	HRESULT SHGetFolderPath(HWND hwnd, int csidl, HANDLE hndl, DWORD dwf, LPTSTR szp);
	DWORD_PTR SHGetFileInfo(LPCTSTR pszPath, DWORD dwFileAttributes, SHFILEINFO* psfi, UINT cbFileInfo, UINT uFlags);
	BOOL Shell_NotifyIcon(DWORD dwMessage, NOTIFYICONDATA* nid);

	// Shlwapi
	BOOL PathFileExists(LPCTSTR pszPath);

	// User32
	BOOL AnimateWindow(HWND hWnd, DWORD time, DWORD flags);
	BOOL GetMenuInfo(HMENU hMenu, LPMENUINFO lpMenuInfo);
	BOOL SetMenuInfo(HMENU hMenu, LPMENUINFO lpMenuInfo);
	BOOL GetGestureInfo(HGESTUREINFO hGestureInfo, PGESTUREINFO pGestureInfo);
	BOOL GetMonitorInfo(HMONITOR hmon, LPMONITORINFO lpmi);
	HMONITOR MonitorFromPoint(POINT pt, DWORD flags);
	BOOL TrackMouseEvent(LPTRACKMOUSEEVENT lptme);
	BOOL DrawEdge(HDC, LPRECT, UINT, UINT);
	BOOL DrawIconEx(HDC, int, int, HICON, int, int, UINT, HBRUSH, UINT);
	BOOL GetScrollInfo(HWND, int, LPSCROLLINFO);
	BOOL SetScrollInfo(HWND, int, LPSCROLLINFO, BOOL);
	HBRUSH GetSysColorBrush(int);
	HBITMAP LoadImage(HINSTANCE, LPCTSTR, UINT, int, int, UINT);

	// Gdi32
	COLORREF SetDCBrushColor(HDC hdc, COLORREF color);
	COLORREF SetDCPenColor(HDC hdc, COLORREF color);
	HBITMAP CreateDIBSection(HDC hdc, const BITMAPINFO* pbmi, UINT usage, VOID** ppvBits, HANDLE hSection, DWORD offset);
	
	// Not actually part of win32, but we have to do this instead
	// of relying on GetStockBrush:
	HBRUSH GetDCBrush();
	HPEN GetDCPen();

	// Some flags that would/might actually cause things to
	// break if they were set on earlier versions of Windows
	int GetHalfToneStretchMode();
	int GetWordEllipsisFlag();
	
	// Ole32
	HRESULT CoInitialize(LPVOID lpv);

	// ComCtl32
	HIMAGELIST ImageList_Create(int cx, int cy, UINT flags, int cInitial, int cGrow);
	BOOL ImageList_Destroy(HIMAGELIST himl);
	BOOL ImageList_Add(HIMAGELIST himl, HBITMAP hbmImage, HBITMAP hbmMask);
	int ImageList_GetImageCount(HIMAGELIST himl);
	int ImageList_ReplaceIcon(HIMAGELIST himl, int i, HICON hicon);
}
// namespace ri
