// BaifanYu (白饭鱼) for Windows — trilingual UI strings + the hover bubble's
// kind words.  Port of the macOS LocalizationManager.
#ifndef BAIFANYU_L10N_H
#define BAIFANYU_L10N_H

#include "common.h"

namespace bfy {

enum class Lang { System = 0, En, ZhHans, ZhHant };

enum class S {
    // Identity
    AppName, Tagline, Subtitle,
    // Tray menu
    ShowHer, HideHer, NextFace, SkinMenu, Settings, About, Quit, LaunchAtLogin,
    // Skins
    SkinBasin, SkinMaid,
    // Settings — size & motion
    SizeSection, SizeHint, HoverSize, PerchWidthLabel, AmplitudeLabel,
    // Settings — behaviour
    BehaviourSection, WanderTitle, WanderHint, SoundTitle, SoundHint,
    RandomFaceTitle, RandomFaceHint,
    // Settings — skin
    SkinSection, SkinHint, TapToSwitch,
    // Settings — actions
    ShowButton, HideButton, TestFaceButton, SheIsRunning, SheIsResting,
    // Status
    StatusSection, StatusRunning, StatusReady, StateHover, StatePerchLeft, StatePerchRight,
    // How to play
    HowToTitle, HowToBody,
    // About
    AboutBody, VersionLabel, Copyright, MitLicense, Language, FollowSystem,
    CreditsTitle, CreditsBody, LicenseTitle, LicenseBody,
    // First launch
    FirstRunCopyrightTitle, FirstRunCopyrightBody, FirstRunPrivacyTitle, FirstRunPrivacyBody,
    Agree, Disagree,
    // Misc
    PrivacyTitle, PrivacyBody, LoginItemFailed, NotesTitle, NotesBody,
};

class L10n {
  public:
    static L10n& Get();

    void Init();

    Lang language() const { return language_; }
    void SetLanguage(Lang lang);

    /// The language actually in use (resolves `System`).
    Lang Resolved() const;

    std::wstring T(S key) const;

    /// "Artwork loaded: 6 / 6 expressions"
    std::wstring FacesLoaded(int n) const;

    /// A different kind word each time she is hovered.
    std::wstring RandomCaringSentence(const std::wstring& previous) const;

    std::wstring FullDate(const SYSTEMTIME& st) const;
    std::wstring MediumTime(const SYSTEMTIME& st) const;

  private:
    L10n() = default;
    Lang language_ = Lang::System;
};

/// Human-readable language name for the settings combo box.
std::wstring LanguageDisplayName(Lang lang);

/// The language the OS is set to (zh-Hant locales resolve to Traditional).
Lang DetectSystemLanguage();

}  // namespace bfy

#endif  // BAIFANYU_L10N_H
