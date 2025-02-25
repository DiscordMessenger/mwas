#pragma once

#include <wincrypt.h>

#ifdef MINGW_SPECIFIC_HACKS
typedef HCRYPTPROV HCRYPTPROV_LEGACY;
#endif

// This part of the DiscordMessenger project is designed to reimplement APIs missing
// in earlier versions of Windows, such as Windows NT 4.  It's basically like a polyfill.

// This file defines the Crypto APIs used by cpp-httplib.

namespace ri
{
	// Crypt32
	HCERTSTORE CertOpenSystemStoreA(HCRYPTPROV_LEGACY hProv, LPCSTR szSubSystemProtocol);
	BOOL CertCloseStore(HCERTSTORE hStore, DWORD dwFlags);
	BOOL CertFreeCertificateContext(PCCERT_CONTEXT pcc);
	PCCERT_CONTEXT CertFindCertificateInStore(HCERTSTORE hs, DWORD dw1, DWORD dw2, DWORD dw3, const void* p1, PCCERT_CONTEXT pccc1);
	PCCERT_CONTEXT CertEnumCertificatesInStore(HCERTSTORE hs, PCCERT_CONTEXT pcc);
}
// namespace ri