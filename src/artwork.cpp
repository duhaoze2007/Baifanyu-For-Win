#include "artwork.h"

#include <algorithm>

namespace bfy {

namespace {

const SkinDef kSkins[] = {
    // Bowl-head (饭盆头): the bowl is much wider than the maid's head, so it
    // gets its own crop ratios (measured on the original artwork).
    {0, IDR_SKIN_BASIN_BASE, L"basin", S::SkinBasin, 0.020f, 0.985f, 0.015f, 0.560f},
    // Maid outfit (女仆装): ratios given against the 1191x1514 master art.
    {1, IDR_SKIN_MAID_BASE, L"maid", S::SkinMaid, 35.0f / 1191.0f, 985.0f / 1191.0f,
     30.0f / 1514.0f, 800.0f / 1514.0f},
};

const wchar_t* const kFaceOrder[] = {L"happy", L"sad",  L"angry",
                                     L"surprised", L"shy", L"confused"};

constexpr size_t kCacheLimit = 16;

/// Decode a PNG held in an embedded resource.
Gdiplus::Bitmap* BitmapFromBytes(const std::vector<BYTE>& bytes) {
    if (bytes.empty()) return nullptr;
    HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, bytes.size());
    if (!mem) return nullptr;
    void* dst = GlobalLock(mem);
    if (!dst) {
        GlobalFree(mem);
        return nullptr;
    }
    memcpy(dst, bytes.data(), bytes.size());
    GlobalUnlock(mem);

    IStream* stream = nullptr;
    if (CreateStreamOnHGlobal(mem, TRUE, &stream) != S_OK) {
        GlobalFree(mem);
        return nullptr;
    }
    Gdiplus::Bitmap* raw = Gdiplus::Bitmap::FromStream(stream);
    Gdiplus::Bitmap* copy = nullptr;
    if (raw && raw->GetLastStatus() == Gdiplus::Ok && raw->GetWidth() > 0) {
        // Copy into a plain 32-bit surface so the bitmap no longer depends on
        // the stream (GDI+ can otherwise decode lazily).
        copy = new Gdiplus::Bitmap(raw->GetWidth(), raw->GetHeight(),
                                   PixelFormat32bppPARGB);
        if (copy->GetLastStatus() == Gdiplus::Ok) {
            Gdiplus::Graphics g(copy);
            g.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
            g.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
            g.DrawImage(raw, 0, 0, raw->GetWidth(), raw->GetHeight());
        } else {
            delete copy;
            copy = nullptr;
        }
    }
    delete raw;
    stream->Release();
    return copy;
}

}  // namespace

const SkinDef& SkinAt(int index) {
    if (index < 0) index = 0;
    if (index >= static_cast<int>(_countof(kSkins))) index = _countof(kSkins) - 1;
    return kSkins[index];
}

int SkinCount() { return _countof(kSkins); }

const wchar_t* FaceName(int oneBasedIndex) {
    if (oneBasedIndex < 1 || oneBasedIndex > 6) return L"";
    return kFaceOrder[oneBasedIndex - 1];
}

ArtworkStore& ArtworkStore::Get() {
    static ArtworkStore inst;
    return inst;
}

bool ArtworkStore::HasArtwork(int skin, int expression) const {
    if (expression < 0 || expression > 6) return false;
    const int id = SkinAt(skin).resBase + expression;
    return FindResourceW(hinst_, MAKEINTRESOURCEW(id), RT_RCDATA) != nullptr;
}

int ArtworkStore::AvailableExpressionCount(int skin) const {
    int n = 0;
    for (int i = 1; i <= 6; ++i) {
        if (HasArtwork(skin, i)) ++n;
    }
    return n;
}

Gdiplus::Bitmap* ArtworkStore::Source(int skin, int expression) {
    if (skin < 0 || skin > 1 || expression < 0 || expression > 6) return nullptr;
    if (sources_[skin][expression]) return sources_[skin][expression];

    const int id = SkinAt(skin).resBase + expression;
    std::vector<BYTE> bytes = LoadResourceBytes(hinst_, id);
    if (bytes.empty()) {
        // Mark as missing so we do not retry on every frame.
        sources_[skin][expression] = nullptr;
        return nullptr;
    }
    sources_[skin][expression] = BitmapFromBytes(bytes);
    if (!sources_[skin][expression]) return nullptr;
    return sources_[skin][expression];
}

int ArtworkStore::SourceWidth(int skin) {
    Gdiplus::Bitmap* b = Source(skin, 0);
    return b ? static_cast<int>(b->GetWidth()) : 943;
}

int ArtworkStore::SourceHeight(int skin) {
    Gdiplus::Bitmap* b = Source(skin, 0);
    return b ? static_cast<int>(b->GetHeight()) : 1200;
}

float ArtworkStore::SourceAspect(int skin) {
    const int h = SourceHeight(skin);
    if (h <= 0) return 0.786f;
    return static_cast<float>(SourceWidth(skin)) / static_cast<float>(h);
}

/// Rotated head crop: how tall the drawn head is relative to how wide it is.
///
/// The original artwork's head band is (hx1-hx0)*srcW wide and (hy1-hy0)*srcH
/// tall; rotating that 90 degrees swaps the two, so the *drawn* head is
/// (hy1-hy0)*srcH wide and (hx1-hx0)*srcW tall.  This is the Android app's
/// `perchWindowH()` ratio (`headY1-headY0` over `headX1-headX0`).
float ArtworkStore::PerchHeadAspect(int skin) {
    const SkinDef& s = SkinAt(skin);
    const float rotatedW = (s.headY1 - s.headY0) * static_cast<float>(SourceHeight(skin));
    const float rotatedH = (s.headX1 - s.headX0) * static_cast<float>(SourceWidth(skin));
    if (rotatedW <= 0.5f) return 1.23f;
    return rotatedH / rotatedW;
}

/// The source rotated 90 degrees clockwise — dest(x, y) = src(y, srcH-1-x).
Gdiplus::Bitmap* ArtworkStore::Rotated90(int skin, int expression) {
    if (rotated_[skin][expression]) return rotated_[skin][expression];
    Gdiplus::Bitmap* src = Source(skin, expression);
    if (!src) return nullptr;

    const int sw = static_cast<int>(src->GetWidth());
    const int sh = static_cast<int>(src->GetHeight());
    if (sw <= 0 || sh <= 0) return nullptr;

    Gdiplus::Bitmap* dst = new Gdiplus::Bitmap(sh, sw, PixelFormat32bppPARGB);
    if (dst->GetLastStatus() != Gdiplus::Ok) {
        delete dst;
        return nullptr;
    }

    Gdiplus::Rect rect(0, 0, sw, sh);
    Gdiplus::BitmapData inData{}, outData{};
    if (src->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppPARGB,
                      &inData) != Gdiplus::Ok) {
        delete dst;
        return nullptr;
    }
    Gdiplus::Rect dstRect(0, 0, sh, sw);
    if (dst->LockBits(&dstRect, Gdiplus::ImageLockModeWrite, PixelFormat32bppPARGB,
                      &outData) != Gdiplus::Ok) {
        src->UnlockBits(&inData);
        delete dst;
        return nullptr;
    }

    const BYTE* inBase = static_cast<const BYTE*>(inData.Scan0);
    BYTE* outBase = static_cast<BYTE*>(outData.Scan0);
    for (int y = 0; y < sh; ++y) {
        const BYTE* row = inBase + static_cast<ptrdiff_t>(y) * inData.Stride;
        for (int x = 0; x < sw; ++x) {
            const BYTE* px = row + static_cast<ptrdiff_t>(x) * 4;
            // src (x, y) -> dst (sh - 1 - y, x)
            BYTE* out = outBase +
                        static_cast<ptrdiff_t>(x) * outData.Stride +
                        static_cast<ptrdiff_t>(sh - 1 - y) * 4;
            out[0] = px[0];
            out[1] = px[1];
            out[2] = px[2];
            out[3] = px[3];
        }
    }

    src->UnlockBits(&inData);
    dst->UnlockBits(&outData);
    rotated_[skin][expression] = dst;
    return dst;
}

void ArtworkStore::Remember(const std::wstring& key) {
    // The newest entry is whatever the caller just stored in cache_.
    while (cache_.size() > kCacheLimit) {
        delete cache_.front().second;
        cache_.erase(cache_.begin());
    }
    (void)key;
}

Gdiplus::Bitmap* ArtworkStore::Hover(int skin, int expression, int pixelHeight) {
    const int h = std::max(4, pixelHeight);
    Gdiplus::Bitmap* src = Source(skin, expression);
    if (!src) return nullptr;
    const int w = std::max(
        4, static_cast<int>(std::lround(static_cast<double>(h) * src->GetWidth() /
                                        static_cast<double>(src->GetHeight()))));
    const std::wstring key = Format(L"h-%d-%d-%d", skin, expression, h);
    for (auto& entry : cache_) {
        if (entry.first == key) return entry.second;
    }
    Gdiplus::Bitmap* scaled = new Gdiplus::Bitmap(w, h, PixelFormat32bppPARGB);
    if (scaled->GetLastStatus() != Gdiplus::Ok) {
        delete scaled;
        return nullptr;
    }
    {
        Gdiplus::Graphics g(scaled);
        g.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
        g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
        g.DrawImage(src, Gdiplus::Rect(0, 0, w, h), 0, 0, static_cast<int>(src->GetWidth()),
                    static_cast<int>(src->GetHeight()), Gdiplus::UnitPixel);
    }
    cache_.emplace_back(key, scaled);
    Remember(key);
    return scaled;
}

Gdiplus::Bitmap* ArtworkStore::Perch(int skin, int expression, int pixelWidth,
                                     int pixelHeight) {
    const int w = std::max(4, pixelWidth);
    const int h = std::max(4, pixelHeight);
    Gdiplus::Bitmap* rotated = Rotated90(skin, expression);
    if (!rotated) return nullptr;

    const SkinDef& s = SkinAt(skin);
    const float srcW = static_cast<float>(SourceWidth(skin));
    const float srcH = static_cast<float>(SourceHeight(skin));
    // Crop the head band out of the rotated image.
    const float cropX = (1.0f - s.headY1) * srcH;
    const float cropY = s.headX0 * srcW;
    const float cropW = (s.headY1 - s.headY0) * srcH;
    const float cropH = (s.headX1 - s.headX0) * srcW;

    const std::wstring key = Format(L"p-%d-%d-%d", skin, expression, w);
    for (auto& entry : cache_) {
        if (entry.first == key) return entry.second;
    }

    Gdiplus::Bitmap* scaled = new Gdiplus::Bitmap(w, h, PixelFormat32bppPARGB);
    if (scaled->GetLastStatus() != Gdiplus::Ok) {
        delete scaled;
        return nullptr;
    }
    {
        Gdiplus::Graphics g(scaled);
        g.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
        g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
        g.DrawImage(rotated, Gdiplus::RectF(0, 0, static_cast<float>(w),
                                            static_cast<float>(h)),
                    cropX, cropY, cropW, cropH, Gdiplus::UnitPixel);
    }
    cache_.emplace_back(key, scaled);
    Remember(key);
    return scaled;
}

void ArtworkStore::Purge() {
    for (auto& entry : cache_) delete entry.second;
    cache_.clear();
    for (int s = 0; s < 2; ++s) {
        for (int e = 0; e < 7; ++e) {
            delete rotated_[s][e];
            rotated_[s][e] = nullptr;
            delete sources_[s][e];
            sources_[s][e] = nullptr;
        }
    }
}

}  // namespace bfy
