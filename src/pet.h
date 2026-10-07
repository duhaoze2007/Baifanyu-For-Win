// BaifanYu (白饭鱼) for Windows — the floating pet window.
//
// Port of the macOS PetView (drawing, animation, mouse handling) plus the parts
// of PetController that own the window.  On Windows the equivalent of the
// `.nonactivatingPanel` is a WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW
// popup whose pixels are pushed with UpdateLayeredWindow, so she has real
// per-pixel alpha and never steals focus from whatever you are typing in.
#ifndef BAIFANYU_PET_H
#define BAIFANYU_PET_H

#include "artwork.h"
#include "common.h"

namespace bfy {

class App;

class PetWindow {
  public:
    /// Called by the app when she is dragged / tapped / asked for her menu.
    struct Delegate {
        virtual void OnPetDrag(int windowX, int windowY) = 0;
        virtual void OnPetBeginDrag() = 0;
        virtual void OnPetEndDrag(POINT screenPoint) = 0;
        virtual void OnPetTapped() = 0;
        virtual void OnPetMenu(POINT screenPoint) = 0;
    };

    PetWindow() = default;
    ~PetWindow() { Destroy(); }

    bool Create(HINSTANCE hinst, Delegate* delegate);
    void Destroy();
    bool Alive() const { return hwnd_ != nullptr; }
    HWND Hwnd() const { return hwnd_; }

    // ---- configuration (set by the app) ---------------------------------
    void SetSizes(double hoverHeightPt, double perchWidthPt, double amplitude);
    void SetSkinIndex(int index);
    void SetMode(bool perched, bool onRightEdge);
    void SetExpression(int expression);
    void SetWalking(bool walking) { walking_ = walking; }
    int Expression() const { return expression_; }
    int CycleExpression();
    /// Cycle 0 -> 1 -> ... -> 6 -> 0, skipping expressions whose art is missing.
    int AdvancedExpression(int from) const;

    void SetVisible(bool visible);
    void MoveTo(int x, int y, int w, int h);
    void SetClickThrough(bool on);
    bool ClickThrough() const { return clickThrough_; }

    // ---- state ----------------------------------------------------------
    bool IsDragging() const { return dragging_; }
    bool IsPerched() const { return mode_ == Mode::Perch; }
    bool IsMirrored() const { return mirror_; }
    double SecondsSinceTouch() const { return NowSeconds() - lastTouch_; }
    void PokeFeedback();
    void Invalidate() { needsRender_ = true; }

    int WindowWidth() const;
    int WindowHeight() const;
    float DpiScale() const;

    /// Called from the app's 60 Hz timer; redraws when the frame interval
    /// (30 fps floating / 20 fps perched) has elapsed.
    void Tick(double now);

    /// True when the pixel under this screen point is opaque — the app uses it
    /// for per-pixel click-through and for the hover bubble.
    bool IsOpaqueAtScreenPoint(POINT screenPoint) const;

  private:
    enum class Mode { Hover, Perch };

    static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
    LRESULT Handle(UINT message, WPARAM wp, LPARAM lp);

    bool EnsureSurface(int w, int h);
    void ReleaseSurface();
    void Render();
    void Present();
    void UpdateDynamics(double t);
    void DrawHover(Gdiplus::Graphics& g, double t);
    void DrawPerch(Gdiplus::Graphics& g, double t);

    double FrameInterval() const { return mode_ == Mode::Perch ? 1.0 / 20.0 : 1.0 / 30.0; }
    int ArtHeightPx() const;   // floating: her drawn height
    int ArtWidthPx() const;    // perched: her drawn head width
    float SourceAspect() const { return ArtworkStore::Get().SourceAspect(skinIndex_); }
    float PerchHeadAspect() const {
        return ArtworkStore::Get().PerchHeadAspect(skinIndex_);
    }

    HINSTANCE hinst_ = nullptr;
    HWND hwnd_ = nullptr;
    Delegate* delegate_ = nullptr;

    // Layered surface.
    HDC memDc_ = nullptr;
    HBITMAP dib_ = nullptr;
    void* bits_ = nullptr;
    int surfW_ = 0, surfH_ = 0;
    Gdiplus::Bitmap* surface_ = nullptr;

    // Configuration.
    int skinIndex_ = 0;
    double hoverHeight_ = 240.0;  // pt
    double perchWidth_ = 96.0;    // pt
    double amplitude_ = 0.75;
    Mode mode_ = Mode::Hover;
    bool mirror_ = false;
    int expression_ = 0;
    bool walking_ = false;
    bool clickThrough_ = false;
    bool needsRender_ = true;
    bool visible_ = false;

    // Animation state.
    double startTime_ = 0;
    double lastFrameTime_ = 0;
    double tapTime_ = 0;
    double dragTilt_ = 0;
    double pDy_ = 0, pSx_ = 1, pSy_ = 1;

    // Mouse state.
    bool dragging_ = false;
    POINT mouseDownScreen_{0, 0};
    POINT windowOriginAtDown_{0, 0};
    POINT lastScreen_{0, 0};
    double lastTouch_ = 0;

    int posX_ = 0, posY_ = 0;
};

}  // namespace bfy

#endif  // BAIFANYU_PET_H
