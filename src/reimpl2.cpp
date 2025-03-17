//
// Now, these are actually meant to be secret "hidden" polyfills.
// We do these this way because MinGW uses these and I'm too lazy
// to recompile MinGW to, you know, not.
//
// We do the other polyfills in a non hidden way because I don't
// care to refactor anything.
//
#include <windows.h>
#define DLLEXPORT __attribute__((dllexport))

#define TYPEDEF_IMPORT(returnType, ...)             \
	typedef returnType(WINAPI* pfunc)(__VA_ARGS__); \
	static pfunc pf;                                \
	static bool pfi;

#define CHECK_IMPORT(function, module, ...) do {          \
	if (!pfi) {                                           \
		pfi = 1;                                          \
		HMODULE hmod = GetModuleHandleA(module);          \
		if (hmod)                                         \
			pf = (pfunc) GetProcAddress(hmod, #function); \
	}                                                     \
	if (pf)                                               \
		return pf(__VA_ARGS__);                           \
} while (0)

extern "C"
{

BOOL DLLEXPORT WINAPI IsDBCSLeadByteEx(UINT CodePage, BYTE TestChar)
{
	TYPEDEF_IMPORT(BOOL, UINT, BYTE);
	CHECK_IMPORT(IsDBCSLeadByteEx, "kernel32.dll", CodePage, TestChar);
	
	// implementation here
	
	return FALSE;
}

}
