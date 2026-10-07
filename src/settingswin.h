// BaifanYu (白饭鱼) for Windows — the settings window.
//
// The macOS port has a separate Settings window (⌘,) plus a native About
// window; on Windows those are ordinary top-level windows reachable from the
// notification-area menu.
#ifndef BAIFANYU_SETTINGSWIN_H
#define BAIFANYU_SETTINGSWIN_H

#include "common.h"

namespace bfy {

class SettingsWindow {
  public:
    ~SettingsWindow() { Close(); }

    void Open(HINSTANCE hinst);
    void Close();
    bool IsOpen() const { return hwnd_ != nullptr; }
    HWND Hwnd() const { return hwnd_; }

    /// Re-read everything from the app (after a skin / language / state change).
    void Refresh();

  private:
    static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
    LRESULT Handle(HWND hwnd, UINT message, WPARAM wp, LPARAM lp);
    void BuildControls(HWND hwnd);
    void ApplyTexts();
    void SyncFromState();
    float Scale() const;

    HINSTANCE hinst_ = nullptr;
    HWND hwnd_ = nullptr;
    HFONT font_ = nullptr;
    HFONT fontBold_ = nullptr;
    HFONT fontSmall_ = nullptr;
    UINT dpi_ = 96;
};

}  // namespace bfy

#endif  // BAIFANYU_SETTINGSWIN_H
