#include "common.h"

#include <cstdarg>
#include <cstdio>

namespace bfy {

const wchar_t* const kAppName = L"BaifanYu";
const wchar_t* const kAppVersion = L"1.0.0";
const wchar_t* const kRegPath = L"Software\\BaifanYu";

double NowSeconds() {
    static LARGE_INTEGER freq = [] {
        LARGE_INTEGER f;
        QueryPerformanceFrequency(&f);
        return f;
    }();
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return static_cast<double>(now.QuadPart) / static_cast<double>(freq.QuadPart);
}

static std::mt19937& Rng() {
    static std::mt19937 engine = [] {
        std::random_device rd;
        return std::mt19937(rd() ^ static_cast<unsigned>(NowSeconds() * 1000.0));
    }();
    return engine;
}

double RandomDouble(double lo, double hi) {
    std::uniform_real_distribution<double> d(lo, hi);
    return d(Rng());
}

int RandomInt(int lo, int hi) {
    if (hi <= lo) return lo;
    std::uniform_int_distribution<int> d(lo, hi);
    return d(Rng());
}

std::vector<BYTE> LoadResourceBytes(HINSTANCE hinst, int id) {
    std::vector<BYTE> out;
    HRSRC res = FindResourceW(hinst, MAKEINTRESOURCEW(id), RT_RCDATA);
    if (!res) return out;
    HGLOBAL glob = LoadResource(hinst, res);
    if (!glob) return out;
    const DWORD size = SizeofResource(hinst, res);
    const void* data = LockResource(glob);
    if (!data || size == 0) return out;
    out.assign(static_cast<const BYTE*>(data), static_cast<const BYTE*>(data) + size);
    return out;
}

std::wstring Format(const wchar_t* fmt, ...) {
    wchar_t stack[512];
    va_list args;
    va_start(args, fmt);
    int n = _vsnwprintf_s(stack, _countof(stack), _TRUNCATE, fmt, args);
    va_end(args);
    if (n < 0) return std::wstring();
    return std::wstring(stack, static_cast<size_t>(n));
}

std::wstring ExecutablePath() {
    std::wstring buf(MAX_PATH, L'\0');
    for (;;) {
        DWORD n = GetModuleFileNameW(nullptr, buf.data(), static_cast<DWORD>(buf.size()));
        if (n == 0) return std::wstring();
        if (n < buf.size() - 1) {
            buf.resize(n);
            return buf;
        }
        buf.resize(buf.size() * 2);
    }
}

bool IsAsciiPath(const std::wstring& s) {
    for (wchar_t c : s) {
        if (c > 0x7f) return false;
    }
    return true;
}

}  // namespace bfy
