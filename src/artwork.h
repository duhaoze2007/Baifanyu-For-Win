// BaifanYu (白饭鱼) for Windows — skins and artwork.
//
// Port of the macOS PetSkin / PetBitmap / PetArtworkStore.  The full-resolution
// PNGs live in the executable's resources; they are decoded once on demand and
// then pre-scaled (and, while perched, pre-rotated) so a 30 Hz frame is a plain
// blit instead of a 5x downscale of a 1 MB PNG.
#ifndef BAIFANYU_ARTWORK_H
#define BAIFANYU_ARTWORK_H

#include "common.h"
#include "l10n.h"

namespace bfy {

/// One skin = standing art + 6 expressions + how to crop her head when she
/// perches on a screen edge.  The crop ratios are measured on the original
/// artwork and differ per skin: the bowl of the bowl-head skin is much wider
/// than the maid's head, so one shared set of ratios would slice it off.
struct SkinDef {
    int index;
    int resBase;         // IDR_SKIN_*_BASE
    const wchar_t* prefix;
    S nameKey;
    float headX0, headX1, headY0, headY1;  // fractions of width / height
};

const SkinDef& SkinAt(int index);
int SkinCount();
/// Expression order, identical to the Android app's SKINS[].faces.
const wchar_t* FaceName(int oneBasedIndex);

class ArtworkStore {
  public:
    static ArtworkStore& Get();

    void Init(HINSTANCE hinst) { hinst_ = hinst; }

    /// Does this expression's PNG exist in the executable?
    bool HasArtwork(int skin, int expression) const;
    /// How many of the 6 expressions of a skin actually load.
    int AvailableExpressionCount(int skin) const;

    /// The original full-resolution standing / expression image (cached).
    Gdiplus::Bitmap* Source(int skin, int expression);
    int SourceWidth(int skin);
    int SourceHeight(int skin);
    float SourceAspect(int skin);
    /// drawnHeadHeight / drawnHeadWidth for the perched pose.
    float PerchHeadAspect(int skin);

    /// Her whole body, scaled to `pixelHeight`.
    Gdiplus::Bitmap* Hover(int skin, int expression, int pixelHeight);
    /// Her head only, rotated 90 degrees, scaled into pixelWidth x pixelHeight.
    Gdiplus::Bitmap* Perch(int skin, int expression, int pixelWidth, int pixelHeight);

    /// Drop every decoded and cached bitmap (used on a skin switch).
    void Purge();

  private:
    ArtworkStore() = default;

    Gdiplus::Bitmap* Rotated90(int skin, int expression);
    void Remember(const std::wstring& key);

    HINSTANCE hinst_ = nullptr;
    Gdiplus::Bitmap* sources_[2][7] = {{nullptr}};
    Gdiplus::Bitmap* rotated_[2][7] = {{nullptr}};
    std::vector<std::pair<std::wstring, Gdiplus::Bitmap*>> cache_;  // FIFO, capped
};

}  // namespace bfy

#endif  // BAIFANYU_ARTWORK_H
