#include "aboutwin.h"

#include <commctrl.h>

#include <algorithm>

#include "app.h"
#include "l10n.h"
#include "ui.h"

namespace bfy {

namespace {

constexpr wchar_t kClass[] = L"BaifanYuAboutWindow";

enum : int {
    IDC_TITLE = 2000,
    IDC_TAGLINE,
    IDC_BODY,
    IDC_OK,
};

constexpr int kW = 420;
constexpr int kH = 320;

}  // namespace

void AboutWindow::Open(HINSTANCE hinst) {
    hinst_ = hinst;
    if (hwnd_) {
        ShowWindow(hwnd_, SW_SHOWNORMAL);
        SetForegroundWindow(hwnd_);
        return;
    }

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = &AboutWindow::WndProc;
    wc.hInstance = hinst;
    wc.lpszClassName = kClass;
    wc.hIcon = LoadIconW(hinst, MAKEINTRESOURCEW(IDI_APPICON));
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_BTNFACE + 1);
    RegisterClassExW(&wc);

    hwnd_ = CreateWindowExW(WS_EX_TOOLWINDOW, kClass, kAppName,
                            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT,
                            CW_USEDEFAULT, 100, 100, nullptr, nullptr, hinst, this);
    if (!hwnd_) return;

    BuildControls(hwnd_);
    Refresh();

    UINT dpi = GetDpiForWindow(hwnd_);
    if (dpi == 0) dpi = 96;
    const float s = static_cast<float>(dpi) / 96.0f;
    RECT want{0, 0, static_cast<LONG>(std::lround(kW * s)),
              static_cast<LONG>(std::lround(kH * s))};
    AdjustWindowRectEx(&want, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, FALSE,
                       WS_EX_TOOLWINDOW);
    RECT work{};
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &work, 0);
    const int ww = want.right - want.left, wh = want.bottom - want.top;
    const int x = work.left + ((work.right - work.left) - ww) / 2;
    const int y = work.top + ((work.bottom - work.top) - wh) / 2;
    SetWindowPos(hwnd_, HWND_TOP, x, y, ww, wh, SWP_SHOWWINDOW);
}

void AboutWindow::Close() {
    if (hwnd_) {
        DestroyWindow(hwnd_);
        hwnd_ = nullptr;
    }
}

void AboutWindow::BuildControls(HWND parent) {
    UINT dpi = GetDpiForWindow(parent);
    if (dpi == 0) dpi = 96;
    const float s = static_cast<float>(dpi) / 96.0f;
    auto px = [s](int v) { return static_cast<int>(std::lround(v * s)); };

    font_ = CreateUiFont(9);
    fontBold_ = CreateUiFont(15, true);
    fontSmall_ = CreateUiFont(9);

    HWND c = CreateWindowExW(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_LEFT, px(20),
                             px(18), px(380), px(28), parent,
                             reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_TITLE)),
                             hinst_, nullptr);
    SendMessageW(c, WM_SETFONT, reinterpret_cast<WPARAM>(fontBold_), TRUE);

    c = CreateWindowExW(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_LEFT, px(20), px(48),
                        px(380), px(20), parent,
                        reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_TAGLINE)), hinst_,
                        nullptr);
    SendMessageW(c, WM_SETFONT, reinterpret_cast<WPARAM>(font_), TRUE);

    c = CreateWindowExW(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_LEFT, px(20), px(74),
                        px(380), px(196), parent,
                        reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_BODY)), hinst_,
                        nullptr);
    SendMessageW(c, WM_SETFONT, reinterpret_cast<WPARAM>(fontSmall_), TRUE);
    text_ = c;

    c = CreateWindowExW(0, L"BUTTON", L"OK", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, px(20),
                        px(276), px(90), px(26), parent,
                        reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_OK)), hinst_,
                        nullptr);
    SendMessageW(c, WM_SETFONT, reinterpret_cast<WPARAM>(font_), TRUE);
    ok_ = c;
    SendMessageW(hwnd_, DM_SETDEFID, IDC_OK, 0);
}

void AboutWindow::Refresh() {
    if (hwnd_) BuildText();
}

void AboutWindow::BuildText() {
    L10n& loc = L10n::Get();
    SetWindowTextW(hwnd_, (L10n::Get().T(S::About) ).c_str());
    SetDlgItemTextW(hwnd_, IDC_TITLE, loc.T(S::AppName).c_str());
    SetDlgItemTextW(hwnd_, IDC_TAGLINE, loc.T(S::Tagline).c_str());
    if (ok_) SetWindowTextW(ok_, L"OK");

    std::wstring body;
    body += loc.T(S::Subtitle) + L"\r\n\r\n";
    body += loc.T(S::AboutBody) + L"\r\n\r\n";
    body += loc.T(S::VersionLabel) + L" " + kAppVersion + L"\r\n";
    body += loc.T(S::Copyright) + L"\r\n";
    body += loc.T(S::MitLicense) + L"\r\n\r\n";
    body += loc.T(S::CreditsTitle) + L"\r\n" + loc.T(S::CreditsBody) + L"\r\n\r\n";
    body += loc.T(S::LicenseTitle) + L"\r\n" + loc.T(S::LicenseBody);
    if (text_) SetWindowTextW(text_, body.c_str());
}

LRESULT CALLBACK AboutWindow::WndProc(HWND hwnd, UINT message, WPARAM wp, LPARAM lp) {
    AboutWindow* self = nullptr;
    if (message == WM_NCCREATE) {
        auto* cs = reinterpret_cast<CREATESTRUCTW*>(lp);
        self = static_cast<AboutWindow*>(cs->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
    } else {
        self = reinterpret_cast<AboutWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    }
    if (!self) return DefWindowProcW(hwnd, message, wp, lp);
    return self->Handle(hwnd, message, wp, lp);
}

LRESULT AboutWindow::Handle(HWND hwnd, UINT message, WPARAM wp, LPARAM lp) {
    switch (message) {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            hwnd_ = nullptr;
            text_ = nullptr;
            ok_ = nullptr;
            if (font_) DeleteObject(font_);
            if (fontBold_) DeleteObject(fontBold_);
            if (fontSmall_) DeleteObject(fontSmall_);
            font_ = fontBold_ = fontSmall_ = nullptr;
            return 0;
        case WM_COMMAND:
            if (LOWORD(wp) == IDC_OK) {
                DestroyWindow(hwnd);
                return 0;
            }
            return 0;
        default:
            break;
    }
    return DefWindowProcW(hwnd, message, wp, lp);
}

}  // namespace bfy
