#include "app.h"

#include <objbase.h>
#include <shellapi.h>
#include <wtsapi32.h>

#include <algorithm>

#include "sound.h"
#include "ui.h"

namespace bfy {

namespace {

constexpr wchar_t kMainClass[] = L"BaifanYuMainWindow";
constexpr wchar_t kMutexName[] = L"BaifanYu.DesktopPet.SingleInstance";

constexpr UINT kFrameTimer = 1;
constexpr UINT kWatchdogTimer = 2;
constexpr UINT kFrameIntervalMs = 16;

constexpr UINT WM_TRAYICON = WM_APP + 1;
constexpr UINT WM_SHOW_PET = App::kMsgShowPet;

// Tray / context menu commands.
constexpr int IDM_TOGGLE = 40001;
constexpr int IDM_NEXTFACE = 40002;
constexpr int IDM_WANDER = 40003;
constexpr int IDM_RANDOMFACE = 40004;
constexpr int IDM_SOUND = 40005;
constexpr int IDM_AUTOSTART = 40006;
constexpr int IDM_SETTINGS = 40007;
constexpr int IDM_ABOUT = 40008;
constexpr int IDM_QUIT = 40009;
constexpr int IDM_SKIN_BASE = 40100;

constexpr int kTrayIconId = 1;

/// {6FE69556-704A-47A0-8F24-C28D936FDA47} = GUID_CONSOLE_DISPLAY_STATE.
const GUID kConsoleDisplayState = {
    0x6fe69556, 0x704a, 0x47a0, {0x8f, 0x24, 0xc2, 0x8d, 0x93, 0x6f, 0xda, 0x47}};

ULONG_PTR g_gdiplusToken = 0;

bool operator==(const RECT& a, const RECT& b) {
    return a.left == b.left && a.top == b.top && a.right == b.right && a.bottom == b.bottom;
}
bool operator!=(const RECT& a, const RECT& b) { return !(a == b); }

}  // namespace

App& App::Get() {
    static App inst;
    return inst;
}

// ---------------------------------------------------------------- startup

App::StartResult App::Init(HINSTANCE hinst) {
    hinst_ = hinst;
    taskbarCreatedMsg_ = RegisterWindowMessageW(L"TaskbarCreated");

    mutex_ = CreateMutexW(nullptr, TRUE, kMutexName);
    if (mutex_ && GetLastError() == ERROR_ALREADY_EXISTS) {
        // She is already up: ask that copy to bring her out and step aside.
        HWND existing = FindWindowW(kMainClass, nullptr);
        if (existing) PostMessageW(existing, WM_SHOW_PET, 0, 0);
        CloseHandle(mutex_);
        mutex_ = nullptr;
        return StartResult::AlreadyRunning;
    }
    ownsMutex_ = true;

    Gdiplus::GdiplusStartupInput input;
    Gdiplus::GdiplusStartup(&g_gdiplusToken, &input, nullptr);
    OleInitialize(nullptr);

    ArtworkStore::Get().Init(hinst_);
    DuckSound::Get().Init(hinst_);

    settings_.Load();
    L10n::Get().SetLanguage(settings_.language);

    if (!CreateMainWindow()) return StartResult::Declined;

    if (!settings_.firstRunDone) {
        RunFirstLaunchDialogs();
        if (!settings_.firstRunDone) return StartResult::Declined;
    }

    if (!bubble_.Create(hinst_)) return StartResult::Declined;

    trayIcon_ = static_cast<HICON>(LoadImageW(
        hinst_, MAKEINTRESOURCEW(IDI_APPICON), IMAGE_ICON,
        GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR));
    if (!trayIcon_) trayIcon_ = LoadIconW(hinst_, MAKEINTRESOURCEW(IDI_APPICON));
    AddTrayIcon();

    // Stop rendering when nobody can see her: screen off / session locked.
    powerNotify_ = RegisterPowerSettingNotification(mainWnd_, &kConsoleDisplayState,
                                                    DEVICE_NOTIFY_WINDOW_HANDLE);
    sessionNotify_ = WTSRegisterSessionNotification(mainWnd_, NOTIFY_FOR_THIS_SESSION);

    SetTimer(mainWnd_, kWatchdogTimer, 1000, nullptr);

    if (settings_.running) {
        ShowPet();
    } else {
        running_ = false;
        SetAnimating(false);
    }
    return StartResult::Ok;
}

void App::RunFirstLaunchDialogs() {
    const std::wstring name = L10n::Get().T(S::AppName);
    const int copyright = MessageBoxW(
        nullptr, L10n::Get().T(S::FirstRunCopyrightBody).c_str(),
        (L10n::Get().T(S::FirstRunCopyrightTitle) + L" — " + name).c_str(),
        MB_YESNO | MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TOPMOST);
    if (copyright != IDYES) return;
    const int privacy = MessageBoxW(
        nullptr, L10n::Get().T(S::FirstRunPrivacyBody).c_str(),
        (L10n::Get().T(S::FirstRunPrivacyTitle) + L" — " + name).c_str(),
        MB_YESNO | MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TOPMOST);
    if (privacy != IDYES) return;
    settings_.firstRunDone = true;
    settings_.Save();
}

bool App::CreateMainWindow() {
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = &App::MainProc;
    wc.hInstance = hinst_;
    wc.lpszClassName = kMainClass;
    wc.hIcon = LoadIconW(hinst_, MAKEINTRESOURCEW(IDI_APPICON));
    RegisterClassExW(&wc);

    mainWnd_ = CreateWindowExW(0, kMainClass, kAppName, WS_POPUP, 0, 0, 0, 0, nullptr,
                               nullptr, hinst_, this);
    return mainWnd_ != nullptr;
}

LRESULT CALLBACK App::MainProc(HWND hwnd, UINT message, WPARAM wp, LPARAM lp) {
    App* self = nullptr;
    if (message == WM_NCCREATE) {
        auto* cs = reinterpret_cast<CREATESTRUCTW*>(lp);
        self = static_cast<App*>(cs->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
    } else {
        self = reinterpret_cast<App*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    }
    if (!self) return DefWindowProcW(hwnd, message, wp, lp);
    return self->HandleMain(hwnd, message, wp, lp);
}

LRESULT App::HandleMain(HWND hwnd, UINT message, WPARAM wp, LPARAM lp) {
    if (message == taskbarCreatedMsg_ && taskbarCreatedMsg_ != 0) {
        AddTrayIcon();  // Explorer restarted: put her face back
        return 0;
    }

    switch (message) {
        case WM_TRAYICON:
            switch (LOWORD(lp)) {
                case WM_LBUTTONUP:
                case WM_RBUTTONUP: {
                    POINT pt;
                    GetCursorPos(&pt);
                    ShowTrayMenu(pt);
                    return 0;
                }
                case WM_LBUTTONDBLCLK:
                    TogglePet();
                    return 0;
                default:
                    break;
            }
            return 0;

        case WM_SHOW_PET:
            ShowPet();
            return 0;

        case App::kMsgOpenSettings:
            OpenSettings();
            return 0;

        case App::kMsgOpenAbout:
            OpenAbout();
            return 0;

        case WM_TIMER:
            if (wp == kFrameTimer) {
                OnFrame();
            } else if (wp == kWatchdogTimer) {
                OnWatchdog();
            }
            return 0;

        case WM_POWERBROADCAST:
            if (wp == PBT_POWERSETTINGCHANGE && lp) {
                auto* setting = reinterpret_cast<POWERBROADCAST_SETTING*>(lp);
                if (IsEqualGUID(setting->PowerSetting, kConsoleDisplayState) &&
                    setting->DataLength >= sizeof(DWORD)) {
                    displayOff_ = *reinterpret_cast<DWORD*>(setting->Data) == 0;
                    if (displayOff_) HideBubble();
                }
                return TRUE;
            }
            if (wp == PBT_APMSUSPEND) {
                displayOff_ = true;
                HideBubble();
                return TRUE;
            }
            if (wp == PBT_APMRESUMEAUTOMATIC || wp == PBT_APMRESUMESUSPEND) {
                displayOff_ = false;
                return TRUE;
            }
            return TRUE;

        case WM_WTSSESSION_CHANGE:
            if (wp == WTS_SESSION_LOCK) {
                sessionLocked_ = true;
                HideBubble();
            } else if (wp == WTS_SESSION_UNLOCK) {
                sessionLocked_ = false;
            }
            return 0;

        case WM_ENDSESSION:
            RemoveTrayIcon();
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        default:
            break;
    }
    return DefWindowProcW(hwnd, message, wp, lp);
}

int App::Run() {
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        HWND settings = settingsWin_.Hwnd();
        if (settings && IsDialogMessageW(settings, &msg)) continue;
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return static_cast<int>(msg.wParam);
}

// ---------------------------------------------------------------- tray icon

void App::AddTrayIcon() {
    if (!mainWnd_ || !trayIcon_) return;
    NOTIFYICONDATAW nid{};
    nid.cbSize = sizeof(nid);
    nid.hWnd = mainWnd_;
    nid.uID = kTrayIconId;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = trayIcon_;
    wcsncpy_s(nid.szTip, L10n::Get().T(S::AppName).c_str(), _TRUNCATE);
    Shell_NotifyIconW(NIM_ADD, &nid);
}

void App::RemoveTrayIcon() {
    if (!mainWnd_) return;
    NOTIFYICONDATAW nid{};
    nid.cbSize = sizeof(nid);
    nid.hWnd = mainWnd_;
    nid.uID = kTrayIconId;
    Shell_NotifyIconW(NIM_DELETE, &nid);
}

void App::UpdateTrayTip() {
    if (!mainWnd_) return;
    NOTIFYICONDATAW nid{};
    nid.cbSize = sizeof(nid);
    nid.hWnd = mainWnd_;
    nid.uID = kTrayIconId;
    nid.uFlags = NIF_TIP;
    wcsncpy_s(nid.szTip, L10n::Get().T(S::AppName).c_str(), _TRUNCATE);
    Shell_NotifyIconW(NIM_MODIFY, &nid);
}

void App::ShowTrayMenu(POINT pt) {
    L10n& loc = L10n::Get();
    HMENU menu = CreatePopupMenu();
    AppendMenuW(menu, MF_STRING, IDM_TOGGLE, (running_ ? loc.T(S::HideHer) : loc.T(S::ShowHer)).c_str());
    AppendMenuW(menu, MF_STRING, IDM_NEXTFACE, loc.T(S::NextFace).c_str());

    HMENU skinMenu = CreatePopupMenu();
    for (int i = 0; i < SkinCount(); ++i) {
        UINT flags = MF_STRING | (i == settings_.skinIndex ? MF_CHECKED : 0);
        AppendMenuW(skinMenu, flags, IDM_SKIN_BASE + i, loc.T(SkinAt(i).nameKey).c_str());
    }
    AppendMenuW(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(skinMenu), loc.T(S::SkinMenu).c_str());
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);

    AppendMenuW(menu, MF_STRING | (settings_.wanderOn ? MF_CHECKED : 0), IDM_WANDER,
                loc.T(S::WanderTitle).c_str());
    AppendMenuW(menu, MF_STRING | (settings_.randomFace ? MF_CHECKED : 0), IDM_RANDOMFACE,
                loc.T(S::RandomFaceTitle).c_str());
    AppendMenuW(menu, MF_STRING | (settings_.soundOn ? MF_CHECKED : 0), IDM_SOUND,
                loc.T(S::SoundTitle).c_str());
    AppendMenuW(menu, MF_STRING | (LaunchAtLoginEnabled() ? MF_CHECKED : 0), IDM_AUTOSTART,
                loc.T(S::LaunchAtLogin).c_str());
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);

    AppendMenuW(menu, MF_STRING, IDM_SETTINGS, loc.T(S::Settings).c_str());
    AppendMenuW(menu, MF_STRING, IDM_ABOUT, loc.T(S::About).c_str());
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, IDM_QUIT, loc.T(S::Quit).c_str());
    SetMenuDefaultItem(menu, IDM_TOGGLE, FALSE);

    SetForegroundWindow(mainWnd_);
    const int command = TrackPopupMenu(
        menu, TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_NONOTIFY, pt.x, pt.y, 0, mainWnd_,
        nullptr);
    DestroyMenu(menu);
    PostMessageW(mainWnd_, WM_NULL, 0, 0);
    if (command) HandleMenuCommand(command);
}

void App::ShowPetMenu(POINT pt) {
    if (!pet_.Alive()) return;
    L10n& loc = L10n::Get();
    HMENU menu = CreatePopupMenu();

    HMENU skinMenu = CreatePopupMenu();
    for (int i = 0; i < SkinCount(); ++i) {
        UINT flags = MF_STRING | (i == settings_.skinIndex ? MF_CHECKED : 0);
        AppendMenuW(skinMenu, flags, IDM_SKIN_BASE + i, loc.T(SkinAt(i).nameKey).c_str());
    }
    AppendMenuW(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(skinMenu), loc.T(S::SkinMenu).c_str());
    AppendMenuW(menu, MF_STRING, IDM_NEXTFACE, loc.T(S::NextFace).c_str());
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, IDM_SETTINGS, loc.T(S::Settings).c_str());
    AppendMenuW(menu, MF_STRING, IDM_TOGGLE, loc.T(S::HideHer).c_str());

    SetForegroundWindow(pet_.Hwnd());
    const int command = TrackPopupMenu(
        menu, TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_NONOTIFY, pt.x, pt.y, 0, pet_.Hwnd(),
        nullptr);
    DestroyMenu(menu);
    if (command) HandleMenuCommand(command);
}

void App::HandleMenuCommand(int id) {
    if (id >= IDM_SKIN_BASE && id < IDM_SKIN_BASE + SkinCount()) {
        SelectSkin(id - IDM_SKIN_BASE);
        return;
    }
    switch (id) {
        case IDM_TOGGLE: TogglePet(); break;
        case IDM_NEXTFACE: CycleExpression(); break;
        case IDM_WANDER:
            settings_.wanderOn = !settings_.wanderOn;
            ApplyBehaviourChange();
            break;
        case IDM_RANDOMFACE:
            settings_.randomFace = !settings_.randomFace;
            ApplyBehaviourChange();
            break;
        case IDM_SOUND:
            settings_.soundOn = !settings_.soundOn;
            ApplyBehaviourChange();
            break;
        case IDM_AUTOSTART: {
            const bool wanted = !LaunchAtLoginEnabled();
            if (!SetLaunchAtLogin(wanted)) {
                MessageBoxW(nullptr, L10n::Get().T(S::LoginItemFailed).c_str(),
                            L10n::Get().T(S::AppName).c_str(), MB_OK | MB_ICONWARNING);
            }
            break;
        }
        case IDM_SETTINGS: OpenSettings(); break;
        case IDM_ABOUT: OpenAbout(); break;
        case IDM_QUIT: Quit(); break;
        default: break;
    }
}

// ---------------------------------------------------------------- show / hide

void App::ShowPet() {
    if (running_) return;
    if (!pet_.Alive() && !pet_.Create(hinst_, this)) return;

    pet_.SetSizes(settings_.hoverHeight, settings_.perchWidth, settings_.amplitude);
    pet_.SetSkinIndex(settings_.skinIndex);
    pet_.SetExpression(0);
    expression_ = 0;
    perched_ = false;
    running_ = true;

    const int w = pet_.WindowWidth(), h = pet_.WindowHeight();
    POINT origin;
    if (settings_.hasPosition) {
        origin = ClampOrigin(static_cast<int>(settings_.windowX),
                             static_cast<int>(settings_.windowY), w, h);
    } else {
        const RECT work = WorkRect();
        const int x = (work.left + work.right) / 2 - w / 2;
        // Same starting spot as the originals: 42 % down the work area.
        const int y = work.top +
                      static_cast<int>(std::lround((work.bottom - work.top) * 0.42));
        origin = ClampOrigin(x, y, w, h);
    }
    pet_.MoveTo(origin.x, origin.y, w, h);
    pet_.SetVisible(true);

    if (settings_.perched) EnterPerch(settings_.perchedEdge == 1, false);

    settings_.running = true;
    settings_.Save();
    ScheduleNextRandomFace(NowSeconds());
    SetAnimating(true);
    UpdateTrayTip();
    settingsWin_.Refresh();
}

void App::HidePet() {
    if (!running_) return;
    running_ = false;
    HideBubble();
    SetHovering(false);
    pet_.SetVisible(false);
    pet_.Destroy();
    perched_ = false;
    settings_.running = false;
    settings_.Save();
    SetAnimating(false);
    settingsWin_.Refresh();
}

void App::TogglePet() {
    if (running_) {
        HidePet();
    } else {
        ShowPet();
    }
}

void App::Quit() {
    RemoveTrayIcon();
    if (settingsWin_.Hwnd()) settingsWin_.Close();
    if (aboutWin_.IsOpen()) aboutWin_.Close();
    if (powerNotify_) {
        UnregisterPowerSettingNotification(powerNotify_);
        powerNotify_ = nullptr;
    }
    if (sessionNotify_) {
        WTSUnRegisterSessionNotification(mainWnd_);
        sessionNotify_ = false;
    }
    settings_.Save();
    if (mainWnd_) DestroyWindow(mainWnd_);
}

// ---------------------------------------------------------------- frame loop

bool App::ShouldAnimate() {
    if (!running_ || !pet_.Alive()) return false;
    if (displayOff_ || sessionLocked_) return false;
    if (!pet_.ClickThrough() && pet_.IsOpaqueAtScreenPoint(PetRectCenter())) return false;
    return true;
}

void App::SetAnimating(bool on) {
    if (on == animating_) return;
    animating_ = on;
    if (!mainWnd_) return;
    if (on) {
        SetTimer(mainWnd_, kFrameTimer, kFrameIntervalMs, nullptr);
    } else {
        KillTimer(mainWnd_, kFrameTimer);
    }
}

void App::OnFrame() {
    if (!running_ || !pet_.Alive()) return;
    const double now = NowSeconds();
    UpdatePointerState();
    UpdateWander(now);
    UpdateRandomFace(now);
    UpdateBubble(now);
    pet_.Tick(now);
}

void App::OnWatchdog() {
    if (!running_ || !pet_.Alive()) return;
    bool animate = true;
    if (displayOff_ || sessionLocked_) {
        animate = false;
    } else if (!pet_.ClickThrough()) {
        // Occlusion: is something else on top of her centre?
        const RECT r = PetRect();
        const POINT centre{(r.left + r.right) / 2, (r.top + r.bottom) / 2};
        if (pet_.IsOpaqueAtScreenPoint(centre)) {
            HWND top = WindowFromPoint(centre);
            if (top && top != pet_.Hwnd()) {
                DWORD pid = 0;
                GetWindowThreadProcessId(top, &pid);
                if (pid != GetCurrentProcessId()) animate = false;
            }
        }
    }
    SetAnimating(animate);
}

// ---------------------------------------------------------------- geometry

RECT App::MonitorRect() const {
    HMONITOR monitor =
        pet_.Alive() ? MonitorFromWindow(pet_.Hwnd(), MONITOR_DEFAULTTONEAREST)
                     : MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(monitor, &mi);
    return mi.rcMonitor;
}

RECT App::WorkRect() const {
    HMONITOR monitor =
        pet_.Alive() ? MonitorFromWindow(pet_.Hwnd(), MONITOR_DEFAULTTONEAREST)
                     : MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(monitor, &mi);
    return mi.rcWork;
}

float App::Scale() const { return pet_.Alive() ? pet_.DpiScale() : 1.0f; }

/// Keep her mostly on screen: up to 12 % may hang off the top of the work area
/// or the bottom of the screen, exactly like the original's clamp().  The
/// taskbar is excluded from the top limit so she can never hide underneath it.
POINT App::ClampOrigin(int x, int y, int w, int h) const {
    const RECT mon = MonitorRect();
    const RECT work = WorkRect();
    const int cx = Clamp(x, static_cast<int>(mon.left),
                         std::max(static_cast<int>(mon.left),
                                  static_cast<int>(mon.right) - w));
    // The window's top may sit at most 12 % of her height above the work area…
    const int lo = static_cast<int>(work.top) -
                   static_cast<int>(std::lround(h * 0.12));
    // …and her bottom at most 12 % below the bottom of the screen.
    const int hi = static_cast<int>(mon.bottom) -
                   static_cast<int>(std::lround(h * 0.88));
    const int cy = Clamp(y, std::min(lo, hi), std::max(lo, hi));
    return POINT{cx, cy};
}

RECT App::PetRect() const {
    RECT r{};
    if (pet_.Alive()) GetWindowRect(pet_.Hwnd(), &r);
    return r;
}

POINT App::PetRectCenter() const {
    const RECT r = PetRect();
    return POINT{(r.left + r.right) / 2, (r.top + r.bottom) / 2};
}

void App::MovePet(int x, int y, int w, int h) {
    if (pet_.Alive()) pet_.MoveTo(x, y, w, h);
}

void App::ApplyLayout() {
    if (!pet_.Alive()) return;
    const RECT old = PetRect();
    const int w = pet_.WindowWidth(), h = pet_.WindowHeight();
    const int x = old.left + ((old.right - old.left) - w) / 2;
    const POINT p = ClampOrigin(x, old.top, w, h);
    MovePet(p.x, p.y, w, h);
    wanderTargetX_ = -1;
}

void App::EnterPerch(bool onRight, bool persist) {
    if (!pet_.Alive()) return;
    pet_.SetMode(true, onRight);
    perched_ = true;
    perchedRight_ = onRight;
    const int w = pet_.WindowWidth(), h = pet_.WindowHeight();
    const RECT mon = MonitorRect();
    const RECT cur = PetRect();
    const int x = onRight ? static_cast<int>(mon.right) - w : static_cast<int>(mon.left);
    const POINT p = ClampOrigin(x, cur.top, w, h);
    MovePet(p.x, p.y, w, h);
    wanderTargetX_ = -1;
    if (persist) {
        settings_.perched = true;
        settings_.perchedEdge = onRight ? 1 : 0;
        settings_.Save();
    }
    settingsWin_.Refresh();
}

void App::ExitPerch() {
    if (!pet_.Alive()) return;
    pet_.SetMode(false, false);
    perched_ = false;
    settings_.perched = false;
    ApplyLayout();
    settings_.Save();
    settingsWin_.Refresh();
}

void App::RememberPosition() {
    const RECT r = PetRect();
    if (r.right - r.left <= 0) return;
    settings_.windowX = r.left;
    settings_.windowY = r.top;
    settings_.hasPosition = true;
    settings_.SavePosition();
}

// ---------------------------------------------------------------- pointer

void App::UpdatePointerState() {
    if (!running_ || !pet_.Alive()) {
        SetHovering(false);
        return;
    }
    if (pet_.IsDragging()) {  // never let go of a drag in progress
        pet_.SetClickThrough(false);
        SetHovering(false);
        return;
    }
    POINT mouse{};
    if (!GetCursorPos(&mouse)) return;
    const bool opaque = pet_.IsOpaqueAtScreenPoint(mouse);
    SetHovering(opaque);
    const bool shouldIgnore = !opaque;
    if (shouldIgnore != pet_.ClickThrough()) {
        pet_.SetClickThrough(shouldIgnore);
        // She is transparent right under the pointer: hand the cursor back to
        // whatever is behind her. Only on the transition, never per frame.
        if (shouldIgnore) SetCursor(LoadCursorW(nullptr, IDC_ARROW));
    }
}

void App::SetHovering(bool on) {
    if (on == hovering_) return;
    hovering_ = on;
    if (on) {
        hoverBeganAt_ = NowSeconds();
    } else {
        HideBubble();
        lastBubbleSecond_ = -1;
        lastBubbleAnchor_ = RECT{};
    }
}

void App::HideBubble() { bubble_.Hide(); }

void App::UpdateBubble(double now) {
    if (!running_ || !pet_.Alive() || !hovering_ || pet_.IsDragging()) {
        bubble_.Hide();
        return;
    }
    const RECT r = PetRect();
    L10n& loc = L10n::Get();
    if (!bubble_.IsVisible()) {
        // Wait a beat before popping up, so a passing cursor doesn't flash it.
        if (now - hoverBeganAt_ < 0.55) return;
        SYSTEMTIME st;
        GetLocalTime(&st);
        const std::wstring sentence = loc.RandomCaringSentence(lastBubbleSentence_);
        bubble_.Show(r.left, r.top, r.right - r.left, r.bottom - r.top, Scale(), sentence,
                     loc.FullDate(st), loc.MediumTime(st));
        lastBubbleSentence_ = sentence;
        lastBubbleSecond_ = static_cast<int>(now);
        lastBubbleAnchor_ = r;
        return;
    }
    if (r != lastBubbleAnchor_) {  // she can be resized or dragged
        bubble_.Move(r.left, r.top, r.right - r.left, r.bottom - r.top);
        lastBubbleAnchor_ = r;
    }
    if (static_cast<int>(now) != lastBubbleSecond_) {  // keep the clock honest
        lastBubbleSecond_ = static_cast<int>(now);
        SYSTEMTIME st;
        GetLocalTime(&st);
        bubble_.SetClock(loc.FullDate(st), loc.MediumTime(st));
    }
}

// ---------------------------------------------------------------- behaviour

void App::UpdateWander(double now) {
    if (!pet_.Alive()) return;
    if (!settings_.wanderOn || !running_ || perched_ || pet_.IsDragging() || hovering_ ||
        pet_.SecondsSinceTouch() <= 5.0) {
        pet_.SetWalking(false);
        wanderTargetX_ = -1;
        return;
    }
    if (now < wanderWaitUntil_) {
        pet_.SetWalking(false);
        return;
    }
    const RECT mon = MonitorRect();
    const RECT r = PetRect();
    const double width = r.right - r.left;
    const double margin = kWanderMargin * Scale();
    const double step = kWanderStep * Scale();

    if (wanderTargetX_ < 0) {
        wanderX_ = r.left;
        const double low = mon.left + margin;
        const double high = std::max(low + 1.0, mon.right - width - margin);
        wanderTargetX_ = RandomDouble(low, high);
        wanderWaitUntil_ = now + 0.8;
        return;
    }
    const double d = wanderTargetX_ - wanderX_;
    if (std::fabs(d) < 3.0) {
        wanderX_ = wanderTargetX_;
        wanderTargetX_ = -1;
        wanderWaitUntil_ = now + 1.2 + RandomDouble(0.0, 2.4);
        pet_.SetWalking(false);
        return;
    }
    wanderX_ += (d < 0 ? -1.0 : 1.0) * std::min(step, std::fabs(d));
    MovePet(static_cast<int>(std::lround(wanderX_)), r.top, r.right - r.left,
            r.bottom - r.top);
    pet_.SetWalking(true);
}

void App::ScheduleNextRandomFace(double now) {
    nextFaceChangeAt_ = now + RandomDouble(18.0, 75.0);
}

void App::UpdateRandomFace(double now) {
    if (!settings_.randomFace || !running_ || !pet_.Alive()) return;
    if (pet_.IsDragging() || hovering_) return;
    // Don't override a face the user just picked.
    if (pet_.SecondsSinceTouch() <= 4.0) {
        ScheduleNextRandomFace(now);
        return;
    }
    if (nextFaceChangeAt_ <= 0) {
        ScheduleNextRandomFace(now);
        return;
    }
    if (now < nextFaceChangeAt_) return;
    pet_.SetExpression(RandomExpression(pet_.Expression()));
    expression_ = pet_.Expression();
    ScheduleNextRandomFace(now);
}

/// Includes the neutral standing art, so she also drifts back to plain now and then.
int App::RandomExpression(int differentFrom) const {
    std::vector<int> candidates;
    for (int candidate = 0; candidate <= 6; ++candidate) {
        if (candidate == differentFrom) continue;
        if (candidate == 0 ||
            ArtworkStore::Get().HasArtwork(settings_.skinIndex, candidate)) {
            candidates.push_back(candidate);
        }
    }
    if (candidates.empty()) return differentFrom;
    return candidates[RandomInt(0, static_cast<int>(candidates.size()) - 1)];
}

// ---------------------------------------------------------------- actions

void App::CycleExpression() {
    if (!pet_.Alive()) return;
    expression_ = pet_.CycleExpression();
    ScheduleNextRandomFace(NowSeconds());
    settingsWin_.Refresh();
}

void App::TestFace() { CycleExpression(); }

void App::SelectSkin(int index) {
    if (index < 0 || index >= SkinCount() || index == settings_.skinIndex) return;
    settings_.skinIndex = index;
    ArtworkStore::Get().Purge();
    if (pet_.Alive()) {
        pet_.SetSkinIndex(index);
        pet_.SetExpression(0);
        expression_ = 0;
    }
    ScheduleNextRandomFace(NowSeconds());
    ApplyLayout();
    settings_.Save();
    settingsWin_.Refresh();
}

void App::ApplySizeChange() {
    if (pet_.Alive()) {
        pet_.SetSizes(settings_.hoverHeight, settings_.perchWidth, settings_.amplitude);
        pet_.Invalidate();
    }
    ApplyLayout();
    settings_.Save();
}

void App::ApplyBehaviourChange() {
    settings_.Save();
    settingsWin_.Refresh();
}

void App::OnLanguageChanged() {
    settings_.language = L10n::Get().language();
    settings_.Save();
    UpdateTrayTip();
    if (settingsWin_.Hwnd()) settingsWin_.Refresh();
    if (aboutWin_.IsOpen()) aboutWin_.Refresh();
}

void App::OpenSettings() { settingsWin_.Open(hinst_); }

void App::OpenAbout() { aboutWin_.Open(hinst_); }

int App::LoadedExpressionCount() const {
    return ArtworkStore::Get().AvailableExpressionCount(settings_.skinIndex);
}

std::wstring App::PetStateText() const {
    L10n& loc = L10n::Get();
    if (!running_) return loc.T(S::StatusReady);
    if (perched_) {
        return loc.T(S::StatusRunning) + L" · " +
               loc.T(perchedRight_ ? S::StatePerchRight : S::StatePerchLeft);
    }
    return loc.T(S::StatusRunning) + L" · " + loc.T(S::StateHover);
}

// ---------------------------------------------------------------- delegate

void App::OnPetDrag(int windowX, int windowY) {
    if (!pet_.Alive()) return;
    const int w = pet_.WindowWidth(), h = pet_.WindowHeight();
    if (perched_) {
        // Dragging her away from the edge is what brings her back to floating.
        const RECT mon = MonitorRect();
        const double width = mon.right - mon.left;
        if (windowX > mon.left + width * 0.15 && windowX < mon.left + width * 0.85) {
            ExitPerch();
            MovePet(windowX, windowY, w, h);
            return;
        }
        MovePet(windowX, PetRect().top, w, h);
        return;
    }
    const POINT p = ClampOrigin(windowX, windowY, w, h);
    MovePet(p.x, p.y, w, h);
    wanderTargetX_ = -1;
}

void App::OnPetBeginDrag() { wanderTargetX_ = -1; }

void App::OnPetEndDrag(POINT screenPoint) {
    (void)screenPoint;
    if (!pet_.Alive()) return;
    RememberPosition();
    if (perched_) return;
    const RECT r = PetRect();
    const RECT mon = MonitorRect();
    // A fixed snap distance would swallow her once she is small, so the
    // threshold is the smaller of 56 pt and 25 % of her width.
    const int snap = std::min(static_cast<int>(std::lround(56.0 * Scale())),
                              static_cast<int>((r.right - r.left) * 0.25));
    if (r.left <= mon.left + snap) {
        EnterPerch(false, true);
    } else if (r.right >= mon.right - snap) {
        EnterPerch(true, true);
    } else {
        ApplyLayout();
    }
}

void App::OnPetTapped() {
    expression_ = pet_.Expression();
    ScheduleNextRandomFace(NowSeconds());
    if (settings_.soundOn) DuckSound::Get().Squeak();
    settingsWin_.Refresh();
}

void App::OnPetMenu(POINT screenPoint) { ShowPetMenu(screenPoint); }

}  // namespace bfy

bool SignalRunningInstance(unsigned int message) {
    HWND existing = FindWindowW(L"BaifanYuMainWindow", nullptr);
    if (!existing) return false;
    PostMessageW(existing, message, 0, 0);
    return true;
}
