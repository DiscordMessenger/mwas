#include <cassert>
#include <tchar.h>
#include "ri/reimpl.hpp"

#ifdef _MSC_VER
#include <intrin.h>
#endif

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
	typedef BOOL(WINAPI* PFNVERIFYVERSIONINFO)(LPOSVERSIONINFOEX pVersionInfo, DWORD typeMask, DWORDLONG conditionMask);
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
	typedef BOOL(WINAPI* PFNANIMATEWINDOW)(HWND hwnd, DWORD time, DWORD flags);
	typedef BOOL(WINAPI* PFNINITIALIZECRITICALSECTIONANDSPINCOUNT)(LPCRITICAL_SECTION lpCriticalSection, DWORD dwSpinCount);
	typedef BOOL(WINAPI* PFNTRYENTERCRITICALSECTION)(LPCRITICAL_SECTION lpCriticalSection);
		
	PFNGETFILESIZEEX pGetFileSizeEx;
	PFNSETFILEPOINTEREX pSetFilePointerEx;
	PFNVERSETCONDITIONMASK pVerSetConditionMask;
	PFNUNREGISTERWAITEX pUnregisterWaitEx;
	PFNREGISTERWAITFORSINGLEOBJECT pRegisterWaitForSingleObject;
	PFNVERIFYVERSIONINFO pVerifyVersionInfo;
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
	PFNANIMATEWINDOW pAnimateWindow;
	PFNINITIALIZECRITICALSECTIONANDSPINCOUNT pInitializeCriticalSectionAndSpinCount;
	PFNTRYENTERCRITICALSECTION pTryEnterCriticalSection;
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
		pVerifyVersionInfo = (PFNVERIFYVERSIONINFO)GetProcAddress(hLibKernel32, "VerifyVersionInfo" UNIVER);
		pInitializeCriticalSectionAndSpinCount = (PFNINITIALIZECRITICALSECTIONANDSPINCOUNT)GetProcAddress(hLibKernel32, "InitializeCriticalSectionAndSpinCount");
		pTryEnterCriticalSection = (PFNTRYENTERCRITICALSECTION)GetProcAddress(hLibKernel32, "TryEnterCriticalSection");
	}

	if (hLibUser32)
	{
		pGetMenuInfo = (PFNGETMENUINFO)GetProcAddress(hLibUser32, "GetMenuInfo");
		pSetMenuInfo = (PFNSETMENUINFO)GetProcAddress(hLibUser32, "SetMenuInfo");
		pGetGestureInfo = (PFNGETGESTUREINFO)GetProcAddress(hLibUser32, "GetGestureInfo");
		pGetMonitorInfo = (PFNGETMONITORINFO)GetProcAddress(hLibUser32, "GetMonitorInfo" UNIVER);
		pMonitorFromPoint = (PFNMONITORFROMPOINT)GetProcAddress(hLibUser32, "MonitorFromPoint");
		pAnimateWindow = (PFNANIMATEWINDOW)GetProcAddress(hLibUser32, "AnimateWindow");
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

static bool CompareVersionField(DWORD data1, DWORD data2, int type)
{
	switch (type)
	{
		case VER_EQUAL: return data1 == data2;
		case VER_GREATER: return data1 > data2;
		case VER_GREATER_EQUAL: return data1 >= data2;
		case VER_LESS: return data1 < data2;
		case VER_LESS_EQUAL: return data1 <= data2;
		case VER_AND: return (data1 & data2) == data1;
		case VER_OR: return (data1 & data2) != 0;
		default:
			assert(!"Unknown comparison type");
			return true;
	}
}

BOOL ri::VerifyVersionInfo(LPOSVERSIONINFOEX pVersionInfo, DWORD typeMask, DWORDLONG conditionMask)
{
	if (pVerifyVersionInfo)
		return pVerifyVersionInfo(pVersionInfo, typeMask, conditionMask);

	OSVERSIONINFO ver{};
	ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if (!GetVersionEx(&ver))
		return FALSE;

	if (typeMask & VER_MINORVERSION)
		if (!CompareVersionField(ver.dwMinorVersion, pVersionInfo->dwMinorVersion, (conditionMask >> (0 * VER_NUM_BITS_PER_CONDITION_MASK)) & VER_CONDITION_MASK))
			return FALSE;
	if (typeMask & VER_MAJORVERSION)
		if (!CompareVersionField(ver.dwMajorVersion, pVersionInfo->dwMajorVersion, (conditionMask >> (1 * VER_NUM_BITS_PER_CONDITION_MASK)) & VER_CONDITION_MASK))
			return FALSE;
	if (typeMask & VER_BUILDNUMBER)
		if (!CompareVersionField(ver.dwBuildNumber, pVersionInfo->dwBuildNumber, (conditionMask >> (2 * VER_NUM_BITS_PER_CONDITION_MASK)) & VER_CONDITION_MASK))
			return FALSE;
	if (typeMask & VER_PLATFORMID)
		if (!CompareVersionField(ver.dwPlatformId, pVersionInfo->dwPlatformId, (conditionMask >> (3 * VER_NUM_BITS_PER_CONDITION_MASK)) & VER_CONDITION_MASK))
			return FALSE;
	if (typeMask & VER_SERVICEPACKMINOR)
		if (!CompareVersionField(0, pVersionInfo->wServicePackMinor, (conditionMask >> (4 * VER_NUM_BITS_PER_CONDITION_MASK)) & VER_CONDITION_MASK))
			return FALSE;
	if (typeMask & VER_SERVICEPACKMAJOR)
		if (!CompareVersionField(0, pVersionInfo->wServicePackMajor, (conditionMask >> (5 * VER_NUM_BITS_PER_CONDITION_MASK)) & VER_CONDITION_MASK))
			return FALSE;
	if (typeMask & VER_SUITENAME)
		if (!CompareVersionField(0, pVersionInfo->wSuiteMask, (conditionMask >> (6 * VER_NUM_BITS_PER_CONDITION_MASK)) & VER_CONDITION_MASK))
			return FALSE;
	if (typeMask & VER_PRODUCT_TYPE)
		if (!CompareVersionField(0, pVersionInfo->wProductType, (conditionMask >> (7 * VER_NUM_BITS_PER_CONDITION_MASK)) & VER_CONDITION_MASK))
			return FALSE;

	return TRUE;
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

static constexpr LONG missingint = 2147483647;
static inline LONG minimum(LONG a, LONG b) { return a < b ? a : b; }
static inline LONG maximum(LONG a, LONG b) { return a > b ? a : b; }

BOOL ri::GradientFill(HDC hdc, PTRIVERTEX trivertex, ULONG nvertex, PVOID pmesh, ULONG nmesh, ULONG ulmode)
{
	if (pGradientFill)
		return pGradientFill(hdc, trivertex, nvertex, pmesh, nmesh, ulmode);

	if (nvertex < 1)
		return FALSE;

	// XXX: Only supports rectangles for now.
	RECT r { missingint, missingint, -missingint, -missingint };

	for (ULONG i = 0; i < nvertex; i++)
	{
		r.left   = minimum(r.left,   trivertex[i].x);
		r.top    = minimum(r.top,    trivertex[i].y);
		r.right  = maximum(r.right,  trivertex[i].x);
		r.bottom = maximum(r.bottom, trivertex[i].y);
	}

	COLORREF cr = RGB(trivertex->Red >> 8, trivertex->Green >> 8, trivertex->Blue >> 8);
	HBRUSH hbr = CreateSolidBrush(cr);
	FillRect(hdc, &r, hbr);
	DeleteObject(hbr);

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

BOOL ri::AnimateWindow(HWND hWnd, DWORD time, DWORD flags)
{
	if (pAnimateWindow)
		return pAnimateWindow(hWnd, time, flags);
	
	int showType = SW_SHOWNOACTIVATE;
	if (flags & AW_ACTIVATE)
		showType = SW_SHOW;
	if (flags & AW_HIDE)
		showType = SW_HIDE;
	
	ShowWindow(hWnd, showType);
	return TRUE;
}

BOOL ri::InitializeCriticalSectionAndSpinCount(LPCRITICAL_SECTION lpCriticalSection, DWORD dwSpinCount)
{
	if (!pInitializeCriticalSectionAndSpinCount)
	{
		::InitializeCriticalSection(lpCriticalSection);
		return TRUE;
	}

	return pInitializeCriticalSectionAndSpinCount(lpCriticalSection, dwSpinCount);
}

LONG ri::InterlockedExchangeAdd(LONG* Addend, LONG Value)
{
	// Please I beg you. Do not reference Kernel32.dll!InterlockedExchangeAdd here.
	// Nope, neither variant seems to do that, instead generating a `lock xadd` like they should.
#ifdef _MSC_VER
	return _InterlockedExchangeAdd(Addend, Value);
#else
	return __atomic_fetch_add(Addend, Value, __ATOMIC_SEQ_CST);
#endif
}

BOOL ri::TryEnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
	if (pTryEnterCriticalSection)
		return pTryEnterCriticalSection(lpCriticalSection);

	// NOTE: Currently, neither iprog::recursive_mutex::try_lock nor iprog::mutex::try_lock
	// seem to be used. So, on Windows 95, we're safe, for now ...
	return FALSE;
}

namespace ri {
namespace internal {

	static HBRUSH _dcBrush = NULL;
	static HPEN _dcPen = NULL;
	static COLORREF _dcBrushColor, _dcPenColor;

	bool MayKeepBrush(COLORREF newBrushColor)
	{
		return _dcBrushColor == newBrushColor && _dcBrush;
	}
	bool MayKeepPen(COLORREF newPenColor)
	{
		return _dcPenColor == newPenColor && _dcPen;
	}

	void DeleteDCBrush()
	{
		if (_dcBrush)
			DeleteObject(_dcBrush);
	}

	void CreateDCBrush()
	{
		DeleteDCBrush();
		_dcBrush = CreateSolidBrush(_dcBrushColor);
	}

	void DeleteDCPen()
	{
		if (_dcPen)
			DeleteObject(_dcPen);
	}

	void CreateDCPen()
	{
		DeleteDCPen();
		_dcPen = CreatePen(PS_SOLID, 1, _dcPenColor);
	}

	void CreateDCBrushIfNeeded()
	{
		if (!_dcBrush)
			CreateDCBrush();
	}

	void CreateDCPenIfNeeded()
	{
		if (!_dcPen)
			CreateDCPen();
	}
}
}

// TODO: Perhaps we can just delete the DC brush once they call
// SetDCBrushColor with the "old color"? Just return 0xFFFFFFFF
// every time, and when you receive it as parameter, bam, no more
// brush for you.
//
// !! Might break things, if text is drawn in between two
// SetDCBrushColor calls for example... I'll keep it as it is for now.

COLORREF ri::SetDCBrushColor(HDC hdc, COLORREF color)
{
	if (pSetDCBrushColor)
		return pSetDCBrushColor(hdc, color);

	using namespace ri::internal;

	// NOTE: This is *NOT* a 100% compliant implementation
	// given that you have to call `ri::GetDCBrush()` and it
	// might return a different brush, and you also can't hang
	// onto the DC brush handle because it might be deleted!
	if (MayKeepBrush(color))
		return color;

	COLORREF oldColor = _dcBrushColor;

	_dcBrushColor = color;
	CreateDCBrush();

	return oldColor;
}

COLORREF ri::SetDCPenColor(HDC hdc, COLORREF color)
{
	if (pSetDCPenColor)
		return pSetDCPenColor(hdc, color);

	using namespace ri::internal;

	// NOTE: This is *NOT* a 100% compliant implementation
	// given that you have to call `ri::GetDCPen()` and it
	// might return a different pen, and you also can't hang
	// onto the DC brush handle because it might be deleted!
	if (MayKeepPen(color))
		return color;

	COLORREF oldColor = _dcPenColor;

	_dcPenColor = color;
	CreateDCPen();

	return oldColor;
}

HBRUSH ri::GetDCBrush()
{
	if (pSetDCBrushColor)
		return (HBRUSH) GetStockObject(DC_BRUSH);

	ri::internal::CreateDCBrushIfNeeded();
	return ri::internal::_dcBrush;
}

HPEN ri::GetDCPen()
{
	if (pSetDCPenColor)
		return (HPEN) GetStockObject(DC_PEN);

	ri::internal::CreateDCPenIfNeeded();
	return ri::internal::_dcPen;
}
