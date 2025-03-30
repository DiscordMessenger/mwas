#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <cassert>
#include <tchar.h>
#include "ri/reimpl.hpp"
#include "ri/recrypt.hpp"
#include "ri/resock2.hpp"

#ifdef _MSC_VER
#include <intrin.h>
#endif

#include <commctrl.h>
#include <shellapi.h>

#include <map>

#ifdef UNICODE
#define UNIVER "W"
#else
#define UNIVER "A"
#endif

#ifdef MINGW_SPECIFIC_HACKS
extern "C" int _CRT_MT;
#endif

namespace ri
{
	static bool s_bSupportsAtomics = false;

	HMODULE hLibKernel32;
	HMODULE hLibUser32;
	HMODULE hLibGdi32;
	HMODULE hLibShell32;
	HMODULE hLibMsimg32;
	HMODULE hLibShlwapi;
	HMODULE hLibCrypt32;
	HMODULE hLibWs2_32;
	HMODULE hLibOle32;
	HMODULE hLibComCtl32;

	// Kernel32
	typedef ULONGLONG(WINAPI* PFNVERSETCONDITIONMASK)(ULONGLONG ConditionMask, DWORD TypeMask, BYTE Condition);
	typedef BOOL(WINAPI* PFNGETFILESIZEEX)(HANDLE hFile, PLARGE_INTEGER lpFileSize);
	typedef BOOL(WINAPI* PFNSETFILEPOINTEREX)(HANDLE hFile, LARGE_INTEGER liDistanceToMove, PLARGE_INTEGER lpNewFilePointer, DWORD dwMoveMethod);
	typedef BOOL(WINAPI* PFNUNREGISTERWAITEX)(HANDLE WaitHandle, HANDLE CompletionEvent);
	typedef BOOL(WINAPI* PFNREGISTERWAITFORSINGLEOBJECT)(PHANDLE phNewWaitObject, HANDLE hObject, WAITORTIMERCALLBACK Callback, PVOID Context, ULONG dwMilliseconds, ULONG dwFlags);
	typedef BOOL(WINAPI* PFNVERIFYVERSIONINFO)(LPOSVERSIONINFOEX pVersionInfo, DWORD typeMask, DWORDLONG conditionMask);
	typedef BOOL(WINAPI* PFNINITIALIZECRITICALSECTIONANDSPINCOUNT)(LPCRITICAL_SECTION lpCriticalSection, DWORD dwSpinCount);
	typedef BOOL(WINAPI* PFNTRYENTERCRITICALSECTION)(LPCRITICAL_SECTION lpCriticalSection);
	typedef BOOL(WINAPI* PFNGETVERSIONEX)(OSVERSIONINFO*);
	typedef DWORD(WINAPI* PFNQUEUEUSERAPC)(PAPCFUNC pfnAPC, HANDLE hThread, ULONG_PTR dwData);

	// MsImg32
	typedef BOOL(WINAPI* PFNGRADIENTFILL)(HDC hdc, PTRIVERTEX trivertex, ULONG nvertex, PVOID pmesh, ULONG nmesh, ULONG ulmode);
	typedef BOOL(WINAPI* PFNALPHABLEND)(HDC hdc, int xod, int yod, int wd, int hd, HDC hdcs, int xos, int yos, int ws, int hs, BLENDFUNCTION bf);
	typedef BOOL(WINAPI* PFNTRANSPARENTBLT)(HDC hdc, int xod, int yod, int wd, int hd, HDC hdcs, int xos, int yos, int ws, int hs, UINT crt);

	// Shell32
	typedef HRESULT(WINAPI* PFNSHGETFOLDERPATH)(HWND hwnd, int csidl, HANDLE hndl, DWORD dwf, LPTSTR szp);
	typedef DWORD_PTR(WINAPI* PFNSHGETFILEINFO)(LPCTSTR pszPath, DWORD dwfa, SHFILEINFO* psfi, UINT, UINT);
	typedef BOOL(WINAPI* PFNSHELL_NOTIFYICON)(DWORD, PNOTIFYICONDATA);

	// Shlwapi
	typedef BOOL(WINAPI* PFNPATHFILEEXISTS)(LPCTSTR pszPath);

	// Gdi32
	typedef COLORREF(WINAPI* PFNSETDCBRUSHCOLOR)(HDC hdc, COLORREF color);
	typedef COLORREF(WINAPI* PFNSETDCPENCOLOR)(HDC hdc, COLORREF color);
	typedef HBITMAP(WINAPI* PFNCREATEDIBSECTION)(HDC hdc, const BITMAPINFO*, UINT, VOID**, HANDLE, DWORD);

	// User32
	typedef BOOL(WINAPI* PFNGETMENUINFO)(HMENU hMenu, LPMENUINFO lpMenuInfo);
	typedef BOOL(WINAPI* PFNSETMENUINFO)(HMENU hMenu, LPMENUINFO lpMenuInfo);
	typedef BOOL(WINAPI* PFNGETGESTUREINFO)(HGESTUREINFO hGestureInfo, PGESTUREINFO pGestureInfo);
	typedef HMONITOR(WINAPI* PFNMONITORFROMPOINT)(POINT pt, DWORD flags);
	typedef BOOL(WINAPI* PFNGETMONITORINFO)(HMONITOR hmon, LPMONITORINFO lpmi);
	typedef BOOL(WINAPI* PFNANIMATEWINDOW)(HWND hwnd, DWORD time, DWORD flags);
	typedef BOOL(WINAPI* PFNTRACKMOUSEEVENT)(LPTRACKMOUSEEVENT lpTME);
	typedef BOOL(WINAPI* PFNDRAWEDGE)(HDC, LPRECT, UINT, UINT);
	typedef BOOL(WINAPI* PFNDRAWICONEX)(HDC, int, int, HICON, int, int, UINT, HBRUSH, UINT);
	typedef BOOL(WINAPI* PFNGETSCROLLINFO)(HWND, int, LPSCROLLINFO);
	typedef BOOL(WINAPI* PFNSETSCROLLINFO)(HWND, int, LPSCROLLINFO, BOOL);
	typedef HBRUSH(WINAPI* PFNGETSYSCOLORBRUSH)(int);
	typedef HBITMAP(WINAPI* PFNLOADIMAGE)(HINSTANCE, LPCTSTR, UINT, int, int, UINT);

	// Crypt32
	typedef HCERTSTORE    (WINAPI* PFNCERTOPENSYSTEMSTOREA)       (HCRYPTPROV_LEGACY, LPCSTR);
	typedef BOOL          (WINAPI* PFNCERTCLOSESTORE)             (HCERTSTORE, DWORD);
	typedef PCCERT_CONTEXT(WINAPI* PFNCERTFINDCERTIFICATEINSTORE) (HCERTSTORE, DWORD, DWORD, DWORD, const void*, PCCERT_CONTEXT);
	typedef BOOL          (WINAPI* PFNCERTFREECERTIFICATECONTEXT) (PCCERT_CONTEXT);
	typedef PCCERT_CONTEXT(WINAPI* PFNCERTENUMCERTIFICATESINSTORE)(HCERTSTORE, PCCERT_CONTEXT);

	// Ws2_32
	typedef INT(WSAAPI* PFNWSASTRINGTOADDRESSA)(LPSTR, INT, LPWSAPROTOCOL_INFOA, LPSOCKADDR, LPINT);
	typedef INT(WSAAPI* PFNWSAADDRESSTOSTRINGA)(LPSOCKADDR, DWORD, LPWSAPROTOCOL_INFOA, LPSTR, LPDWORD);
	typedef INT(WSAAPI* PFNWSASEND)(SOCKET, LPWSABUF, DWORD, LPDWORD, DWORD, LPWSAOVERLAPPED, LPWSAOVERLAPPED_COMPLETION_ROUTINE);
	typedef INT(WSAAPI* PFNWSASENDTO)(SOCKET, LPWSABUF, DWORD, LPDWORD, DWORD, const sockaddr*, int, LPWSAOVERLAPPED, LPWSAOVERLAPPED_COMPLETION_ROUTINE);
	typedef INT(WSAAPI* PFNWSARECV)(SOCKET, LPWSABUF, DWORD, LPDWORD, LPDWORD, LPWSAOVERLAPPED, LPWSAOVERLAPPED_COMPLETION_ROUTINE);
	typedef INT(WSAAPI* PFNWSARECVFROM)(SOCKET, LPWSABUF, DWORD, LPDWORD, LPDWORD, sockaddr*, LPINT, LPWSAOVERLAPPED, LPWSAOVERLAPPED_COMPLETION_ROUTINE);
	typedef SOCKET(WSAAPI* PFNWSASOCKETA)(int, int, int, LPWSAPROTOCOL_INFOA, GROUP, DWORD);

	// Comctl32
	typedef HIMAGELIST(WINAPI* PFNIMAGELIST_CREATE)(int, int, UINT, int, int);
	typedef INT(WINAPI* PFNIMAGELIST_ADD)(HIMAGELIST, HBITMAP, HBITMAP);
	typedef BOOL(WINAPI* PFNIMAGELIST_DESTROY)(HIMAGELIST);
	typedef INT(WINAPI* PFNIMAGELIST_GETIMAGECOUNT)(HIMAGELIST);
	typedef INT(WINAPI* PFNIMAGELIST_REPLACEICON)(HIMAGELIST, int, HICON);
	typedef HWND(WINAPI* PFNCREATESTATUSWINDOWA)(LONG, LPCSTR, HWND, UINT);
	
	// Ole32
	typedef HRESULT(WINAPI* PFNCOINITIALIZE)(LPVOID);
	
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
	PFNSHGETFILEINFO pSHGetFileInfo; // Points to the W version on UNICODE and the A version otherwise.
	PFNSHELL_NOTIFYICON pShell_NotifyIcon;
	PFNPATHFILEEXISTS pPathFileExists;
	PFNGETMENUINFO pGetMenuInfo;
	PFNSETMENUINFO pSetMenuInfo;
	PFNGETGESTUREINFO pGetGestureInfo;
	PFNSETDCBRUSHCOLOR pSetDCBrushColor;
	PFNSETDCPENCOLOR pSetDCPenColor;
	PFNMONITORFROMPOINT pMonitorFromPoint;
	PFNGETMONITORINFO pGetMonitorInfo;
	PFNANIMATEWINDOW pAnimateWindow;
	PFNTRACKMOUSEEVENT pTrackMouseEvent;
	PFNINITIALIZECRITICALSECTIONANDSPINCOUNT pInitializeCriticalSectionAndSpinCount;
	PFNTRYENTERCRITICALSECTION pTryEnterCriticalSection;
	PFNQUEUEUSERAPC pQueueUserAPC;
	PFNGETVERSIONEX pGetVersionEx;
	PFNCERTOPENSYSTEMSTOREA pCertOpenSystemStoreA;
	PFNCERTCLOSESTORE pCertCloseStore;
	PFNCERTFINDCERTIFICATEINSTORE pCertFindCertificateInStore;
	PFNCERTFREECERTIFICATECONTEXT pCertFreeCertificateContext;
	PFNCERTENUMCERTIFICATESINSTORE pCertEnumCertificatesInStore;
	PFNWSAADDRESSTOSTRINGA pWSAAddressToStringA;
	PFNWSASTRINGTOADDRESSA pWSAStringToAddressA;
	PFNWSASEND pWSASend;
	PFNWSASENDTO pWSASendTo;
	PFNWSARECV pWSARecv;
	PFNWSARECVFROM pWSARecvFrom;
	PFNWSASOCKETA pWSASocketA;
	PFNCOINITIALIZE pCoInitialize;
	PFNIMAGELIST_CREATE pImageList_Create;
	PFNIMAGELIST_ADD pImageList_Add;
	PFNIMAGELIST_DESTROY pImageList_Destroy;
	PFNIMAGELIST_GETIMAGECOUNT pImageList_GetImageCount;
	PFNIMAGELIST_REPLACEICON pImageList_ReplaceIcon;
	PFNCREATESTATUSWINDOWA pCreateStatusWindowA;
	PFNCREATEDIBSECTION pCreateDIBSection;
	PFNDRAWEDGE pDrawEdge;
	PFNDRAWICONEX pDrawIconEx;
	PFNGETSCROLLINFO pGetScrollInfo;
	PFNSETSCROLLINFO pSetScrollInfo;
	PFNGETSYSCOLORBRUSH pGetSysColorBrush;
	PFNLOADIMAGE pLoadImage;
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
	hLibCrypt32  = LoadLibrary(TEXT("crypt32.dll"));
	hLibWs2_32   = LoadLibrary(TEXT("ws2_32.dll"));
	hLibOle32    = LoadLibrary(TEXT("ole32.dll"));
	hLibComCtl32 = LoadLibrary(TEXT("comctl32.dll"));

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
		pGetVersionEx = (PFNGETVERSIONEX)GetProcAddress(hLibKernel32, "GetVersionEx" UNIVER);
		pQueueUserAPC = (PFNQUEUEUSERAPC)GetProcAddress(hLibKernel32, "QueueUserAPC");
	}

	if (hLibUser32)
	{
		pGetMenuInfo = (PFNGETMENUINFO)GetProcAddress(hLibUser32, "GetMenuInfo");
		pSetMenuInfo = (PFNSETMENUINFO)GetProcAddress(hLibUser32, "SetMenuInfo");
		pGetGestureInfo = (PFNGETGESTUREINFO)GetProcAddress(hLibUser32, "GetGestureInfo");
		pGetMonitorInfo = (PFNGETMONITORINFO)GetProcAddress(hLibUser32, "GetMonitorInfo" UNIVER);
		pMonitorFromPoint = (PFNMONITORFROMPOINT)GetProcAddress(hLibUser32, "MonitorFromPoint");
		pAnimateWindow = (PFNANIMATEWINDOW)GetProcAddress(hLibUser32, "AnimateWindow");
		pTrackMouseEvent = (PFNTRACKMOUSEEVENT)GetProcAddress(hLibUser32, "TrackMouseEvent");
		pDrawEdge = (PFNDRAWEDGE)GetProcAddress(hLibUser32, "DrawEdge");
		pDrawIconEx = (PFNDRAWICONEX)GetProcAddress(hLibUser32, "DrawIconEx");
		pGetScrollInfo = (PFNGETSCROLLINFO)GetProcAddress(hLibUser32, "GetScrollInfo");
		pSetScrollInfo = (PFNSETSCROLLINFO)GetProcAddress(hLibUser32, "SetScrollInfo");
		pGetSysColorBrush = (PFNGETSYSCOLORBRUSH)GetProcAddress(hLibUser32, "GetSysColorBrush");
		pLoadImage = (PFNLOADIMAGE)GetProcAddress(hLibUser32, "LoadImage" UNIVER);
	}

	if (hLibGdi32)
	{
		pSetDCBrushColor = (PFNSETDCBRUSHCOLOR)GetProcAddress(hLibGdi32, "SetDCBrushColor");
		pSetDCPenColor = (PFNSETDCPENCOLOR)GetProcAddress(hLibGdi32, "SetDCPenColor");
		pCreateDIBSection = (PFNCREATEDIBSECTION)GetProcAddress(hLibGdi32, "CreateDIBSection");
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
		pSHGetFileInfo = (PFNSHGETFILEINFO)GetProcAddress(hLibShell32, "SHGetFileInfo" UNIVER);
		pShell_NotifyIcon = (PFNSHELL_NOTIFYICON)GetProcAddress(hLibShell32, "Shell_NotifyIcon" UNIVER);
	}

	if (hLibShlwapi)
	{
		pPathFileExists = (PFNPATHFILEEXISTS)GetProcAddress(hLibShlwapi, "PathFileExists" UNIVER);
	}

	if (hLibCrypt32)
	{
		pCertOpenSystemStoreA        = (PFNCERTOPENSYSTEMSTOREA)       GetProcAddress(hLibCrypt32, "CertOpenSystemStoreA");
		pCertCloseStore              = (PFNCERTCLOSESTORE)             GetProcAddress(hLibCrypt32, "CertCloseStore");
		pCertFreeCertificateContext  = (PFNCERTFREECERTIFICATECONTEXT) GetProcAddress(hLibCrypt32, "CertFreeCertificateContext");
		pCertFindCertificateInStore  = (PFNCERTFINDCERTIFICATEINSTORE) GetProcAddress(hLibCrypt32, "CertFindCertificateInStore");
		pCertEnumCertificatesInStore = (PFNCERTENUMCERTIFICATESINSTORE)GetProcAddress(hLibCrypt32, "CertEnumCertificatesInStore");
	}

	if (hLibWs2_32)
	{
		pWSAAddressToStringA = (PFNWSAADDRESSTOSTRINGA) GetProcAddress(hLibWs2_32, "WSAAddressToStringA");
		pWSAStringToAddressA = (PFNWSASTRINGTOADDRESSA) GetProcAddress(hLibWs2_32, "WSAStringToAddressA");
		pWSASend             = (PFNWSASEND)             GetProcAddress(hLibWs2_32, "WSASend");
		pWSASendTo           = (PFNWSASENDTO)           GetProcAddress(hLibWs2_32, "WSASendTo");
		pWSARecv             = (PFNWSARECV)             GetProcAddress(hLibWs2_32, "WSARecv");
		pWSARecvFrom         = (PFNWSARECVFROM)         GetProcAddress(hLibWs2_32, "WSARecvFrom");
		pWSASocketA          = (PFNWSASOCKETA)          GetProcAddress(hLibWs2_32, "WSASocketA");
	}
	
	if (hLibOle32)
	{
		pCoInitialize = (PFNCOINITIALIZE) GetProcAddress(hLibOle32, "CoInitialize");
	}

	if (hLibComCtl32)
	{
		pImageList_Add           = (PFNIMAGELIST_ADD)          GetProcAddress(hLibComCtl32, "ImageList_Add");
		pImageList_Create        = (PFNIMAGELIST_CREATE)       GetProcAddress(hLibComCtl32, "ImageList_Create");
		pImageList_Destroy       = (PFNIMAGELIST_DESTROY)      GetProcAddress(hLibComCtl32, "ImageList_Destroy");
		pImageList_GetImageCount = (PFNIMAGELIST_GETIMAGECOUNT)GetProcAddress(hLibComCtl32, "ImageList_GetImageCount");
		pImageList_ReplaceIcon   = (PFNIMAGELIST_REPLACEICON)  GetProcAddress(hLibComCtl32, "ImageList_ReplaceIcon");

		pCreateStatusWindowA = (PFNCREATESTATUSWINDOWA) GetProcAddress(hLibComCtl32, "CreateStatusWindowA");
		if (!pCreateStatusWindowA)
			// Windows NT 3.1's comctl32.dll exports just CreateStatusWindow, which takes ANSI text. Use that instead.
			pCreateStatusWindowA = (PFNCREATESTATUSWINDOWA) GetProcAddress(hLibComCtl32, "CreateStatusWindow");
	}
	
	// Test if atomic instructions are supported
	s_bSupportsAtomics = true;

	// If we are Windows NT and our version >= 4, then atomics should be used.
	OSVERSIONINFO ovi{};
	ovi.dwOSVersionInfoSize = sizeof ovi;
	ri::GetVersionEx(&ovi);

	if (ovi.dwMajorVersion < 4 || ovi.dwPlatformId != VER_PLATFORM_WIN32_NT)
	{
		s_bSupportsAtomics = false;

#ifdef MINGW_SPECIFIC_HACKS
		_CRT_MT = 0;
#endif
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

	if (!ri::GetVersionEx(&ver))
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

DWORD_PTR ri::SHGetFileInfo(LPCTSTR pszPath, DWORD dwFileAttributes, SHFILEINFO* psfi, UINT cbFileInfo, UINT uFlags)
{
	if (pSHGetFileInfo)
		return pSHGetFileInfo(pszPath, dwFileAttributes, psfi, cbFileInfo, uFlags);

	psfi->hIcon = 0;
	return 0;
}

BOOL ri::Shell_NotifyIcon(DWORD dwMessage, NOTIFYICONDATA* nid)
{
	if (pShell_NotifyIcon)
		return pShell_NotifyIcon(dwMessage, nid);

	// Just do nothing.
	return 0;
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
	return ::_InterlockedExchangeAdd(Addend, Value);
#else
	if (s_bSupportsAtomics)
	{
		return __atomic_fetch_add(Addend, Value, __ATOMIC_SEQ_CST);
	}
	else
	{
		LONG Old = *Addend;
		*Addend += Value;
		return Old;
	}
#endif
}

DWORD ri::QueueUserAPC(PAPCFUNC pfnAPC, HANDLE hThread, ULONG_PTR dwData)
{
	// Asio uses this (win_thread.ipp), in its `join()` function.
	// It doesn't check for the return value or anything.
	//
	// As far as I can tell, it's purely there to cancel IO operations. I don't
	// know of an alternative... so stubbed it is.
	if (pQueueUserAPC)
		return pQueueUserAPC(pfnAPC, hThread, dwData);

	return 0;

}

BOOL ri::GetVersionEx(LPOSVERSIONINFO lpvi)
{
	if (pGetVersionEx)
		return pGetVersionEx(lpvi);

	DWORD oldsize = lpvi->dwOSVersionInfoSize;
	memset(lpvi, 0, sizeof * lpvi);

	DWORD ver = GetVersion();
	lpvi->dwOSVersionInfoSize = oldsize;
	lpvi->dwMajorVersion = LOBYTE(ver);
	lpvi->dwMinorVersion = HIBYTE(LOWORD(ver));
	lpvi->dwBuildNumber = HIWORD(ver);
	// TODO: detect NT
	lpvi->dwPlatformId = VER_PLATFORM_WIN32_WINDOWS;

	_tcscpy(lpvi->szCSDVersion, TEXT("Unknown Windows"));
	return TRUE;
}

BOOL ri::TryEnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
	if (pTryEnterCriticalSection)
		return pTryEnterCriticalSection(lpCriticalSection);

	// NOTE: Currently, neither iprog::recursive_mutex::try_lock nor iprog::mutex::try_lock
	// seem to be used. So, on Windows 95, we're safe, for now ...
	return FALSE;
}

BOOL ri::TrackMouseEvent(LPTRACKMOUSEEVENT lptme)
{
	if (pTrackMouseEvent)
		return pTrackMouseEvent(lptme);

	// TODO?
	return FALSE;
}

#define DRE_BLACKOUTER  0x80000000
#define DRE_HOT         0x40000000
BOOL ri::DrawEdge(HDC hdc, LPRECT lprect, UINT style, UINT grfFlags)
{
	if (pDrawEdge)
		return pDrawEdge(hdc, lprect, style, grfFlags);

	RECT rect = *lprect;

	// copied from NanoShell, which kinda recreated Windows
	if (grfFlags & BF_FLAT)
	{
		grfFlags &= ~BF_FLAT;
		style &= ~(BDR_INNER | BDR_OUTER);
		style |=  DRE_BLACKOUTER;
	}
	
	// the depth levels are as follows:
	// 6 - BUTTON_HILITE_COLOR
	// 5 - Avg(BUTTON_HILITE_COLOR, BUTTON_MIDDLE_COLOR)
	// 4 - BUTTON_MIDDLE_COLOR
	// 3 - WINDOW_BORDER_COLOR
	// 2 - BUTTON_SHADOW_COLOR
	// 1 - BUTTON_XSHADOW_COLOR
	// 0 - BUTTON_EDGE_COLOR
	
	uint32_t tl = 0, br = 0;
	
	// the flags we need to check, in order of priority.
	// These match up with their definitions in window.h and represent the amount 1 is shifted by.
	static const int flags[] = { 4, 3, 1, 2, 0 };
	
	int colors[] =
	{
		0, //BUTTON_EDGE_COLOR,
		0, //BUTTON_XSHADOW_COLOR,
		0, //BUTTON_SHADOW_COLOR,
		0, //WINDOW_BORDER_COLOR,
		0, //0,
		0, //0,
		0, //BUTTON_HILITE_COLOR,
	};

	colors[0] = GetSysColor(COLOR_WINDOWFRAME);
	colors[1] = GetSysColor(COLOR_3DDKSHADOW);
	colors[2] = GetSysColor(COLOR_3DSHADOW);
	colors[3] = GetSysColor(COLOR_ACTIVEBORDER);
	colors[6] = GetSysColor(COLOR_3DHILIGHT);
	
	if ((style & DRE_BLACKOUTER) && colors[1] == 0)
		colors[1] = colors[2];
	
	if (style & DRE_HOT)
		colors[4] = GetSysColor(COLOR_3DLIGHT);
	else
		colors[4] = GetSysColor(COLOR_3DFACE);
	
	// get color #5.
	unsigned colorAvg = 0;
	colorAvg |= ((colors[6] & 0xff0000) + (colors[4] & 0xff0000)) >> 1;
	colorAvg |= ((colors[6] & 0x00ff00) + (colors[4] & 0x00ff00)) >> 1;
	colorAvg |= ((colors[6] & 0x0000ff) + (colors[4] & 0x0000ff)) >> 1;
	colors[5] = colorAvg;
	
	// 4 pairs of ints corresponding to the border type. These ints are
	// indices into the colors array.
	static const int color_indices[] =
	{
		6, 2, // raised inner
		1, 2, // sunken inner
		5, 1, // raised outer
		2, 6,
		0, 0,
	};

#define ARRAY_COUNT(x) (sizeof(x)/sizeof((x)[0]))
	for (int i = 0; i < (int)ARRAY_COUNT(flags); i++)
	{
		if (~style & (1 << flags[i])) continue;
		
		tl = colors[color_indices[0 + 2 * flags[i]]];
		br = colors[color_indices[1 + 2 * flags[i]]];
		
		// top left
		HPEN hp = CreatePen(PS_SOLID, 1, tl);
		HGDIOBJ go = SelectObject(hdc, hp);
		POINT ptold;
		
		//VidDrawHLine(tl, rect.left, rect.right, rect.top);
		MoveToEx(hdc, rect.left, rect.top, &ptold);
		if (grfFlags & BF_TOP) {
			LineTo(hdc, rect.right, rect.top);
		}

		//VidDrawVLine(tl, rect.top, rect.bottom, rect.left);
		if (grfFlags & BF_LEFT) {
			MoveToEx(hdc, rect.left, rect.top, NULL);
			LineTo(hdc, rect.left, rect.bottom);
		}

		SelectObject(hdc, go);
		DeleteObject(hp);

		// bottom right
		hp = CreatePen(PS_SOLID, 1, br);
		go = SelectObject(hdc, hp);
		
		//VidDrawHLine(br, rect.left, rect.right, rect.bottom);
		if (grfFlags & BF_BOTTOM) {
			MoveToEx(hdc, rect.left, rect.bottom - 1, NULL);
			LineTo(hdc, rect.right, rect.bottom - 1);
		}

		//VidDrawVLine(br, rect.top, rect.bottom, rect.right);
		if (grfFlags & BF_RIGHT) {
			MoveToEx(hdc, rect.right - 1, rect.top, NULL);
			LineTo(hdc, rect.right - 1, rect.bottom);
		}

		SelectObject(hdc, go);
		DeleteObject(hp);
		MoveToEx(hdc, ptold.x, ptold.y, NULL);
		
		rect.left++;
		rect.top++;
		rect.right--;
		rect.bottom--;
	}
	
	if (grfFlags & BF_MIDDLE)
		//VidFillRectangle(bg, rect);
		FillRect(hdc, &rect, GetSysColorBrush(COLOR_3DFACE));

	if (grfFlags & BF_ADJUST)
		*lprect = rect;

#undef ARRAY_COUNT

	return TRUE;
}

BOOL ri::DrawIconEx(HDC hdc, int x, int y, HICON hicon, int cx, int cy, UINT isiac, HBRUSH hbrffd, UINT dif)
{
	if (pDrawIconEx)
		return pDrawIconEx(hdc, x, y, hicon, cx, cy, isiac, hbrffd, dif);

	// TODO: emulate it properly?
	return DrawIcon(hdc, x, y, hicon);
}

namespace ri
{
	struct HWndAndBar {
		HWND hwnd;
		int bar;

		bool operator<(const HWndAndBar& oth) const {
			if (hwnd != oth.hwnd)
				return hwnd < oth.hwnd;

			return bar < oth.bar;
		}
	};

	std::map<HWndAndBar, SCROLLINFO> m_scrollInfo;

	HWndAndBar HB(HWND hwnd, int bar) {
		return { hwnd, bar };
	}
}

BOOL ri::GetScrollInfo(HWND hwnd, int nBar, LPSCROLLINFO scrollInfo)
{
	if (pGetScrollInfo)
		return pGetScrollInfo(hwnd, nBar, scrollInfo);

	SCROLLINFO& psi = m_scrollInfo[HB(hwnd, nBar)];

	if (scrollInfo->fMask & SIF_POS)
		scrollInfo->nPos = psi.nPos = GetScrollPos(hwnd, nBar);

	if (scrollInfo->fMask & SIF_TRACKPOS)
		scrollInfo->nTrackPos = psi.nTrackPos;
	
	if (scrollInfo->fMask & SIF_RANGE)
		scrollInfo->nMin = psi.nMin, scrollInfo->nMax = psi.nMax;

	if (scrollInfo->fMask & SIF_PAGE)
		scrollInfo->nPage = psi.nPage;

	return 0;
}

BOOL ri::SetScrollInfo(HWND hwnd, int nBar, LPSCROLLINFO lpsi, BOOL redraw)
{
	if (pSetScrollInfo)
		return pSetScrollInfo(hwnd, nBar, lpsi, redraw);

	SCROLLINFO* Info = &m_scrollInfo[HB(hwnd, nBar)];

	// NOTE: partly copied from ReactOS
	BOOL bChangeParams = FALSE;
	UINT MaxPage = 0;
	int OldPos = 0;
	int MaxPos = 0;

	// Set the page size
	if (lpsi->fMask & SIF_PAGE)
	{
		if (Info->nPage != lpsi->nPage)
		{
			Info->nPage = lpsi->nPage;
			bChangeParams = TRUE;
		}
	}

	// Set the scroll pos
	if (lpsi->fMask & SIF_POS)
	{
		OldPos = Info->nPos;
		if (Info->nPos != lpsi->nPos)
		{
			Info->nPos = lpsi->nPos;
		}
	}

	// Set the scroll range
	if (lpsi->fMask & SIF_RANGE)
	{
		if (lpsi->nMin > lpsi->nMax)
		{
			Info->nMin = lpsi->nMin;
			Info->nMax = lpsi->nMin;
			bChangeParams = TRUE;
		}
		else if (Info->nMin != lpsi->nMin || Info->nMax != lpsi->nMax)
		{
			Info->nMin = lpsi->nMin;
			Info->nMax = lpsi->nMax;
			bChangeParams = TRUE;
		}
	}

	// Make sure the page size is valid
	MaxPage = abs(Info->nMax - Info->nMin) + 1;
	if (Info->nPage > MaxPage)
		Info->nPage = MaxPage;

	// Make sure the pos is inside the range
#define MAXIMUM(a, b) ((a) > (b) ? (a) : (b))
	MaxPos = Info->nMax + 1 - (int)MAXIMUM(Info->nPage, 1);
	if (Info->nPos < Info->nMin)
		Info->nPos = Info->nMin;
	else if (Info->nPos > MaxPos)
		Info->nPos = MaxPos;
#undef MAXIMUM

	if (lpsi->fMask & SIF_RANGE)
	{
		// redraw only if redraw is set and SIF_POS isn't set
		SetScrollRange(hwnd, nBar, Info->nMin, Info->nMax, redraw && (~lpsi->fMask & SIF_POS));
	}

	if (lpsi->fMask & SIF_POS)
	{
		SetScrollPos(hwnd, nBar, Info->nPos, redraw);
	}


	/*
	if (scrollInfo->fMask & SIF_PAGE) {
		psi.nPage = scrollInfo->nPage;
	}

	if (scrollInfo->fMask & SIF_RANGE) {
		psi.nMin = scrollInfo->nMin;
		psi.nMax = scrollInfo->nMax;

		// calculate the actual range and set it
		int max = psi.nMax - psi.nPage;
		SetScrollRange(hwnd, nBar, scrollInfo->nMin, max, redraw);
	}

	if (scrollInfo->fMask & SIF_TRACKPOS) {
		psi.nTrackPos = scrollInfo->nTrackPos;
		if (psi.nTrackPos >= psi.nMax - psi.nPage)
			psi.nTrackPos = psi.nMax - psi.nPage - 1;
		if (psi.nTrackPos < psi.nMin)
			psi.nTrackPos = psi.nMin;
	}

	if (scrollInfo->fMask & SIF_POS) {
		psi.nPos = scrollInfo->nPos;
		if (psi.nPos >= psi.nMax - psi.nPage)
			psi.nPos = psi.nMax - psi.nPage - 1;
		if (psi.nPos < psi.nMin)
			psi.nPos = psi.nMin;

		SetScrollPos(hwnd, nBar, scrollInfo->nPos, redraw);
	}*/

	return 0;
}

HBRUSH ri::GetSysColorBrush(int type)
{
	if (pGetSysColorBrush)
		return pGetSysColorBrush(type);

	static HBRUSH hBrushCache[40];
	if (!hBrushCache[type])
		hBrushCache[type] = CreateSolidBrush(GetSysColor(type));

	if (!hBrushCache[type])
		// I mean come on
		hBrushCache[type] = (HBRUSH) GetStockObject(BLACK_BRUSH);

	return hBrushCache[type];
}

HBITMAP ri::LoadImage(HINSTANCE hInst, LPCTSTR name, UINT type, int cx, int cy, UINT fuLoad)
{
	if (pLoadImage)
		return pLoadImage(hInst, name, type, cx, cy, fuLoad);

	// TODO
	return NULL;
}

#ifndef EAFNOSUPPORT
#define EAFNOSUPPORT 102
#endif

INT ri::WSAStringToAddressA(LPSTR addressString, INT addressFamily, LPWSAPROTOCOL_INFOA protocolInfo, LPSOCKADDR address, LPINT addressLength)
{
	if (pWSAStringToAddressA)
		return pWSAStringToAddressA(addressString, addressFamily, protocolInfo, address, addressLength);

	// Reimplementation
	if (addressFamily != AF_INET)
	{
		// IPv6 isn't supported by Winsock 1 anyway AFAIK
		::WSASetLastError(EAFNOSUPPORT);
		return SOCKET_ERROR;
	}

	PSOCKADDR_IN addrIn = (PSOCKADDR_IN)address;

	addrIn->sin_family = addressFamily;
	addrIn->sin_addr.s_addr = inet_addr(addressString);
	addrIn->sin_port = 0;

	if (addrIn->sin_addr.s_addr == INADDR_NONE)
	{
		WSASetLastError(WSAEINVAL);
		return SOCKET_ERROR;
	}

	return 0;
}

INT ri::WSAAddressToStringA(LPSOCKADDR address, DWORD addressLength, LPWSAPROTOCOL_INFOA protocolInfo, LPSTR addressString, LPDWORD addressStringLength)
{
	if (pWSAAddressToStringA)
		return pWSAAddressToStringA(address, addressLength, protocolInfo, addressString, addressStringLength);

	// Reimplementation
	if (address->sa_family != AF_INET)
	{
		// IPv6 isn't supported by Winsock 1 anyway AFAIK
		::WSASetLastError(EAFNOSUPPORT);
		return SOCKET_ERROR;
	}

	PSOCKADDR_IN addrIn = (PSOCKADDR_IN)address;

	if (addressStringLength && *addressStringLength != 0)
	{
		strncpy(addressString, inet_ntoa(addrIn->sin_addr), *addressStringLength);
		addressString[*addressStringLength - 1] = 0;
		*addressStringLength = strlen(addressString);
	}
	else
	{
		strcpy(addressString, inet_ntoa(addrIn->sin_addr));
		*addressStringLength = strlen(addressString);
	}

	return 0;
}

//! I don't know how reliable this is.
INT ri::WSASend(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	if (pWSASend)
		return pWSASend(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpOverlapped, lpCompletionRoutine);

	if (!lpBuffers || dwBufferCount == 0 || !lpNumberOfBytesSent)
	{
		::WSASetLastError(WSAEINVAL);
		return SOCKET_ERROR;
	}

	DWORD totalBytesSent = 0;

	for (DWORD i = 0; i < dwBufferCount; ++i)
	{
		DWORD bytesRemaining = lpBuffers[i].len;
		const char* bufferPtr = lpBuffers[i].buf;

		while (bytesRemaining > 0)
		{
			int bytesSent = ::send(s, bufferPtr, bytesRemaining, dwFlags);
			if (bytesSent == SOCKET_ERROR)
				return SOCKET_ERROR;

			bytesRemaining -= bytesSent;
			bufferPtr += bytesSent;
			totalBytesSent += bytesSent;
		}
	}

	*lpNumberOfBytesSent = totalBytesSent;
	return 0;
}

INT ri::WSASendTo(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, const sockaddr* lpTo, int iTolen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	if (pWSASendTo)
		return pWSASendTo(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpTo, iTolen, lpOverlapped, lpCompletionRoutine);

	if (!lpBuffers || dwBufferCount == 0 || !lpNumberOfBytesSent || !lpTo)
	{
		::WSASetLastError(WSAEINVAL);
		return SOCKET_ERROR;
	}

	DWORD totalBytesSent = 0;

	for (DWORD i = 0; i < dwBufferCount; ++i)
	{
		DWORD bytesRemaining = lpBuffers[i].len;
		const char* bufferPtr = lpBuffers[i].buf;

		while (bytesRemaining > 0)
		{
			int bytesSent = ::sendto(s, bufferPtr, bytesRemaining, dwFlags, lpTo, iTolen);
			if (bytesSent == SOCKET_ERROR)
				return SOCKET_ERROR;

			bytesRemaining -= bytesSent;
			bufferPtr += bytesSent;
			totalBytesSent += bytesSent;
		}
	}

	*lpNumberOfBytesSent = totalBytesSent;
	return 0;
}

INT ri::WSARecv(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpdwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) 
{
	if (pWSARecv)
		return pWSARecv(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpdwFlags, lpOverlapped, lpCompletionRoutine);
	
	if (!lpBuffers || dwBufferCount == 0 || !lpNumberOfBytesRecvd || !lpdwFlags)
	{
		::WSASetLastError(WSAEINVAL);
		return SOCKET_ERROR;
	}

	DWORD totalBytesRecvd = 0;

	for (DWORD i = 0; i < dwBufferCount; ++i)
	{
		DWORD bytesRemaining = lpBuffers[i].len;
		char* bufferPtr = lpBuffers[i].buf;

		int bytesRecvd = ::recv(s, bufferPtr, bytesRemaining, *lpdwFlags);
		if (bytesRecvd == SOCKET_ERROR)
			return SOCKET_ERROR;

		totalBytesRecvd += bytesRecvd;
	}

	*lpNumberOfBytesRecvd = totalBytesRecvd;
	return 0;
}

INT ri::WSARecvFrom(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpdwFlags, sockaddr* lpFrom, int* lpFromlen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	if (pWSARecvFrom)
		return pWSARecvFrom(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpdwFlags, lpFrom, lpFromlen, lpOverlapped, lpCompletionRoutine);

	if (!lpBuffers || dwBufferCount == 0 || !lpNumberOfBytesRecvd || !lpdwFlags)
	{
		::WSASetLastError(WSAEINVAL);
		return SOCKET_ERROR;
	}

	DWORD totalBytesRecvd = 0;

	for (DWORD i = 0; i < dwBufferCount; ++i)
	{
		DWORD bytesRemaining = lpBuffers[i].len;
		char* bufferPtr = lpBuffers[i].buf;

		while (bytesRemaining > 0)
		{
			int bytesRecvd = ::recvfrom(s, bufferPtr, bytesRemaining, *lpdwFlags, lpFrom, lpFromlen);
			if (bytesRecvd == SOCKET_ERROR)
			{
				int error = ::WSAGetLastError();
				if (error == WSAEWOULDBLOCK)
					continue; // Try again if non-blocking
				
				return SOCKET_ERROR;
			}

			if (bytesRecvd == 0)
				break; // Connection closed

			bytesRemaining -= bytesRecvd;
			bufferPtr += bytesRecvd;
			totalBytesRecvd += bytesRecvd;
		}
	}

	*lpNumberOfBytesRecvd = totalBytesRecvd;
	return 0;
}

SOCKET ri::WSASocketA(int af, int type, int protocol, LPWSAPROTOCOL_INFOA lpProtocolInfo, GROUP g, DWORD dwFlags)
{
	if (pWSASocketA)
		return pWSASocketA(af, type, protocol, lpProtocolInfo, g, dwFlags);

	return ::socket(af, type, protocol);
}

bool ri::SupportsWSARecv()
{
	return pWSARecv != NULL;
}

int ri::GetWSVersion()
{
	if (!SupportsWSARecv())
		// we should NOT initialize WS2 if we don't have it!
		return 0x0101;

	return 0x0002;
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

HBITMAP ri::CreateDIBSection(HDC hdc, const BITMAPINFO* pbmi, UINT usage, VOID** ppvBits, HANDLE hSection, DWORD offset)
{
	if (pCreateDIBSection)
		return pCreateDIBSection(hdc, pbmi, usage, ppvBits, hSection, offset);
	
	// just don't create it
	return NULL;
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

int ri::GetHalfToneStretchMode()
{
	// WINDOWS 95: For some reason, somehow, SHLWAPI's SHGetInverseCMAP function calls
	// SHInterlockedCompareExchange.  This is a guess.  According to the MSDN docs,
	// it "retrieves the inverse color table mapping for the halftone palette".
	// Given it mentions halftone, I can only assume that the HALFTONE setting is
	// responsible. But this is a random shot in the dark.

	// No, there are no other references to SHInterlockedCompareExchange, other than
	// its export table entry, and a sub function called only by SHGetInverseCMAP.

	// TODO: Is this just a red herring?

	//if (LOBYTE(GetVersion()) < 5)
	//	return COLORONCOLOR;

	return HALFTONE;
}

int ri::GetWordEllipsisFlag()
{
	// WINDOWS NT 3.51: I think setting this flag to DrawText makes
	// text not render.  Which is a real bummer.  The windows headers
	// check if WINNT >= 0x0400 so that probably explains it.

	if (LOBYTE(GetVersion()) < 4)
		return 0;

	return DT_WORD_ELLIPSIS;
}

bool ri::SupportsScrollInfo()
{
	return pGetScrollInfo && pSetScrollInfo;
}

HCERTSTORE ri::CertOpenSystemStoreA(HCRYPTPROV_LEGACY hProv, LPCSTR szSubSystemProtocol)
{
	if (pCertOpenSystemStoreA)
		return pCertOpenSystemStoreA(hProv, szSubSystemProtocol);

	return NULL;
}

BOOL ri::CertCloseStore(HCERTSTORE hStore, DWORD dwFlags)
{
	if (pCertCloseStore)
		return pCertCloseStore(hStore, dwFlags);

	return FALSE;
}

PCCERT_CONTEXT ri::CertFindCertificateInStore(HCERTSTORE hs, DWORD dw1, DWORD dw2, DWORD dw3, const void* p1, PCCERT_CONTEXT pccc1)
{
	if (pCertFindCertificateInStore)
		return pCertFindCertificateInStore(hs, dw1, dw2, dw3, p1, pccc1);

	return NULL;
}

BOOL ri::CertFreeCertificateContext(PCCERT_CONTEXT pcc)
{
	if (pCertFreeCertificateContext)
		return pCertFreeCertificateContext(pcc);

	return FALSE;
}

PCCERT_CONTEXT ri::CertEnumCertificatesInStore(HCERTSTORE hs, PCCERT_CONTEXT pcc)
{
	if (pCertEnumCertificatesInStore)
		return pCertEnumCertificatesInStore(hs, pcc);

	return NULL;
}

HRESULT ri::CoInitialize(LPVOID Reserved)
{
	if (pCoInitialize)
		return pCoInitialize(Reserved);
	
	return 0;
}

HIMAGELIST ri::ImageList_Create(int cx, int cy, UINT flags, int cInitial, int cGrow)
{
	if (pImageList_Create)
		return pImageList_Create(cx, cy, flags, cInitial, cGrow);

	// Don't create it.
	return (HIMAGELIST) 0xCAFEBABE;
}

BOOL ri::ImageList_Destroy(HIMAGELIST himl)
{
	if (pImageList_Destroy)
		return pImageList_Destroy(himl);

	return 0;
}

BOOL ri::ImageList_Add(HIMAGELIST himl, HBITMAP hbmImage, HBITMAP hbmMask)
{
	if (pImageList_Add)
		return pImageList_Add(himl, hbmImage, hbmMask);

	return 0;
}

int ri::ImageList_GetImageCount(HIMAGELIST himl)
{
	if (pImageList_GetImageCount)
		return pImageList_GetImageCount(himl);

	return 0;
}

int ri::ImageList_ReplaceIcon(HIMAGELIST himl, int i, HICON hicon)
{
	if (pImageList_ReplaceIcon)
		return pImageList_ReplaceIcon(himl, i, hicon);

	return 0;
}

HWND ri::CreateStatusWindowANSI(LONG style, LPCSTR text, HWND hwnd, UINT cid)
{
	if (pCreateStatusWindowA)
		return pCreateStatusWindowA(style, text, hwnd, cid);

	return NULL;
}
