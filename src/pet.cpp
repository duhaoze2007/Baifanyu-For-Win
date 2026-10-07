#include "pet.h"

#include <algorithm>
#include <cmath>

namespace bfy {

namespace {
constexpr wchar_t kPetClass[] = L"BaifanYuPetWindow";
constexpr UINT_PTR kLongPressTimer = 1;
constexpr double kLongPressSeconds = 0.8;
constexpr int kDragSlop = 3;           // px before a press becomes a drag
constexpr double kTau = 6.283185307179586;
constexpr BYTE kOpaqueThreshold = 26;  // same alpha cut-off as the macOS port
}  // namespace

float PetWindow::DpiScale() const {
    if (hwnd_) {
        const UINT dpi = GetDpiForWindow(hwnd_);
        if (dpi > 0) return static_cast<float>(dpi) / 96.0f;
    }
    return 1.0f;
}

int PetWindow::ArtHeightPx() const {
    return static_cast<int>(std::lround(hoverHeight_ * DpiScale()));
}

int PetWindow::ArtWidthPx() const {
    return static_cast<int>(std::lround(perchWidth_ * DpiScale()));
}

int PetWindow::WindowWidth() const {
    if (mode_ == Mode::Perch) {
        return static_cast<int>(std::lround(ArtWidthPx() * 1.10));
    }
    return static_cast<int>(std::lround(ArtHeightPx() * SourceAspect()));
}

int PetWindow::WindowHeight() const {
    if (mode_ == Mode::Perch) {
        return static_cast<int>(std::lround(ArtWidthPx() * PerchHeadAspect() * 1.16));
    }
    return static_cast<int>(std::lround(hoverHeight_ * DpiScale() * 1.16));
}

// ---------------------------------------------------------------- lifecycle

bool PetWindow::Create(HINSTANCE hinst, Delegate* delegate) {
    hinst_ = hinst;
    delegate_ = delegate;

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = &PetWindow::WndProc;
    wc.hInstance = hinst;
    wc.lpszClassName = kPetClass;
    wc.hCursor = nullptr;  // the cursor is managed explicitly
    RegisterClassExW(&wc);  // an "already registered" error is fine

    const int w = WindowWidth(), h = WindowHeight();
    hwnd_ = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW |
                                WS_EX_NOACTIVATE,
                            kPetClass, L"BaifanYu", WS_POPUP, 0, 0, w, h, nullptr,
                            nullptr, hinst, this);
    if (!hwnd_) return false;

    startTime_ = NowSeconds();
    lastTouch_ = startTime_;
    EnsureSurface(w, h);
    Render();
    return true;
}

void PetWindow::Destroy() {
    if (hwnd_) {
        KillTimer(hwnd_, kLongPressTimer);
        DestroyWindow(hwnd_);
        hwnd_ = nullptr;
    }
    ReleaseSurface();
    visible_ = false;
}

LRESULT CALLBACK PetWindow::WndProc(HWND hwnd, UINT message, WPARAM wp, LPARAM lp) {
    PetWindow* self = nullptr;
    if (message == WM_NCCREATE) {
        auto* cs = reinterpret_cast<CREATESTRUCTW*>(lp);
        self = static_cast<PetWindow*>(cs->lpCreateParams);
        self->hwnd_ = hwnd;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
    } else {
        self = reinterpret_cast<PetWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    }
    if (!self) return DefWindowProcW(hwnd, message, wp, lp);
    return self->Handle(message, wp, lp);
}

LRESULT PetWindow::Handle(UINT message, WPARAM wp, LPARAM lp) {
    switch (message) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;

        case WM_SETCURSOR:
            if (dragging_) {
                SetCursor(LoadCursorW(nullptr, IDC_SIZEALL));
                return TRUE;
            }
            SetCursor(LoadCursorW(nullptr, IDC_HAND));
            return TRUE;

        case WM_LBUTTONDOWN: {
            if (GetKeyState(VK_CONTROL) & 0x8000) {  // control-click = her menu
                POINT pt;
                GetCursorPos(&pt);
                if (delegate_) delegate_->OnPetMenu(pt);
                return 0;
            }
            lastTouch_ = NowSeconds();
            GetCursorPos(&mouseDownScreen_);
            lastScreen_ = mouseDownScreen_;
            windowOriginAtDown_ = POINT{posX_, posY_};
            dragging_ = false;
            dragTilt_ = 0;
            SetCapture(hwnd_);
            SetTimer(hwnd_, kLongPressTimer,
                     static_cast<UINT>(kLongPressSeconds * 1000), nullptr);
            Invalidate();
            return 0;
        }

        case WM_MOUSEMOVE: {
            if (!dragging_ && GetCapture() != hwnd_) return 0;
            POINT pt;
            GetCursorPos(&pt);
            const double dist = std::hypot(static_cast<double>(pt.x - mouseDownScreen_.x),
                                           static_cast<double>(pt.y - mouseDownScreen_.y));
            if (dist > kDragSlop) KillTimer(hwnd_, kLongPressTimer);
            if (!dragging_ && dist > kDragSlop) {
                dragging_ = true;
                if (delegate_) delegate_->OnPetBeginDrag();
            }
            if (!dragging_) return 0;

            lastTouch_ = NowSeconds();
            dragTilt_ = Clamp(dragTilt_ + (pt.x - lastScreen_.x) * 0.35, -9.0, 9.0);
            dragTilt_ *= 0.85;
            lastScreen_ = pt;
            if (delegate_) {
                delegate_->OnPetDrag(windowOriginAtDown_.x + (pt.x - mouseDownScreen_.x),
                                     windowOriginAtDown_.y + (pt.y - mouseDownScreen_.y));
            }
            Invalidate();
            return 0;
        }

        case WM_LBUTTONUP: {
            KillTimer(hwnd_, kLongPressTimer);
            if (GetCapture() == hwnd_) ReleaseCapture();
            POINT pt;
            GetCursorPos(&pt);
            if (dragging_) {
                dragging_ = false;
                dragTilt_ = 0;
                if (delegate_) delegate_->OnPetEndDrag(pt);
            } else {
                CycleExpression();
                PokeFeedback();
                if (delegate_) delegate_->OnPetTapped();
            }
            Invalidate();
            return 0;
        }

        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP: {
            KillTimer(hwnd_, kLongPressTimer);
            if (message == WM_RBUTTONUP) {
                POINT pt;
                GetCursorPos(&pt);
                if (delegate_) delegate_->OnPetMenu(pt);
            }
            return 0;
        }

        case WM_TIMER:
            if (wp == kLongPressTimer) {
                KillTimer(hwnd_, kLongPressTimer);
                if (!dragging_) {
                    POINT pt;
                    GetCursorPos(&pt);
                    if (delegate_) delegate_->OnPetMenu(pt);
                }
            }
            return 0;

        case WM_ERASEBKGND:
            return 1;

        default:
            break;
    }
    return DefWindowProcW(hwnd_, message, wp, lp);
}

// ---------------------------------------------------------------- surface

bool PetWindow::EnsureSurface(int w, int h) {
    if (w < 1 || h < 1) return false;
    if (surface_ && surfW_ == w && surfH_ == h) return true;
    ReleaseSurface();

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;  // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    dib_ = CreateDIBSection(nullptr, &bmi, DIB_RGB_COLORS, &bits_, nullptr, 0);
    if (!dib_ || !bits_) return false;
    memDc_ = CreateCompatibleDC(nullptr);
    if (!memDc_) return false;
    SelectObject(memDc_, dib_);

    // GDI+ draws straight into the DIB, so presenting is a zero-copy blit.
    surface_ = new Gdiplus::Bitmap(w, h, w * 4, PixelFormat32bppPARGB,
                                   static_cast<BYTE*>(bits_));
    if (!surface_ || surface_->GetLastStatus() != Gdiplus::Ok) {
        ReleaseSurface();
        return false;
    }
    surfW_ = w;
    surfH_ = h;
    return true;
}

void PetWindow::ReleaseSurface() {
    delete surface_;
    surface_ = nullptr;
    if (memDc_) {
        DeleteDC(memDc_);
        memDc_ = nullptr;
    }
    if (dib_) {
        DeleteObject(dib_);
        dib_ = nullptr;
    }
    bits_ = nullptr;
    surfW_ = surfH_ = 0;
}

void PetWindow::Present() {
    if (!hwnd_ || !memDc_) return;
    BLENDFUNCTION blend{};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;
    SIZE size{surfW_, surfH_};
    POINT src{0, 0};
    // pptDst = nullptr: the window position is owned by MoveTo()/SetWindowPos,
    // this call only refreshes the pixels.
    UpdateLayeredWindow(hwnd_, nullptr, nullptr, &size, memDc_, &src, 0, &blend,
                        ULW_ALPHA);
}

// ---------------------------------------------------------------- animation

void PetWindow::UpdateDynamics(double t) {
    pDy_ = 0.022 * amplitude_ * std::sin(kTau * t / 2.4);
    pSx_ = 1.0;
    pSy_ = 1.0 + 0.014 * amplitude_ * std::sin(kTau * 2.0 * t / 2.4);

    if (dragging_) {
        pSx_ += 0.05 * std::fabs(dragTilt_) / 9.0;
        pSy_ -= 0.02 * std::fabs(dragTilt_) / 9.0;
    }
    if (walking_) {
        // Walking = left/right sway + up/down bounce.  Pure translation looks
        // like a sliding sticker, not like walking.
        const double wk = kTau * t * 1.9;
        pDy_ += 0.016 * amplitude_ * std::fabs(std::sin(wk));
        pSy_ *= 1.0 + 0.018 * amplitude_ * std::sin(wk * 2.0);
        dragTilt_ = 5.5 * std::sin(wk);
    } else if (!dragging_) {
        dragTilt_ = 0.0;
    }
    if (tapTime_ != 0) {
        const double now = NowSeconds();
        const double k = (now - tapTime_) / 0.30;
        if (k >= 1.0) {
            tapTime_ = 0;
        } else {
            pSy_ *= 1.0 - 0.10 * std::sin(M_PI * k);
        }
    }
}

void PetWindow::Tick(double now) {
    if (!hwnd_ || !visible_) return;
    if (startTime_ == 0) startTime_ = now;
    if (!needsRender_ && now - lastFrameTime_ < FrameInterval()) return;
    lastFrameTime_ = now;
    needsRender_ = false;
    Render();
}

void PetWindow::Render() {
    if (!hwnd_) return;
    const int w = WindowWidth(), h = WindowHeight();
    if (!EnsureSurface(w, h)) return;

    const double now = NowSeconds();
    if (startTime_ == 0) startTime_ = now;
    const double t = now - startTime_;

    Gdiplus::Graphics g(surface_);
    g.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
    g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
    g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    g.Clear(Gdiplus::Color(0, 0, 0, 0));

    UpdateDynamics(t);
    if (mode_ == Mode::Perch) {
        DrawPerch(g, t);
    } else {
        DrawHover(g, t);
    }
    Present();
}

void PetWindow::DrawHover(Gdiplus::Graphics& g, double t) {
    Gdiplus::Bitmap* art =
        ArtworkStore::Get().Hover(skinIndex_, expression_, ArtHeightPx());
    if (!art) return;
    (void)t;

    const double artH = ArtHeightPx();
    const double artW = artH * SourceAspect();
    const double w = surfW_, h = surfH_;
    const double cx = w / 2.0;
    const double bottom = h - artH * 0.08 + pDy_ * artH;
    const double top = bottom - artH * pSy_;
    const double halfW = artW * pSx_ / 2.0;

    Gdiplus::RectF dst(static_cast<Gdiplus::REAL>(cx - halfW),
                       static_cast<Gdiplus::REAL>(top),
                       static_cast<Gdiplus::REAL>(halfW * 2.0),
                       static_cast<Gdiplus::REAL>(bottom - top));

    const Gdiplus::GraphicsState state = g.Save();
    if (dragging_ && dragTilt_ != 0.0) {
        const double pivotY = h - artH * 0.10;
        g.TranslateTransform(static_cast<Gdiplus::REAL>(cx),
                             static_cast<Gdiplus::REAL>(pivotY));
        g.RotateTransform(static_cast<Gdiplus::REAL>(dragTilt_));
        g.TranslateTransform(static_cast<Gdiplus::REAL>(-cx),
                             static_cast<Gdiplus::REAL>(-pivotY));
    }
    g.DrawImage(art, dst);
    g.Restore(state);
}

void PetWindow::DrawPerch(Gdiplus::Graphics& g, double t) {
    const double artW = ArtWidthPx();
    const double artH = artW * PerchHeadAspect();
    const int ph = std::max(4, static_cast<int>(std::lround(artH)));
    Gdiplus::Bitmap* art = ArtworkStore::Get().Perch(skinIndex_, expression_,
                                                     static_cast<int>(artW), ph);
    if (!art) return;

    const double peek = artW * 0.02 * (1.0 + std::sin(kTau * t / 2.4 - 1.2));
    const double hh = artH * pSy_;
    const double top = (surfH_ - hh) / 2.0 + pDy_ * artH;
    Gdiplus::RectF dst(static_cast<Gdiplus::REAL>(peek), static_cast<Gdiplus::REAL>(top),
                       static_cast<Gdiplus::REAL>(artW), static_cast<Gdiplus::REAL>(hh));

    const Gdiplus::GraphicsState state = g.Save();
    if (mirror_) {
        g.TranslateTransform(static_cast<Gdiplus::REAL>(surfW_), 0.0f);
        g.ScaleTransform(-1.0f, 1.0f);
    }
    g.DrawImage(art, dst);
    g.Restore(state);
}

// ---------------------------------------------------------------- behaviour

void PetWindow::SetSizes(double hoverHeightPt, double perchWidthPt, double amplitude) {
    hoverHeight_ = hoverHeightPt;
    perchWidth_ = perchWidthPt;
    amplitude_ = amplitude;
    Invalidate();
}

void PetWindow::SetSkinIndex(int index) {
    if (index == skinIndex_) return;
    skinIndex_ = index;
    expression_ = 0;
    Invalidate();
}

void PetWindow::SetMode(bool perched, bool onRightEdge) {
    const Mode mode = perched ? Mode::Perch : Mode::Hover;
    if (mode_ != mode) {
        mode_ = mode;
        startTime_ = NowSeconds();
    }
    mirror_ = perched && onRightEdge;
    Invalidate();
}

void PetWindow::SetExpression(int expression) {
    if (expression == expression_) return;
    expression_ = expression;
    Invalidate();
}

int PetWindow::AdvancedExpression(int from) const {
    const int total = 6;
    for (int step = 1; step <= total; ++step) {
        const int candidate = (from + step) % (total + 1);
        if (candidate == 0 || ArtworkStore::Get().HasArtwork(skinIndex_, candidate)) {
            return candidate;
        }
    }
    return from;
}

int PetWindow::CycleExpression() {
    expression_ = AdvancedExpression(expression_);
    Invalidate();
    return expression_;
}

void PetWindow::PokeFeedback() {
    tapTime_ = NowSeconds();
    Invalidate();
}

void PetWindow::SetVisible(bool visible) {
    if (!hwnd_) return;
    visible_ = visible;
    if (visible) {
        ShowWindow(hwnd_, SW_SHOWNOACTIVATE);
        SetWindowPos(hwnd_, HWND_TOPMOST, posX_, posY_, WindowWidth(), WindowHeight(),
                     SWP_NOACTIVATE | SWP_SHOWWINDOW);
        startTime_ = NowSeconds();
        needsRender_ = true;
        Render();
    } else {
        ShowWindow(hwnd_, SW_HIDE);
    }
}

void PetWindow::MoveTo(int x, int y, int w, int h) {
    posX_ = x;
    posY_ = y;
    if (!hwnd_) return;
    SetWindowPos(hwnd_, HWND_TOPMOST, x, y, w, h, SWP_NOACTIVATE | SWP_NOREDRAW);
    needsRender_ = true;
}

void PetWindow::SetClickThrough(bool on) {
    if (!hwnd_ || clickThrough_ == on) return;
    clickThrough_ = on;
    LONG_PTR ex = GetWindowLongPtrW(hwnd_, GWL_EXSTYLE);
    if (on) {
        ex |= WS_EX_TRANSPARENT;
    } else {
        ex &= ~WS_EX_TRANSPARENT;
    }
    SetWindowLongPtrW(hwnd_, GWL_EXSTYLE, ex);
}

bool PetWindow::IsOpaqueAtScreenPoint(POINT screenPoint) const {
    if (!hwnd_ || !visible_ || !bits_ || surfW_ <= 0 || surfH_ <= 0) return false;
    RECT r{};
    if (!GetWindowRect(hwnd_, &r)) return false;
    const int x = screenPoint.x - r.left;
    const int y = screenPoint.y - r.top;
    if (x < 0 || y < 0 || x >= surfW_ || y >= surfH_) return false;
    const BYTE* px = static_cast<const BYTE*>(bits_) +
                     (static_cast<ptrdiff_t>(y) * surfW_ + x) * 4;
    return px[3] > kOpaqueThreshold;
}

}  // namespace bfy
