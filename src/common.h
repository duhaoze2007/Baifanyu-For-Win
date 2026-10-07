// BaifanYu (白饭鱼) for Windows — shared includes and small helpers.
#ifndef BAIFANYU_COMMON_H
#define BAIFANYU_COMMON_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
// Windows 10: GetDpiForWindow, GetDateFormatEx, per-monitor DPI, WTS notifications.
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif
#ifndef WINVER
#define WINVER 0x0A00
#endif
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <windows.h>
#include <windowsx.h>
// objbase.h must come before objidl.h: HRESULT/GUID live there, and
// WIN32_LEAN_AND_MEAN keeps windows.h from pulling OLE in for us.
#include <objbase.h>
#include <objidl.h>
#include <commctrl.h>
#include <gdiplus.h>

#include <cmath>
#include <cstdint>
#include <random>
#include <string>
#include <vector>

#include "resource.h"

namespace bfy {

// Application identity.
extern const wchar_t* const kAppName;      // "BaifanYu"
extern const wchar_t* const kAppVersion;   // "1.0.0"
extern const wchar_t* const kRegPath;      // "Software\\BaifanYu"

// ---------------------------------------------------------------- numbers

template <typename T>
inline T Clamp(T v, T lo, T hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

inline float Lerp(float a, float b, float t) { return a + (b - a) * t; }

/// Seconds on a monotonic clock — the C++ stand-in for CACurrentMediaTime().
double NowSeconds();

/// Uniform in [lo, hi].
double RandomDouble(double lo, double hi);
int RandomInt(int lo, int hi);

// ---------------------------------------------------------------- resources

/// Read an embedded RCDATA resource into memory. Empty vector == not found.
std::vector<BYTE> LoadResourceBytes(HINSTANCE hinst, int id);

// ---------------------------------------------------------------- strings

std::wstring Format(const wchar_t* fmt, ...);

/// "C:\dir\app.exe" -> "C:\dir\app.exe"; also used for the Run key value.
std::wstring ExecutablePath();

/// True when every character is plain ASCII (windres cannot cope with
/// non-ASCII paths, so build.sh checks this too).
bool IsAsciiPath(const std::wstring& s);

}  // namespace bfy

#endif  // BAIFANYU_COMMON_H
