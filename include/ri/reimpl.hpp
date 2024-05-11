#pragma once

#include <windows.h>

// This part of the DiscordMessenger project is designed to reimplement APIs missing
// in earlier versions of Windows, such as Windows NT 4.  It's basically like a polyfill.

// NOTE: This header is in "exinclude" (and its associated source in external/) because
// I want to access it easily from asio sources.

// Here are the APIs we will reimplement*:

/*
IMPORTS WHICH NT 4 DOESN'T HAVE:

KERNEL32:
	[X] GetFileSizeEx                 -- Used by asio::detail::win_iocp_file_service
	[X] SetFilePointerEx              -- Used by asio::detail::win_iocp_file_service
	[X] VerSetConditionMask           -- Used by asio::detail::win_iocp_io_context
	[X] RegisterWaitForSingleObject   -- Used by asio::detail::win_object_handle_service
	[X] UnregisterWaitEx              -- Used by asio::detail::win_object_handle_service

MSIMG32: // All missing
	[X] AlphaBlend      -- FillGradientColors
	[X] GradientFill    -- MessageList, FillGradientColors
	[X] TransparentBlt  -- DrawBitmap

SHELL32:
	[X] SHGetFolderPathW -- Used by WinUtils / SetupCachePathIfNeeded

SHLWAPI: // All missing
	[X] PathFileExistsW  -- Used by WinUtils / FileExists

USER32:
	[x] GetMenuInfo -- Used by MessageList
	[x] SetMenuInfo -- Used by MessageList

GDI32:
	SetDCBrushColor -- RichEmbedItem, MessageList, RoleList
	SetDCPenColor   -- GuildHeader, GuildLister, MessageList, RoleList

*/

// * - Sometimes we will emulate them, sometimes we will simply make them do nothing

#ifndef SHGetFolderPath
# ifdef UNICODE
#  define SHGetFolderPath SHGetFolderPathW
# else
#  define SHGetFolderPath SHGetFolderPathA
# endif
#endif

#ifndef PathFileExists
# ifdef UNICODE
#  define PathFileExists PathFileExistsW
# else
#  define PathFileExists PathFileExistsA
# endif
#endif

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

	// Msimg32
	bool HaveMsImg();
	BOOL GradientFill(HDC hdc, PTRIVERTEX trivertex, ULONG nvertex, PVOID pmesh, ULONG nmesh, ULONG ulmode);
	BOOL AlphaBlend(HDC hdc, int xod, int yod, int wd, int hd, HDC hdcs, int xos, int yos, int ws, int hs, BLENDFUNCTION bf);
	BOOL TransparentBlt(HDC hdc, int xod, int yod, int wd, int hd, HDC hdcs, int xos, int yos, int ws, int hs, UINT crt);

	// Shell32
	// NOTE: This is allowed to manifest as a macro.  We only define the version we actually use.
	HRESULT SHGetFolderPath(HWND hwnd, int csidl, HANDLE hndl, DWORD dwf, LPTSTR szp);

	// Shlwapi
	BOOL PathFileExists(LPCTSTR pszPath);

	// User32
	BOOL GetMenuInfo(HMENU hMenu, LPMENUINFO lpMenuInfo);
	BOOL SetMenuInfo(HMENU hMenu, LPMENUINFO lpMenuInfo);

	// Gdi32
	COLORREF SetDCBrushColor(HDC hdc, COLORREF color);
	COLORREF SetDCPenColor(HDC hdc, COLORREF color);
}
// namespace ri
