#include "bubble.h"

#include <algorithm>

#include "ui.h"

namespace bfy {

namespace {

constexpr wchar_t kBubbleClass[] = L"BaifanYuHoverBubble";

// Logical layout at 96 dpi (the macOS panel is 268 x 104 pt).
constexpr float kW = 280.0f;
constexpr float kH = 112.0f;
constexpr float kRadius = 12.0f;
constexpr float kPadX = 14.0f;

struct Colors {
    Gdiplus::Color bg, border, primary, secondary;
};

Colors Theme() {
    if (SystemUsesDarkMode()) {
        return {Gdiplus::Color(232, 32, 32, 32), Gdiplus::Color(60, 255, 255, 255),
                Gdiplus::Color(255, 244, 244, 244), Gdiplus::Color(255, 165, 165, 165)};
    }
    return {Gdiplus::Color(238, 252, 252, 252), Gdiplus::Color(48, 0, 0, 0),
            Gdiplus::Color(255, 22, 22, 22), Gdiplus::Color(255, 112, 112, 112)};
}

}  // namespace

bool HoverBubble::Create(HINSTANCE hinst) {
    hinst_ = hinst;
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = DefWindowProcW;
    wc.hInstance = hinst;
    wc.lpszClassName = kBubbleClass;
    RegisterClassExW(&wc);

    hwnd_ = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW |
                                WS_EX_NOACTIVATE | WS_EX_TRANSPARENT,
                            kBubbleClass, L"", WS_POPUP, 0, 0, 10, 10, nullptr,
                            nullptr, hinst, nullptr);
    return hwnd_ != nullptr;
}

void HoverBubble::Destroy() {
    if (hwnd_) {
        DestroyWindow(hwnd_);
        hwnd_ = nullptr;
    }
    ReleaseSurface();
}

bool HoverBubble::EnsureSurface(int w, int h) {
    if (w < 1 || h < 1) return false;
    if (surface_ && surfW_ == w && surfH_ == h) return true;
    ReleaseSurface();

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    dib_ = CreateDIBSection(nullptr, &bmi, DIB_RGB_COLORS, &bits_, nullptr, 0);
    if (!dib_ || !bits_) return false;
    memDc_ = CreateCompatibleDC(nullptr);
    if (!memDc_) return false;
    SelectObject(memDc_, dib_);
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

void HoverBubble::ReleaseSurface() {
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

void HoverBubble::Render() {
    if (!hwnd_ || !surface_) return;
    const float s = scale_;
    Gdiplus::Graphics g(surface_);
    g.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
    g.Clear(Gdiplus::Color(0, 0, 0, 0));
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
    g.SetCompositingMode(Gdiplus::CompositingModeSourceOver);

    const Colors c = Theme();
    const Gdiplus::RectF box(0.5f, 0.5f, surfW_ - 1.0f, surfH_ - 1.0f);
    Gdiplus::GraphicsPath path;
    {
        const float r = kRadius * s;
        const float d = r * 2;
        const Gdiplus::RectF a(box.X, box.Y, box.Width, box.Height);
        path.AddArc(a.X, a.Y, d, d, 180, 90);
        path.AddArc(a.X + a.Width - d, a.Y, d, d, 270, 90);
        path.AddArc(a.X + a.Width - d, a.Y + a.Height - d, d, d, 0, 90);
        path.AddArc(a.X, a.Y + a.Height - d, d, d, 90, 90);
        path.CloseFigure();
    }
    Gdiplus::SolidBrush bgBrush(c.bg);
    g.FillPath(&bgBrush, &path);
    Gdiplus::Pen borderPen(c.border, std::max(1.0f, 1.0f * s));
    g.DrawPath(&borderPen, &path);

    Gdiplus::FontFamily family(UiFontFamily());
    Gdiplus::StringFormat left;
    left.SetAlignment(Gdiplus::StringAlignmentNear);
    left.SetLineAlignment(Gdiplus::StringAlignmentNear);
    left.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
    left.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);

    Gdiplus::SolidBrush secondaryBrush(c.secondary);
    Gdiplus::SolidBrush primaryBrush(c.primary);

    const float padX = kPadX * s;

    // Date — small, secondary.
    {
        Gdiplus::Font font(&family, 12.5f * s, Gdiplus::FontStyleRegular,
                           Gdiplus::UnitPixel);
        Gdiplus::RectF r(padX, 10.0f * s, surfW_ - padX * 2, 18.0f * s);
        g.DrawString(date_.c_str(), -1, &font, r, &left, &secondaryBrush);
    }
    // Live clock — big.
    {
        Gdiplus::Font font(&family, 25.0f * s, Gdiplus::FontStyleBold,
                           Gdiplus::UnitPixel);
        Gdiplus::RectF r(padX - 1.0f * s, 27.0f * s, surfW_ - padX * 2, 34.0f * s);
        g.DrawString(time_.c_str(), -1, &font, r, &left, &primaryBrush);
    }
    // Divider.
    {
        Gdiplus::Pen pen(c.border, std::max(1.0f, 1.0f * s));
        const float y = 67.0f * s;
        g.DrawLine(&pen, padX, y, surfW_ - padX, y);
    }
    // The kind word — up to two lines.
    {
        Gdiplus::Font font(&family, 13.5f * s, Gdiplus::FontStyleRegular,
                           Gdiplus::UnitPixel);
        Gdiplus::StringFormat wrap;
        wrap.SetAlignment(Gdiplus::StringAlignmentNear);
        wrap.SetLineAlignment(Gdiplus::StringAlignmentNear);
        wrap.SetTrimming(Gdiplus::StringTrimmingNone);
        Gdiplus::RectF r(padX, 73.0f * s, surfW_ - padX * 2, 36.0f * s);
        g.DrawString(sentence_.c_str(), -1, &font, r, &wrap, &primaryBrush);
    }

    BLENDFUNCTION blend{};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;
    SIZE size{surfW_, surfH_};
    POINT src{0, 0};
    POINT dst{x_, y_};
    UpdateLayeredWindow(hwnd_, nullptr, &dst, &size, memDc_, &src, 0, &blend, ULW_ALPHA);
}

void HoverBubble::Show(int petX, int petY, int petW, int petH, float scale,
                       const std::wstring& sentence, const std::wstring& date,
                       const std::wstring& time) {
    if (!hwnd_) return;
    scale_ = scale;
    sentence_ = sentence;
    date_ = date;
    time_ = time;
    if (!EnsureSurface(std::lround(kW * scale), std::lround(kH * scale))) return;
    Layout(petX, petY, petW, petH, true);
    ShowWindow(hwnd_, SW_SHOWNOACTIVATE);
    visible_ = true;
    Render();
}

void HoverBubble::Move(int petX, int petY, int petW, int petH) {
    if (!visible_) return;
    Layout(petX, petY, petW, petH, false);
    Render();
}

void HoverBubble::SetClock(const std::wstring& date, const std::wstring& time) {
    if (!visible_) return;
    date_ = date;
    time_ = time;
    Render();
}

void HoverBubble::Hide() {
    if (!hwnd_) return;
    ShowWindow(hwnd_, SW_HIDE);
    visible_ = false;
}

void HoverBubble::Layout(int petX, int petY, int petW, int petH, bool force) {
    const int w = surfW_, h = surfH_;
    HMONITOR monitor = MonitorFromPoint(POINT{petX + petW / 2, petY + petH / 2},
                                        MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(monitor, &mi)) {
        mi.rcWork = RECT{0, 0, GetSystemMetrics(SM_CXSCREEN),
                         GetSystemMetrics(SM_CYSCREEN)};
    }
    const int margin = std::lround(8 * scale_);
    int x = petX + petW / 2 - w / 2;
    int y = petY + petH + std::lround(10 * scale_);          // below her
    if (y + h > mi.rcWork.bottom - margin) {                  // no room: above
        y = petY - h - std::lround(10 * scale_);
    }
    x = std::max(static_cast<int>(mi.rcWork.left) + margin,
                 std::min(x, static_cast<int>(mi.rcWork.right) - w - margin));
    y = std::max(static_cast<int>(mi.rcWork.top) + margin,
                 std::min(y, static_cast<int>(mi.rcWork.bottom) - h - margin));
    if (!force && x == x_ && y == y_) return;
    x_ = x;
    y_ = y;
}

}  // namespace bfy
