// BaifanYu (白饭鱼) for Windows — the hover bubble.
//
// Rest the pointer on her and she stops wandering while a small bubble shows
// today's date, a live clock and a random kind word.  It is a separate,
// always-click-through topmost window, so it never steals a click from whatever
// is underneath.
#ifndef BAIFANYU_BUBBLE_H
#define BAIFANYU_BUBBLE_H

#include "common.h"

namespace bfy {

class HoverBubble {
  public:
    ~HoverBubble() { Destroy(); }

    bool Create(HINSTANCE hinst);
    void Destroy();
    bool IsVisible() const { return visible_; }

    /// Show (or relocate) the bubble next to the pet's screen rectangle.
    void Show(int petX, int petY, int petW, int petH, float scale,
              const std::wstring& sentence, const std::wstring& date,
              const std::wstring& time);
    void Move(int petX, int petY, int petW, int petH);
    /// Keep the clock honest while it stays on screen.
    void SetClock(const std::wstring& date, const std::wstring& time);
    void Hide();

  private:
    void Layout(int petX, int petY, int petW, int petH, bool force);
    bool EnsureSurface(int w, int h);
    void ReleaseSurface();
    void Render();

    HINSTANCE hinst_ = nullptr;
    HWND hwnd_ = nullptr;
    bool visible_ = false;

    HDC memDc_ = nullptr;
    HBITMAP dib_ = nullptr;
    void* bits_ = nullptr;
    int surfW_ = 0, surfH_ = 0;
    Gdiplus::Bitmap* surface_ = nullptr;

    float scale_ = 1.0f;
    int x_ = 0, y_ = 0;
    std::wstring sentence_, date_, time_;
};

}  // namespace bfy

#endif  // BAIFANYU_BUBBLE_H
