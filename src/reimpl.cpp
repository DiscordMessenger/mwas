#include <cassert>
#include <tchar.h>
#include "ri/reimpl.hpp"

#ifdef UNICODE
#define UNIVER "W"
#else
#define UNIVER "A"
#endif

namespace ri
{
	HMODULE hLibKernel32;
	HMODULE hLibUser32;
	HMODULE hLibGdi32;
	HMODULE hLibShell32;
	HMODULE hLibMsimg32;
	HMODULE hLibShlwapi;

	typedef ULONGLONG(WINAPI* PFNVERSETCONDITIONMASK)(ULONGLONG ConditionMask, DWORD TypeMask, BYTE Condition);
	typedef BOOL(WINAPI* PFNGETFILESIZEEX)(HANDLE hFile, PLARGE_INTEGER lpFileSize);
	typedef BOOL(WINAPI* PFNSETFILEPOINTEREX)(HANDLE hFile, LARGE_INTEGER liDistanceToMove, PLARGE_INTEGER lpNewFilePointer, DWORD dwMoveMethod);
	typedef BOOL(WINAPI* PFNUNREGISTERWAITEX)(HANDLE WaitHandle, HANDLE CompletionEvent);
	typedef BOOL(WINAPI* PFNREGISTERWAITFORSINGLEOBJECT)(PHANDLE phNewWaitObject, HANDLE hObject, WAITORTIMERCALLBACK Callback, PVOID Context, ULONG dwMilliseconds, ULONG dwFlags);
	typedef BOOL(WINAPI* PFNGRADIENTFILL)(HDC hdc, PTRIVERTEX trivertex, ULONG nvertex, PVOID pmesh, ULONG nmesh, ULONG ulmode);
	typedef BOOL(WINAPI* PFNALPHABLEND)(HDC hdc, int xod, int yod, int wd, int hd, HDC hdcs, int xos, int yos, int ws, int hs, BLENDFUNCTION bf);
	typedef BOOL(WINAPI* PFNTRANSPARENTBLT)(HDC hdc, int xod, int yod, int wd, int hd, HDC hdcs, int xos, int yos, int ws, int hs, UINT crt);
	typedef HRESULT(WINAPI* PFNSHGETFOLDERPATH)(HWND hwnd, int csidl, HANDLE hndl, DWORD dwf, LPTSTR szp);
	typedef BOOL(WINAPI* PFNPATHFILEEXISTS)(LPCTSTR pszPath);
	typedef BOOL(WINAPI* PFNGETMENUINFO)(HMENU hMenu, LPMENUINFO lpMenuInfo);
	typedef BOOL(WINAPI* PFNSETMENUINFO)(HMENU hMenu, LPMENUINFO lpMenuInfo);
	typedef BOOL(WINAPI* PFNGETGESTUREINFO)(HGESTUREINFO hGestureInfo, PGESTUREINFO pGestureInfo);
	typedef COLORREF(WINAPI* PFNSETDCBRUSHCOLOR)(HDC hdc, COLORREF color);
	typedef COLORREF(WINAPI* PFNSETDCPENCOLOR)(HDC hdc, COLORREF color);
	typedef HMONITOR(WINAPI* PFNMONITORFROMPOINT)(POINT pt, DWORD flags);
	typedef BOOL(WINAPI* PFNGETMONITORINFO)(HMONITOR hmon, LPMONITORINFO lpmi);
		
	PFNGETFILESIZEEX pGetFileSizeEx;
	PFNSETFILEPOINTEREX pSetFilePointerEx;
	PFNVERSETCONDITIONMASK pVerSetConditionMask;
	PFNUNREGISTERWAITEX pUnregisterWaitEx;
	PFNREGISTERWAITFORSINGLEOBJECT pRegisterWaitForSingleObject;
	PFNGRADIENTFILL pGradientFill;
	PFNALPHABLEND pAlphaBlend;
	PFNTRANSPARENTBLT pTransparentBlt;
	PFNSHGETFOLDERPATH pSHGetFolderPath; // Points to the W version on UNICODE and the A version otherwise.
	PFNPATHFILEEXISTS pPathFileExists;
	PFNGETMENUINFO pGetMenuInfo;
	PFNSETMENUINFO pSetMenuInfo;
	PFNGETGESTUREINFO pGetGestureInfo;
	PFNSETDCBRUSHCOLOR pSetDCBrushColor;
	PFNSETDCPENCOLOR pSetDCPenColor;
	PFNMONITORFROMPOINT pMonitorFromPoint;
	PFNGETMONITORINFO pGetMonitorInfo;
}
// namespace ri

void ri::InitReimplementation()
{
	// Load all the libraries.
	hLibKernel32 = LoadLibrary(TEXT("kernel32.dll"));
	hLibUser32   = LoadLibrary(TEXT("user32.dll"));
	hLibGdi32    = LoadLibrary(TEXT("gdi32.dll"));
	hLibShell32  = LoadLibrary(TEXT("shell32.dll"));
	hLibMsimg32  = LoadLibrary(TEXT("msimg32.dll"));
	hLibShlwapi  = LoadLibrary(TEXT("shlwapi.dll"));

	if (hLibKernel32)
	{
		pGetFileSizeEx = (PFNGETFILESIZEEX)GetProcAddress(hLibKernel32, "GetFileSizeEx");
		pSetFilePointerEx = (PFNSETFILEPOINTEREX)GetProcAddress(hLibKernel32, "SetFilePointerEx");
		pVerSetConditionMask = (PFNVERSETCONDITIONMASK)GetProcAddress(hLibKernel32, "VerSetConditionMask");
		pRegisterWaitForSingleObject = (PFNREGISTERWAITFORSINGLEOBJECT)GetProcAddress(hLibKernel32, "RegisterWaitForSingleObject");
		pUnregisterWaitEx = (PFNUNREGISTERWAITEX)GetProcAddress(hLibKernel32, "UnregisterWaitEx");
	}

	if (hLibUser32)
	{
		pGetMenuInfo = (PFNGETMENUINFO)GetProcAddress(hLibUser32, "GetMenuInfo");
		pSetMenuInfo = (PFNSETMENUINFO)GetProcAddress(hLibUser32, "SetMenuInfo");
		pGetGestureInfo = (PFNGETGESTUREINFO)GetProcAddress(hLibUser32, "GetGestureInfo");
		pGetMonitorInfo = (PFNGETMONITORINFO)GetProcAddress(hLibUser32, "GetMonitorInfo");
		pMonitorFromPoint = (PFNMONITORFROMPOINT)GetProcAddress(hLibUser32, "MonitorFromPoint");
	}

	if (hLibGdi32)
	{
		pSetDCBrushColor = (PFNSETDCBRUSHCOLOR)GetProcAddress(hLibGdi32, "SetDCBrushColor");
		pSetDCPenColor = (PFNSETDCPENCOLOR)GetProcAddress(hLibGdi32, "SetDCPenColor");
	}

	if (hLibMsimg32)
	{
		pGradientFill = (PFNGRADIENTFILL)GetProcAddress(hLibMsimg32, "GradientFill");
		pAlphaBlend = (PFNALPHABLEND)GetProcAddress(hLibMsimg32, "AlphaBlend");
		pTransparentBlt = (PFNTRANSPARENTBLT)GetProcAddress(hLibMsimg32, "TransparentBlt");
	}

	if (hLibShell32)
	{
		pSHGetFolderPath = (PFNSHGETFOLDERPATH)GetProcAddress(hLibShell32, "SHGetFolderPath" UNIVER);
	}

	if (hLibShlwapi)
	{
		pPathFileExists = (PFNPATHFILEEXISTS)GetProcAddress(hLibShlwapi, "PathFileExists" UNIVER);
	}
}

BOOL ri::GetFileSizeEx(HANDLE hFile, PLARGE_INTEGER lpFileSize)
{
	if (pGetFileSizeEx)
		return pGetFileSizeEx(hFile, lpFileSize);

	// Reimplementation - Use GetFileSize
	DWORD dwFileSizeHi = 0;
	DWORD dwFileSize = GetFileSize(hFile, &dwFileSizeHi);

	if (dwFileSize == INVALID_FILE_SIZE) {
		// TODO: Is this OK?
		if (GetLastError() != NO_ERROR)
			return FALSE;
	}

	lpFileSize->HighPart = dwFileSizeHi;
	lpFileSize->LowPart  = dwFileSize;
	return TRUE;
}

BOOL ri::SetFilePointerEx(HANDLE hFile, LARGE_INTEGER liDistanceToMove, PLARGE_INTEGER lpNewFilePointer, DWORD dwMoveMethod)
{
	// Thanks to https://building.enlyze.com/posts/targeting-25-years-of-windows-with-visual-studio-2019/
	if (pSetFilePointerEx)
		return pSetFilePointerEx(hFile, liDistanceToMove, lpNewFilePointer, dwMoveMethod);

	liDistanceToMove.LowPart = SetFilePointer(hFile, liDistanceToMove.LowPart, &liDistanceToMove.HighPart, dwMoveMethod);
	if (liDistanceToMove.LowPart == INVALID_SET_FILE_POINTER)
		return FALSE;

	if (lpNewFilePointer)
		lpNewFilePointer->LowPart = liDistanceToMove.LowPart;

	return TRUE;
}

#ifndef VER_NUM_BITS_PER_CONDITION_MASK
#define VER_NUM_BITS_PER_CONDITION_MASK (3)
#endif
#ifndef VER_CONDITION_MASK
#define VER_CONDITION_MASK (7)
#endif

ULONGLONG ri::VerSetConditionMask(ULONGLONG ConditionMask, DWORD TypeMask, BYTE Condition)
{
	if (pVerSetConditionMask)
		return pVerSetConditionMask(ConditionMask, TypeMask, Condition);

	// TODO: Stolen from ReactOS
	ULONGLONG ullCondMask;
 
	if (TypeMask == 0)
		return ConditionMask;
 
	Condition &= VER_CONDITION_MASK;
 
	if (Condition == 0)
		return ConditionMask;
 
	ullCondMask = Condition;
	if (TypeMask & VER_PRODUCT_TYPE)
		ConditionMask |= ullCondMask << (7 * VER_NUM_BITS_PER_CONDITION_MASK);
	else if (TypeMask & VER_SUITENAME)
		ConditionMask |= ullCondMask << (6 * VER_NUM_BITS_PER_CONDITION_MASK);
	else if (TypeMask & VER_SERVICEPACKMAJOR)
		ConditionMask |= ullCondMask << (5 * VER_NUM_BITS_PER_CONDITION_MASK);
	else if (TypeMask & VER_SERVICEPACKMINOR)
		ConditionMask |= ullCondMask << (4 * VER_NUM_BITS_PER_CONDITION_MASK);
	else if (TypeMask & VER_PLATFORMID)
		ConditionMask |= ullCondMask << (3 * VER_NUM_BITS_PER_CONDITION_MASK);
	else if (TypeMask & VER_BUILDNUMBER)
		ConditionMask |= ullCondMask << (2 * VER_NUM_BITS_PER_CONDITION_MASK);
	else if (TypeMask & VER_MAJORVERSION)
		ConditionMask |= ullCondMask << (1 * VER_NUM_BITS_PER_CONDITION_MASK);
	else if (TypeMask & VER_MINORVERSION)
		ConditionMask |= ullCondMask << (0 * VER_NUM_BITS_PER_CONDITION_MASK);
 
	return ConditionMask;
}

BOOL ri::RegisterWaitForSingleObject(PHANDLE phNewWaitObject, HANDLE hObject, WAITORTIMERCALLBACK Callback, PVOID Context, ULONG dwMilliseconds, ULONG dwFlags)
{
	if (pRegisterWaitForSingleObject)
		return pRegisterWaitForSingleObject(phNewWaitObject, hObject, Callback, Context, dwMilliseconds, dwFlags);

	// TODO - We don't use win_object_handle_service.  I've actually disabled it, so maybe we should get rid of it?
	*phNewWaitObject = INVALID_HANDLE_VALUE;
	return FALSE;
}

BOOL ri::UnregisterWaitEx(HANDLE WaitHandle, HANDLE CompletionEvent)
{
	if (pUnregisterWaitEx)
		return pUnregisterWaitEx(WaitHandle, CompletionEvent);

	// TODO - We don't use win_object_handle_service.  I've actually disabled it, so maybe we should get rid of it?

	assert(CompletionEvent == NULL || CompletionEvent == INVALID_HANDLE_VALUE);
	// Since the wait handle is bogus, do nothing.
	return FALSE;
}

bool ri::HaveMsImg()
{
	return hLibMsimg32 != NULL;
}

BOOL ri::GradientFill(HDC hdc, PTRIVERTEX trivertex, ULONG nvertex, PVOID pmesh, ULONG nmesh, ULONG ulmode)
{
	if (pGradientFill)
		return pGradientFill(hdc, trivertex, nvertex, pmesh, nmesh, ulmode);

	// XXX: Not going to implement it
	return FALSE;
}

BOOL ri::AlphaBlend(HDC hdc, int xod, int yod, int wd, int hd, HDC hdcs, int xos, int yos, int ws, int hs, BLENDFUNCTION bf)
{
	if (pAlphaBlend)
		return pAlphaBlend(hdc, xod, yod, wd, hd, hdcs, xos, yos, ws, hs, bf);

	// TODO: Just StretchBlt for now
	return StretchBlt(hdc, xod, yod, wd, hd, hdcs, xos, yos, ws, hs, SRCCOPY);
}

BOOL ri::TransparentBlt(HDC hdc, int xod, int yod, int wd, int hd, HDC hdcs, int xos, int yos, int ws, int hs, UINT crt)
{
	if (pTransparentBlt)
		return pTransparentBlt(hdc, xod, yod, wd, hd, hdcs, xos, yos, ws, hs, crt);

	// TODO: Just StretchBlt for now
	return StretchBlt(hdc, xod, yod, wd, hd, hdcs, xos, yos, ws, hs, SRCCOPY);
}

HRESULT ri::SHGetFolderPath(HWND hwnd, int csidl, HANDLE hndl, DWORD dwf, LPTSTR szp)
{
	if (pSHGetFolderPath)
		return pSHGetFolderPath(hwnd, csidl, hndl, dwf, szp);

	// TODO: For now, just "."
	_tcscpy(szp, TEXT("."));
	return S_OK;
}

BOOL ri::PathFileExists(LPCTSTR pszPath)
{
	if (pPathFileExists)
		return pPathFileExists(pszPath);
	
	DWORD lastError = GetLastError();
	DWORD attrs = GetFileAttributes(pszPath);

	if (attrs == INVALID_FILE_ATTRIBUTES) {
		SetLastError(ERROR_FILE_NOT_FOUND);
		return FALSE;
	}

	return TRUE;
}

BOOL ri::GetMenuInfo(HMENU hMenu, LPMENUINFO lpMenuInfo)
{
	if (pGetMenuInfo)
		return pGetMenuInfo(hMenu, lpMenuInfo);
	
	// UNIMPLEMENTED
	return FALSE;
}

BOOL ri::SetMenuInfo(HMENU hMenu, LPMENUINFO lpMenuInfo)
{
	if (pSetMenuInfo)
		return pSetMenuInfo(hMenu, lpMenuInfo);
	
	// UNIMPLEMENTED
	return FALSE;
}

BOOL ri::GetGestureInfo(HGESTUREINFO hGestureInfo, PGESTUREINFO pGestureInfo)
{
	if (pGetGestureInfo)
		return pGetGestureInfo(hGestureInfo, pGestureInfo);
	
	// UNIMPLEMENTED
	return FALSE;
}

COLORREF ri::SetDCBrushColor(HDC hdc, COLORREF color)
{
	if (pSetDCBrushColor)
		return pSetDCBrushColor(hdc, color);

	// TODO
	return 0;
}

COLORREF ri::SetDCPenColor(HDC hdc, COLORREF color)
{
	if (pSetDCPenColor)
		return pSetDCPenColor(hdc, color);

	// TODO
	return 0;
}

#define FAKE_PRIMARY_MONITOR ((HMONITOR) 0x1235679A)

HMONITOR ri::MonitorFromPoint(POINT pt, DWORD flags)
{
	if (pMonitorFromPoint)
		return pMonitorFromPoint(pt, flags);

	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);

	if ((flags & MONITOR_DEFAULTTONULL) && (pt.x < 0 || pt.y < 0 || pt.x >= width || pt.y >= height))
		return NULL;

	return FAKE_PRIMARY_MONITOR;
}

BOOL ri::GetMonitorInfo(HMONITOR hmon, LPMONITORINFO lpmi)
{
	if (pGetMonitorInfo)
		return pGetMonitorInfo(hmon, lpmi);

	if (hmon != FAKE_PRIMARY_MONITOR)
		return FALSE;

	DWORD size = lpmi->cbSize;
	ZeroMemory(lpmi, size);
	lpmi->cbSize = size;

	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);

	lpmi->dwFlags = MONITORINFOF_PRIMARY;

	// Get monitor rectangle
	lpmi->rcMonitor.left = lpmi->rcMonitor.top = 0;
	lpmi->rcMonitor.right = width;
	lpmi->rcMonitor.bottom = height;
	
	// Get work area
	// Note, preserve last error
	DWORD lastErr = GetLastError();
	if (!SystemParametersInfo(SPI_GETWORKAREA, 0, &lpmi->rcWork, 0)) {
		// Failed to call the function, just return rcMonitor
		lpmi->rcWork = lpmi->rcMonitor;
	}
	SetLastError(lastErr);

	if (lpmi->cbSize == sizeof(MONITORINFOEX)) {
		LPMONITORINFOEX lpmiex = (LPMONITORINFOEX) lpmi;
		_tcscpy(lpmiex->szDevice, TEXT("MWAS Reimpl Monitor"));
	}

	return TRUE;
}
