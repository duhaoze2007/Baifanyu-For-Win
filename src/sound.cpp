#include "sound.h"

#include <mmsystem.h>

namespace bfy {

DuckSound& DuckSound::Get() {
    static DuckSound inst;
    return inst;
}

void DuckSound::Init(HINSTANCE hinst) { hinst_ = hinst; }

void DuckSound::Prepare() {
    if (prepared_) return;
    prepared_ = true;
    for (int id : {IDR_DUCK1, IDR_DUCK2, IDR_DUCK3}) {
        std::vector<BYTE> bytes = LoadResourceBytes(hinst_, id);
        if (!bytes.empty()) samples_.push_back(std::move(bytes));
    }
}

void DuckSound::Squeak() {
    Prepare();
    if (samples_.empty()) return;
    const double now = NowSeconds();
    if (now - lastPlay_ <= 0.06) return;
    lastPlay_ = now;

    // PlaySound with SND_MEMORY plays straight out of the embedded resource —
    // no temporary files, nothing written to disk.  The buffers live for the
    // whole process, which is what SND_ASYNC requires.
    const std::vector<BYTE>& sample = samples_[next_ % samples_.size()];
    ++next_;
    PlaySoundW(reinterpret_cast<LPCWSTR>(sample.data()), nullptr,
               SND_MEMORY | SND_ASYNC | SND_NODEFAULT);
}

}  // namespace bfy
