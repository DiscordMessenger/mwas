# Modern Windows API Shim (MWAS)

This is the repository for Discord Messenger's modern Windows API shim.  Its aim is to implement a
shim for certain APIs that were implemented in versions earlier than Discord Messenger targets.

It performs its duty by:

- Using the actual procedures, when available, or

- Providing an (admittedly maybe crappy, but passable) implementation of the function, if Windows
  did not provide us with its own version.

## Usage

To use, simply embed the `src/*.cpp` and `include/ri/*.hpp` files in your project.  Then you will
simply add `ri::` to all uses of modern Windows APIs that are implemented by this project and call
`ri::InitReimplementation()` before any of the code that uses it.

## Functions shimmed by this compatibility layer

This is a list of functions shimmed, along with their implementation if Windows doesn't provide one.
This list hasn't been updated in a while and is probably incomplete.

- `GetFileSizeEx` - Implemented using `GetFileSize`.

- `SetFilePointerEx` - Copied from
  https://github.com/enlyze/EnlyzeWinCompatLib/blob/master/src/winnt_40.cpp#L43

- `VerSetConditionMask` - Copied from
  [ReactOS' implementation](https://github.com/mirror/reactos/blob/master/reactos/lib/rtl/version.c#L213)

- `VerifyVersionInfo` - Implemented using `GetVersionEx`.

- `RegisterWaitForSingleObject` - Returns `INVALID_HANDLE_VALUE` through `phNewWaitObject` and
  `FALSE` to the caller. Not implemented.

- `UnregisterWaitEx` - Not implemented, returns `FALSE`.

- `GradientFill` - Not implemented, returns `FALSE`.

- `AlphaBlend` - Not implemented, simply calls `StretchBlt`.

- `TransparentBlt` - Not implemented, simply calls `StretchBlt`.

- `SHGetFolderPath` - Not implemented, just returns `"."`.

- `PathFileExists` - Implemented using `GetFileAttributes`.

- `GetMenuInfo` - Not implemented, returns `FALSE`.

- `SetMenuInfo` - Not implemented, returns `FALSE`.

- `GetGestureInfo` - Not implemented, returns `FALSE`.

- `SetDCBrushColor` - Creates a new brush unless the color is the same. You must use
  `ri::GetDCBrush()` instead of `GetStockObject()` to get this brush. If `SetDCBrushColor` 
  is imported, then `ri::GetDCBrush()` redirects to `GetStockObject(DC_BRUSH)`.

- `SetDCPenColor` - Creates a new pen unless the color is the same. You must use
  `ri::GetDCPen()` instead of `GetStockObject()` to get this pen. If `SetDCPenColor`
  is imported, then `ri::GetDCPen()` redirects to `GetStockObject(DC_PEN)`.

- `MonitorFromPoint` - Returns a fake magic value to be passed into `GetMonitorInfo`.

- `GetMonitorInfo` - Simulates fetching information about the primary monitor using `GetSystemMetrics`.

- `AnimateWindow` - Hides or shows the window based on the flags parameter. Animation related flags are ignored.

- `InitializeCriticalSectionAndSpinCount` - Initializes a critical section. Ignores the spin count.

- `TryEnterCriticalSection` - Not implemented, returns `FALSE`.

- `GetVersionEx` - Assumes a Windows 9X target (`WIN32_WINDOWS`) and `szCSDVersion` returns
  `"Unknown Windows"`, but the major, minor, and build numbers are pulled from `GetVersion()`.

- `QueueUserAPC` - Not implemented.  Used by asio to wake up sleeping threads.

- `SHGetFileInfo` - Not implemented.  Clears the `hIcon` member and returns 0.

- `Shell_NotifyIcon` - Not implemented.  Returns 0.

- `TrackMouseEvent` - Not implemented. Returns 0.

- `DrawEdge` - Barely implemented. Draws a single solid black rectangle.

- `DrawIconEx` - Barely implemented. Simply calls `DrawIcon` on the icon with no regard to sizing.

- `GetScrollInfo` - Emulated using `GetScrollPos` and `GetScrollRange` to the best of its ability.

- `SetScrollInfo` - Emulated using `SetScrollPos` and `SetScrollRange` to the best of its ability.

- `GetSysColorBrush` - Uses `GetSysColor` and creates solid brushes on demand.

- `LoadImage` - Unimplemented. Returns `NULL`.

- `CertOpenSystemStoreA` - Unimplemented. Returns `NULL`.

- `CertCloseStore` - Unimplemented. Returns `FALSE`.

- `CertFindCertificateInStore` - Unimplemented. Returns `NULL`.

- `CertFreeCertificateContext` - Unimplemented. Returns `FALSE`.

- `CertEnumCertificatesInStore` - Unimplemented. Returns `NULL`.

- `CoInitialize` - Unimplemented. Returns `0`.

## License

This project is licensed under the MIT license.  See the license file for details.
