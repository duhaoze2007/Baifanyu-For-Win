// BaifanYu (白饭鱼) for Windows — persistent settings.
//
// Everything lives under HKCU\Software\BaifanYu, so she needs no installer and
// no admin rights.  Paths are used as REG_SZ, numbers as REG_DWORD.
#ifndef BAIFANYU_SETTINGS_H
#define BAIFANYU_SETTINGS_H

#include "common.h"
#include "l10n.h"

namespace bfy {

struct Settings {
    // Ranges & defaults — identical to the Android app's sliders / the macOS port.
    static constexpr double kMinHoverHeight = 120.0, kMaxHoverHeight = 480.0,
                            kDefaultHoverHeight = 240.0;
    static constexpr double kMinPerchWidth = 56.0, kMaxPerchWidth = 160.0,
                            kDefaultPerchWidth = 96.0;
    static constexpr double kMinAmplitude = 0.30, kMaxAmplitude = 1.50,
                            kDefaultAmplitude = 0.75;

    double hoverHeight = kDefaultHoverHeight;  // pt (dp on Android)
    double perchWidth = kDefaultPerchWidth;
    double amplitude = kDefaultAmplitude;

    bool soundOn = true;
    bool wanderOn = true;
    bool randomFace = true;

    int skinIndex = 0;
    bool running = true;     // she is on screen (vs. called off)
    bool perched = false;
    int perchedEdge = 0;     // 0 = left, 1 = right
    double windowX = 0, windowY = 0;
    bool hasPosition = false;

    Lang language = Lang::System;
    bool firstRunDone = false;

    void Load();
    void Save() const;
    /// Only the things that move; called when a drag ends.
    void SavePosition() const;
};

// ---------------------------------------------------------------- autostart

bool LaunchAtLoginEnabled();
bool SetLaunchAtLogin(bool enabled);

}  // namespace bfy

#endif  // BAIFANYU_SETTINGS_H
