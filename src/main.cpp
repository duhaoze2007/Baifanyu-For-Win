// BaifanYu (白饭鱼) for Windows — entry point.
//
// A rice-eating whale girl who lives on your desktop.  Port of the Android app
// https://github.com/LIN428924379/baifanyu and of the macOS port
// https://github.com/duhaoze2007/BaifanYu-For-Mac.
#include "app.h"
#include "common.h"

#include <shellapi.h>  // CommandLineToArgvW (needs windows.h first)

namespace {

/// Handy for a Start-menu shortcut: `BaifanYu.exe --settings` / `--about`.
bool HasFlag(const wchar_t* wanted) {
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) return false;
    bool found = false;
    for (int i = 1; i < argc && !found; ++i) {
        if (_wcsicmp(argv[i], wanted) == 0) found = true;
    }
    LocalFree(argv);
    return found;
}

}  // namespace

int APIENTRY wWinMain(HINSTANCE hinst, HINSTANCE, LPWSTR, int) {
    INITCOMMONCONTROLSEX icc{};
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_BAR_CLASSES | ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&icc);

    const bool wantSettings = HasFlag(L"--settings");
    const bool wantAbout = HasFlag(L"--about");

    bfy::App& app = bfy::App::Get();
    switch (app.Init(hinst)) {
        case bfy::App::StartResult::AlreadyRunning:
            // She is already up: nudge the running copy instead of starting a
            // second pet.  Without a flag that just brings her out.
            if (wantSettings) {
                SignalRunningInstance(bfy::App::kMsgOpenSettings);
            } else if (wantAbout) {
                SignalRunningInstance(bfy::App::kMsgOpenAbout);
            }
            return 0;
        case bfy::App::StartResult::Declined:
            return 1;
        case bfy::App::StartResult::Ok:
        default:
            break;
    }
    if (wantSettings) app.OpenSettings();
    if (wantAbout) app.OpenAbout();
    return app.Run();
}
