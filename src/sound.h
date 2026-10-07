// BaifanYu (白饭鱼) for Windows — the rubber-duck squeaks.
#ifndef BAIFANYU_SOUND_H
#define BAIFANYU_SOUND_H

#include "common.h"

namespace bfy {

class DuckSound {
  public:
    static DuckSound& Get();

    void Init(HINSTANCE hinst);
    /// Load the three samples so the first squeak is not late.
    void Prepare();
    bool Available() const { return !samples_.empty(); }

    /// One squeak, at most one every 60 ms so a click storm isn't noise
    /// (the macOS port's DuckSound.squeak()).
    void Squeak();

  private:
    DuckSound() = default;

    HINSTANCE hinst_ = nullptr;
    bool prepared_ = false;
    std::vector<std::vector<BYTE>> samples_;
    size_t next_ = 0;
    double lastPlay_ = 0;
};

}  // namespace bfy

#endif  // BAIFANYU_SOUND_H
