# Modern Windows API Shim (MWAS)

This is the repository for Discord Messenger's modern Windows API shim.  Its aim is to implement a
shim for certain APIs that were implemented in versions earlier than Discord Messenger targets.

It performs its duty by:

- Using the actual procedures, when available, or

- Providing an (admittedly maybe crappy, but passable) implementation of the function, if Windows
  did not provide us with its own version.

## Usage

To use, simply embed the `reimpl.cpp` and `ri/reimpl.hpp` files in your project.  Then you will
simply add `ri::` to all uses of modern Windows APIs that are implemented by this project and call
`ri::InitReimplementation()` before any of the code that uses it.

## Functions shimmed by this compatibility layer

This is a list of functions shimmed, along with their implementation if Windows doesn't provide one.

- `GetFileSizeEx` - Implemented using `GetFileSize`.

- `SetFilePointerEx` - Copied from
  https://github.com/enlyze/EnlyzeWinCompatLib/blob/master/src/winnt_40.cpp#L43

- `VerSetConditionMask` - Copied from
  [ReactOS' implementation](https://github.com/mirror/reactos/blob/master/reactos/lib/rtl/version.c#L213)

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

- `SetDCBrushColor` - Not implemented, returns `0`.

- `SetDCPenColor` - Not implemented, returns `0`.

- `MonitorFromPoint` - Returns a fake magic value to be passed into `GetMonitorInfo`.

- `GetMonitorInfo` - Simulates fetching information about the primary monitor using `GetSystemMetrics`.

## License

This project is licensed under the MIT license.  See the license file for details.
