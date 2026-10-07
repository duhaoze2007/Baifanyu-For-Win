#include "ui.h"

namespace bfy {

const wchar_t* UiFontFamily() {
    static const wchar_t* family = [] {
        const wchar_t* candidates[] = {L"Microsoft YaHei UI", L"Microsoft YaHei",
                                       L"Segoe UI"};
        for (const wchar_t* name : candidates) {
            Gdiplus::FontFamily probe(name);
            if (probe.IsAvailable()) return name;
        }
        return L"Segoe UI";
    }();
    return family;
}

bool SystemUsesDarkMode() {
    HKEY key = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\"
                      L"Personalize",
                      0, KEY_READ, &key) != ERROR_SUCCESS) {
        return false;
    }
    DWORD value = 1, size = sizeof(value), type = 0;
    const bool ok = RegQueryValueExW(key, L"AppsUseLightTheme", nullptr, &type,
                                     reinterpret_cast<LPBYTE>(&value), &size) ==
                        ERROR_SUCCESS &&
                    type == REG_DWORD;
    RegCloseKey(key);
    return ok && value == 0;
}

HFONT CreateUiFont(int pointSize, bool bold) {
    HDC screen = GetDC(nullptr);
    const int dpi = screen ? GetDeviceCaps(screen, LOGPIXELSY) : 96;
    if (screen) ReleaseDC(nullptr, screen);
    LOGFONTW lf{};
    lf.lfHeight = -MulDiv(pointSize, dpi, 72);
    lf.lfWeight = bold ? FW_SEMIBOLD : FW_NORMAL;
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfQuality = CLEARTYPE_QUALITY;
    wcsncpy_s(lf.lfFaceName, UiFontFamily(), _TRUNCATE);
    return CreateFontIndirectW(&lf);
}

}  // namespace bfy
