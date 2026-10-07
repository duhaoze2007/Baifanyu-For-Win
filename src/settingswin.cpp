#include "settingswin.h"

#include <commctrl.h>

#include <algorithm>

#include "app.h"
#include "l10n.h"
#include "sound.h"
#include "ui.h"

namespace bfy {

namespace {

constexpr wchar_t kClass[] = L"BaifanYuSettingsWindow";

enum : int {
    IDC_TITLE = 1000,
    IDC_TAGLINE,
    IDC_SEC_SIZE,
    IDC_HINT_SIZE,
    IDC_HOVER_LABEL,
    IDC_HOVER_SLIDER,
    IDC_PERCH_LABEL,
    IDC_PERCH_SLIDER,
    IDC_AMP_LABEL,
    IDC_AMP_SLIDER,
    IDC_SEC_BEHAVIOUR,
    IDC_WANDER,
    IDC_HINT_WANDER,
    IDC_RANDOMFACE,
    IDC_SOUND,
    IDC_SEC_SKIN,
    IDC_HINT_SKIN,
    IDC_SKIN0,
    IDC_SKIN1,
    IDC_SEC_STATUS,
    IDC_STATUS,
    IDC_FACES,
    IDC_SHOW,
    IDC_HIDE,
    IDC_TESTFACE,
    IDC_SEC_LANG,
    IDC_LANG,
    IDC_SEC_HOWTO,
    IDC_BODY_HOWTO,
    IDC_SEC_NOTES,
    IDC_BODY_NOTES,
};

constexpr int kW = 520;  // logical client size
constexpr int kH = 410;

HWND MakeStatic(HWND parent, int id, const wchar_t* text, int x, int y, int w, int h) {
    return CreateWindowExW(0, L"STATIC", text, WS_CHILD | WS_VISIBLE | SS_LEFT, x, y, w, h,
                           parent, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
                           nullptr, nullptr);
}

}  // namespace

float SettingsWindow::Scale() const { return static_cast<float>(dpi_) / 96.0f; }

void SettingsWindow::Open(HINSTANCE hinst) {
    hinst_ = hinst;
    if (hwnd_) {
        ShowWindow(hwnd_, SW_SHOWNORMAL);
        SetForegroundWindow(hwnd_);
        Refresh();
        return;
    }

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = &SettingsWindow::WndProc;
    wc.hInstance = hinst;
    wc.lpszClassName = kClass;
    wc.hIcon = LoadIconW(hinst, MAKEINTRESOURCEW(IDI_APPICON));
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_BTNFACE + 1);
    RegisterClassExW(&wc);

    hwnd_ = CreateWindowExW(WS_EX_TOOLWINDOW, kClass, kAppName,
                            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                            CW_USEDEFAULT, CW_USEDEFAULT, 100, 100, nullptr, nullptr,
                            hinst, this);
    if (!hwnd_) return;

    dpi_ = GetDpiForWindow(hwnd_);
    if (dpi_ == 0) dpi_ = 96;
    font_ = CreateUiFont(9);
    fontBold_ = CreateUiFont(10, true);
    fontSmall_ = CreateUiFont(8);

    BuildControls(hwnd_);
    ApplyTexts();
    SyncFromState();

    // Size to the scaled client area and centre it on the work area.
    const int cw = std::lround(kW * Scale());
    const int ch = std::lround(kH * Scale());
    RECT want{0, 0, cw, ch};
    AdjustWindowRectEx(&want, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                       FALSE, WS_EX_TOOLWINDOW);
    const int ww = want.right - want.left, wh = want.bottom - want.top;
    RECT work{};
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &work, 0);
    const int x = work.left + ((work.right - work.left) - ww) / 2;
    const int y = work.top + ((work.bottom - work.top) - wh) / 2;
    SetWindowPos(hwnd_, HWND_TOP, x, y, ww, wh, SWP_SHOWWINDOW);
}

void SettingsWindow::Close() {
    if (hwnd_) {
        DestroyWindow(hwnd_);
        hwnd_ = nullptr;
    }
}

void SettingsWindow::BuildControls(HWND parent) {
    const float s = Scale();
    auto px = [s](int v) { return static_cast<int>(std::lround(v * s)); };
    auto rect = [&](int x, int y, int w, int h) {
        return RECT{px(x), px(y), px(x + w), px(y + h)};
    };
    auto place = [&](HWND c, int x, int y, int w, int h) {
        RECT r = rect(x, y, w, h);
        SetWindowPos(c, nullptr, r.left, r.top, r.right - r.left, r.bottom - r.top,
                     SWP_NOZORDER);
    };

    const int lx = 16, lw = 236;
    const int rx = 268, rw = 236;

    // ---------------------------------------------------------- left column
    HWND c;
    c = MakeStatic(parent, IDC_SEC_SIZE, L"", px(lx), px(12), px(lw), px(18));
    SendMessageW(c, WM_SETFONT, reinterpret_cast<WPARAM>(fontBold_), TRUE);

    c = MakeStatic(parent, IDC_HINT_SIZE, L"", px(lx), px(32), px(lw), px(30));
    SendMessageW(c, WM_SETFONT, reinterpret_cast<WPARAM>(fontSmall_), TRUE);

    c = MakeStatic(parent, IDC_HOVER_LABEL, L"", px(lx), px(66), px(lw), px(16));
    SendMessageW(c, WM_SETFONT, reinterpret_cast<WPARAM>(font_), TRUE);
    c = CreateWindowExW(0, TRACKBAR_CLASSW, L"",
                        WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_NOTICKS, 0, 0, 10, 10,
                        parent, reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_HOVER_SLIDER)),
                        hinst_, nullptr);
    place(c, lx, 84, lw, 20);
    SendMessageW(c, TBM_SETRANGE, TRUE, MAKELONG(120, 480));
    SendMessageW(c, TBM_SETPAGESIZE, 0, 20);

    c = MakeStatic(parent, IDC_PERCH_LABEL, L"", px(lx), px(108), px(lw), px(16));
    SendMessageW(c, WM_SETFONT, reinterpret_cast<WPARAM>(font_), TRUE);
    c = CreateWindowExW(0, TRACKBAR_CLASSW, L"",
                        WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_NOTICKS, 0, 0, 10, 10,
                        parent, reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_PERCH_SLIDER)),
                        hinst_, nullptr);
    place(c, lx, 126, lw, 20);
    SendMessageW(c, TBM_SETRANGE, TRUE, MAKELONG(56, 160));
    SendMessageW(c, TBM_SETPAGESIZE, 0, 8);

    c = MakeStatic(parent, IDC_AMP_LABEL, L"", px(lx), px(150), px(lw), px(16));
    SendMessageW(c, WM_SETFONT, reinterpret_cast<WPARAM>(font_), TRUE);
    c = CreateWindowExW(0, TRACKBAR_CLASSW, L"",
                        WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_NOTICKS, 0, 0, 10, 10,
                        parent, reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_AMP_SLIDER)),
                        hinst_, nullptr);
    place(c, lx, 168, lw, 20);
    SendMessageW(c, TBM_SETRANGE, TRUE, MAKELONG(30, 150));
    SendMessageW(c, TBM_SETPAGESIZE, 0, 10);

    c = MakeStatic(parent, IDC_SEC_BEHAVIOUR, L"", px(lx), px(198), px(lw), px(18));
    SendMessageW(c, WM_SETFONT, reinterpret_cast<WPARAM>(fontBold_), TRUE);

    c = CreateWindowExW(0, L"BUTTON", L"", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 0, 0,
                        10, 10, parent,
                        reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_WANDER)), hinst_,
                        nullptr);
    place(c, lx, 218, lw, 20);
    SendMessageW(c, WM_SETFONT, reinterpret_cast<WPARAM>(font_), TRUE);

    c = MakeStatic(parent, IDC_HINT_WANDER, L"", px(lx + 18), px(238), px(lw - 18), px(32));
    SendMessageW(c, WM_SETFONT, reinterpret_cast<WPARAM>(fontSmall_), TRUE);

    c = CreateWindowExW(0, L"BUTTON", L"", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 0, 0,
                        10, 10, parent,
                        reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_RANDOMFACE)),
                        hinst_, nullptr);
    place(c, lx, 272, lw, 20);
    SendMessageW(c, WM_SETFONT, reinterpret_cast<WPARAM>(font_), TRUE);

    c = CreateWindowExW(0, L"BUTTON", L"", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 0, 0,
                        10, 10, parent,
                        reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_SOUND)), hinst_,
                        nullptr);
    place(c, lx, 294, lw, 20);
    SendMessageW(c, WM_SETFONT, reinterpret_cast<WPARAM>(font_), TRUE);

    c = MakeStatic(parent, IDC_SEC_SKIN, L"", px(lx), px(322), px(lw), px(18));
    SendMessageW(c, WM_SETFONT, reinterpret_cast<WPARAM>(fontBold_), TRUE);

    c = MakeStatic(parent, IDC_HINT_SKIN, L"", px(lx), px(342), px(lw), px(16));
    SendMessageW(c, WM_SETFONT, reinterpret_cast<WPARAM>(fontSmall_), TRUE);

    c = CreateWindowExW(0, L"BUTTON", L"",
                        WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP, 0, 0, 10, 10,
                        parent, reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_SKIN0)),
                        hinst_, nullptr);
    place(c, lx, 362, lw, 20);
    SendMessageW(c, WM_SETFONT, reinterpret_cast<WPARAM>(font_), TRUE);

    c = CreateWindowExW(0, L"BUTTON", L"", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 0, 0,
                        10, 10, parent,
                        reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_SKIN1)), hinst_,
                        nullptr);
    place(c, lx, 384, lw, 20);
    SendMessageW(c, WM_SETFONT, reinterpret_cast<WPARAM>(font_), TRUE);

    // ---------------------------------------------------------- right column
    c = MakeStatic(parent, IDC_SEC_STATUS, L"", px(rx), px(12), px(rw), px(18));
    SendMessageW(c, WM_SETFONT, reinterpret_cast<WPARAM>(fontBold_), TRUE);

    c = MakeStatic(parent, IDC_STATUS, L"", px(rx), px(32), px(rw), px(34));
    SendMessageW(c, WM_SETFONT, reinterpret_cast<WPARAM>(font_), TRUE);

    c = MakeStatic(parent, IDC_FACES, L"", px(rx), px(70), px(rw), px(16));
    SendMessageW(c, WM_SETFONT, reinterpret_cast<WPARAM>(fontSmall_), TRUE);

    const int bw = 76, gap = 4;
    c = CreateWindowExW(0, L"BUTTON", L"", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0, 0, 10,
                        10, parent, reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_SHOW)),
                        hinst_, nullptr);
    place(c, rx, 92, bw, 26);
    SendMessageW(c, WM_SETFONT, reinterpret_cast<WPARAM>(font_), TRUE);

    c = CreateWindowExW(0, L"BUTTON", L"", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0, 0, 10,
                        10, parent, reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_HIDE)),
                        hinst_, nullptr);
    place(c, rx + bw + gap, 92, bw, 26);
    SendMessageW(c, WM_SETFONT, reinterpret_cast<WPARAM>(font_), TRUE);

    c = CreateWindowExW(0, L"BUTTON", L"", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0, 0, 10,
                        10, parent,
                        reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_TESTFACE)), hinst_,
                        nullptr);
    place(c, rx + (bw + gap) * 2, 92, bw, 26);
    SendMessageW(c, WM_SETFONT, reinterpret_cast<WPARAM>(font_), TRUE);

    c = MakeStatic(parent, IDC_SEC_LANG, L"", px(rx), px(130), px(rw), px(18));
    SendMessageW(c, WM_SETFONT, reinterpret_cast<WPARAM>(fontBold_), TRUE);

    c = CreateWindowExW(0, L"COMBOBOX", L"",
                        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL, 0, 0, 10,
                        200, parent, reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_LANG)),
                        hinst_, nullptr);
    place(c, rx, 148, rw, 200);
    SendMessageW(c, WM_SETFONT, reinterpret_cast<WPARAM>(font_), TRUE);

    c = MakeStatic(parent, IDC_SEC_HOWTO, L"", px(rx), px(184), px(rw), px(18));
    SendMessageW(c, WM_SETFONT, reinterpret_cast<WPARAM>(fontBold_), TRUE);

    c = MakeStatic(parent, IDC_BODY_HOWTO, L"", px(rx), px(202), px(rw), px(112));
    SendMessageW(c, WM_SETFONT, reinterpret_cast<WPARAM>(fontSmall_), TRUE);

    c = MakeStatic(parent, IDC_SEC_NOTES, L"", px(rx), px(322), px(rw), px(18));
    SendMessageW(c, WM_SETFONT, reinterpret_cast<WPARAM>(fontBold_), TRUE);

    c = MakeStatic(parent, IDC_BODY_NOTES, L"", px(rx), px(340), px(rw), px(64));
    SendMessageW(c, WM_SETFONT, reinterpret_cast<WPARAM>(fontSmall_), TRUE);

    (void)gap;
}

void SettingsWindow::ApplyTexts() {
    L10n& loc = L10n::Get();
    auto set = [this](int id, const std::wstring& text) {
        SetDlgItemTextW(hwnd_, id, text.c_str());
    };
    set(IDC_SEC_SIZE, loc.T(S::SizeSection));
    set(IDC_HINT_SIZE, loc.T(S::SizeHint));
    set(IDC_SEC_BEHAVIOUR, loc.T(S::BehaviourSection));
    set(IDC_WANDER, loc.T(S::WanderTitle));
    set(IDC_HINT_WANDER, loc.T(S::WanderHint));
    set(IDC_RANDOMFACE, loc.T(S::RandomFaceTitle));
    set(IDC_SOUND, loc.T(S::SoundTitle));
    set(IDC_SEC_SKIN, loc.T(S::SkinSection));
    set(IDC_HINT_SKIN, loc.T(S::SkinHint) + L"  " + loc.T(S::TapToSwitch));
    set(IDC_SKIN0, loc.T(SkinAt(0).nameKey));
    set(IDC_SKIN1, loc.T(SkinAt(1).nameKey));
    set(IDC_SEC_STATUS, loc.T(S::StatusSection));
    set(IDC_SHOW, loc.T(S::ShowButton));
    set(IDC_HIDE, loc.T(S::HideButton));
    set(IDC_TESTFACE, loc.T(S::TestFaceButton));
    set(IDC_SEC_LANG, loc.T(S::Language));
    set(IDC_SEC_HOWTO, loc.T(S::HowToTitle));
    set(IDC_BODY_HOWTO, loc.T(S::HowToBody));
    set(IDC_SEC_NOTES, loc.T(S::NotesTitle));
    set(IDC_BODY_NOTES, loc.T(S::NotesBody));

    SetWindowTextW(hwnd_, loc.T(S::AppName).c_str());

    // Language combo: Follow System first, then the three explicit choices.
    HWND combo = GetDlgItem(hwnd_, IDC_LANG);
    const int keep = static_cast<int>(SendMessageW(combo, CB_GETCURSEL, 0, 0));
    SendMessageW(combo, CB_RESETCONTENT, 0, 0);
    const std::wstring follow =
        loc.T(S::FollowSystem) + L" (" + LanguageDisplayName(DetectSystemLanguage()) + L")";
    SendMessageW(combo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(follow.c_str()));
    SendMessageW(combo, CB_ADDSTRING, 0,
                 reinterpret_cast<LPARAM>(LanguageDisplayName(Lang::En).c_str()));
    SendMessageW(combo, CB_ADDSTRING, 0,
                 reinterpret_cast<LPARAM>(LanguageDisplayName(Lang::ZhHans).c_str()));
    SendMessageW(combo, CB_ADDSTRING, 0,
                 reinterpret_cast<LPARAM>(LanguageDisplayName(Lang::ZhHant).c_str()));
    (void)keep;
}

void SettingsWindow::SyncFromState() {
    App& app = App::Get();
    Settings& st = app.settings();

    SendDlgItemMessageW(hwnd_, IDC_HOVER_SLIDER, TBM_SETPOS, TRUE,
                        static_cast<LPARAM>(std::lround(st.hoverHeight)));
    SendDlgItemMessageW(hwnd_, IDC_PERCH_SLIDER, TBM_SETPOS, TRUE,
                        static_cast<LPARAM>(std::lround(st.perchWidth)));
    SendDlgItemMessageW(hwnd_, IDC_AMP_SLIDER, TBM_SETPOS, TRUE,
                        static_cast<LPARAM>(std::lround(st.amplitude * 100.0)));

    L10n& loc = L10n::Get();
    SetDlgItemTextW(hwnd_, IDC_HOVER_LABEL,
                    Format(L"%s   %d", loc.T(S::HoverSize).c_str(),
                           std::lround(st.hoverHeight)).c_str());
    SetDlgItemTextW(hwnd_, IDC_PERCH_LABEL,
                    Format(L"%s   %d", loc.T(S::PerchWidthLabel).c_str(),
                           std::lround(st.perchWidth)).c_str());
    SetDlgItemTextW(hwnd_, IDC_AMP_LABEL,
                    Format(L"%s   %d%%", loc.T(S::AmplitudeLabel).c_str(),
                           std::lround(st.amplitude * 100.0)).c_str());

    CheckDlgButton(hwnd_, IDC_WANDER, st.wanderOn ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hwnd_, IDC_RANDOMFACE, st.randomFace ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hwnd_, IDC_SOUND, st.soundOn ? BST_CHECKED : BST_UNCHECKED);

    CheckRadioButton(hwnd_, IDC_SKIN0, IDC_SKIN1,
                     st.skinIndex == 0 ? IDC_SKIN0 : IDC_SKIN1);

    SetDlgItemTextW(hwnd_, IDC_STATUS, app.PetStateText().c_str());
    SetDlgItemTextW(hwnd_, IDC_FACES, loc.FacesLoaded(app.LoadedExpressionCount()).c_str());

    EnableWindow(GetDlgItem(hwnd_, IDC_SHOW), !app.PetRunning());
    EnableWindow(GetDlgItem(hwnd_, IDC_HIDE), app.PetRunning());

    HWND combo = GetDlgItem(hwnd_, IDC_LANG);
    int index = 0;
    switch (loc.language()) {
        case Lang::En: index = 1; break;
        case Lang::ZhHans: index = 2; break;
        case Lang::ZhHant: index = 3; break;
        case Lang::System:
        default: index = 0; break;
    }
    SendMessageW(combo, CB_SETCURSEL, index, 0);
}

void SettingsWindow::Refresh() {
    if (!hwnd_) return;
    ApplyTexts();
    SyncFromState();
    InvalidateRect(hwnd_, nullptr, TRUE);
}

LRESULT CALLBACK SettingsWindow::WndProc(HWND hwnd, UINT message, WPARAM wp, LPARAM lp) {
    SettingsWindow* self = nullptr;
    if (message == WM_NCCREATE) {
        auto* cs = reinterpret_cast<CREATESTRUCTW*>(lp);
        self = static_cast<SettingsWindow*>(cs->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
    } else {
        self = reinterpret_cast<SettingsWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    }
    if (!self) return DefWindowProcW(hwnd, message, wp, lp);
    return self->Handle(hwnd, message, wp, lp);
}

LRESULT SettingsWindow::Handle(HWND hwnd, UINT message, WPARAM wp, LPARAM lp) {
    App& app = App::Get();
    Settings& st = app.settings();

    switch (message) {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            hwnd_ = nullptr;
            if (font_) DeleteObject(font_);
            if (fontBold_) DeleteObject(fontBold_);
            if (fontSmall_) DeleteObject(fontSmall_);
            font_ = fontBold_ = fontSmall_ = nullptr;
            return 0;

        case WM_HSCROLL: {
            HWND control = reinterpret_cast<HWND>(lp);
            const int id = GetDlgCtrlID(control);
            if (id == IDC_HOVER_SLIDER || id == IDC_PERCH_SLIDER || id == IDC_AMP_SLIDER) {
                const int value = static_cast<int>(SendMessageW(control, TBM_GETPOS, 0, 0));
                if (id == IDC_HOVER_SLIDER) st.hoverHeight = value;
                if (id == IDC_PERCH_SLIDER) st.perchWidth = value;
                if (id == IDC_AMP_SLIDER) st.amplitude = value / 100.0;
                SyncFromState();
                app.ApplySizeChange();
                return 0;
            }
            return 0;
        }

        case WM_COMMAND: {
            const int id = LOWORD(wp);
            const int notification = HIWORD(wp);
            switch (id) {
                case IDC_WANDER:
                    st.wanderOn = IsDlgButtonChecked(hwnd, IDC_WANDER) == BST_CHECKED;
                    app.ApplyBehaviourChange();
                    return 0;
                case IDC_RANDOMFACE:
                    st.randomFace = IsDlgButtonChecked(hwnd, IDC_RANDOMFACE) == BST_CHECKED;
                    app.ApplyBehaviourChange();
                    return 0;
                case IDC_SOUND:
                    st.soundOn = IsDlgButtonChecked(hwnd, IDC_SOUND) == BST_CHECKED;
                    app.ApplyBehaviourChange();
                    return 0;
                case IDC_SKIN0:
                    app.SelectSkin(0);
                    return 0;
                case IDC_SKIN1:
                    app.SelectSkin(1);
                    return 0;
                case IDC_SHOW:
                    app.ShowPet();
                    SyncFromState();
                    return 0;
                case IDC_HIDE:
                    app.HidePet();
                    SyncFromState();
                    return 0;
                case IDC_TESTFACE:
                    app.TestFace();
                    return 0;
                case IDC_LANG:
                    if (notification == CBN_SELCHANGE) {
                        const int sel = static_cast<int>(
                            SendDlgItemMessageW(hwnd, IDC_LANG, CB_GETCURSEL, 0, 0));
                        Lang lang = Lang::System;
                        if (sel == 1) lang = Lang::En;
                        if (sel == 2) lang = Lang::ZhHans;
                        if (sel == 3) lang = Lang::ZhHant;
                        L10n::Get().SetLanguage(lang);
                        app.OnLanguageChanged();
                        Refresh();
                    }
                    return 0;
                default:
                    break;
            }
            return 0;
        }

        default:
            break;
    }
    return DefWindowProcW(hwnd, message, wp, lp);
}

}  // namespace bfy
