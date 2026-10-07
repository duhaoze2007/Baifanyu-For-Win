// BaifanYu (白饭鱼) for Windows — the About window.
#ifndef BAIFANYU_ABOUTWIN_H
#define BAIFANYU_ABOUTWIN_H

#include "common.h"

namespace bfy {

class AboutWindow {
  public:
    ~AboutWindow() { Close(); }
    void Open(HINSTANCE hinst);
    void Close();
    bool IsOpen() const { return hwnd_ != nullptr; }
    /// Re-render the text after a language change.
    void Refresh();

  private:
    static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
    LRESULT Handle(HWND hwnd, UINT message, WPARAM wp, LPARAM lp);
    void BuildControls(HWND hwnd);
    void BuildText();

    HINSTANCE hinst_ = nullptr;
    HWND hwnd_ = nullptr;
    HFONT font_ = nullptr;
    HFONT fontBold_ = nullptr;
    HFONT fontSmall_ = nullptr;
    HWND text_ = nullptr;
    HWND ok_ = nullptr;
};

}  // namespace bfy

#endif  // BAIFANYU_ABOUTWIN_H
