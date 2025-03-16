#pragma once

#include <winsock2.h>

// This part of the DiscordMessenger project is designed to reimplement APIs missing
// in earlier versions of Windows, such as Windows NT 4.  It's basically like a polyfill.

// This file defines the Winsock2 APIs used by asio.

namespace ri
{
	// Ws2_32
	INT WSAStringToAddressA(LPSTR addressString, INT addressFamily, LPWSAPROTOCOL_INFOA protocolInfo, LPSOCKADDR address, LPINT addressLength);
	INT WSAAddressToStringA(LPSOCKADDR sockaddr, DWORD addressLength, LPWSAPROTOCOL_INFOA protocolInfo, LPSTR addressString, LPDWORD addressStringLength);
	INT WSASend(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
	INT WSASendTo(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, const sockaddr* lpTo, int iTolen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
	INT WSARecv(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpdwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
	INT WSARecvFrom(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpdwFlags, sockaddr* lpFrom, int* lpFromlen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
	SOCKET WSASocketA(int af, int type, int protocol, LPWSAPROTOCOL_INFOA lpProtocolInfo, GROUP g, DWORD dwFlags);

	bool SupportsWSARecv();
	int GetWSVersion();
}
// namespace ri
