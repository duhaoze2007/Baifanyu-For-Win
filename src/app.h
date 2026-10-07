// BaifanYu (白饭鱼) for Windows — the application controller.
//
// Port of the macOS PetController: owns her live state, the notification-area
// icon, edge snapping, the wander loop, the random expressions and the hover
// bubble.  Everything the settings window reads goes through here.
#ifndef BAIFANYU_APP_H
#define BAIFANYU_APP_H

#include "aboutwin.h"
#include "artwork.h"
#include "bubble.h"
#include "common.h"
#include "l10n.h"
#include "pet.h"
#include "settings.h"
#include "settingswin.h"

namespace bfy {

class App : public PetWindow::Delegate {
  public:
    static App& Get();

    /// Messages a second copy can post to the running one.
    static constexpr UINT kMsgShowPet = WM_APP + 2;
    static constexpr UINT kMsgOpenSettings = WM_APP + 3;
    static constexpr UINT kMsgOpenAbout = WM_APP + 4;

    /// How the app is allowed to start.
    enum class StartResult { Ok, AlreadyRunning, Declined };

    /// Returns AlreadyRunning when another copy is already up (in which case
    /// that copy has been asked to bring her out), and Declined when the user
    /// turned down the first-launch notices.
    StartResult Init(HINSTANCE hinst);
    int Run();

    Settings& settings() { return settings_; }

    // ---- state the settings window reads ---------------------------------
    bool PetRunning() const { return running_; }
    bool PetPerched() const { return perched_; }
    int CurrentSkin() const { return settings_.skinIndex; }
    int CurrentExpression() const { return expression_; }
    int LoadedExpressionCount() const;
    std::wstring PetStateText() const;

    // ---- actions ---------------------------------------------------------
    void ShowPet();
    void HidePet();
    void TogglePet();
    void CycleExpression();
    void TestFace();
    void SelectSkin(int index);
    void ApplySizeChange();
    void ApplyBehaviourChange();
    void OnLanguageChanged();
    void OpenSettings();
    void OpenAbout();
    void Quit();

    // ---- PetWindow::Delegate --------------------------------------------
    void OnPetDrag(int windowX, int windowY) override;
    void OnPetBeginDrag() override;
    void OnPetEndDrag(POINT screenPoint) override;
    void OnPetTapped() override;
    void OnPetMenu(POINT screenPoint) override;

  private:
    App() = default;

    // window plumbing
    static LRESULT CALLBACK MainProc(HWND, UINT, WPARAM, LPARAM);
    LRESULT HandleMain(HWND hwnd, UINT message, WPARAM wp, LPARAM lp);
    bool CreateMainWindow();

    // notification area
    void AddTrayIcon();
    void RemoveTrayIcon();
    void UpdateTrayTip();
    void ShowTrayMenu(POINT pt);
    void ShowPetMenu(POINT pt);
    void HandleMenuCommand(int id);

    // frame loop
    void OnFrame();
    void OnWatchdog();
    void SetAnimating(bool on);
    bool ShouldAnimate();

    // geometry
    RECT MonitorRect() const;
    RECT WorkRect() const;
    POINT ClampOrigin(int x, int y, int w, int h) const;
    RECT PetRect() const;
    POINT PetRectCenter() const;
    void MovePet(int x, int y, int w, int h);
    void ApplyLayout();
    void EnterPerch(bool onRight, bool persist);
    void ExitPerch();
    void RememberPosition();
    float Scale() const;

    // pointer / hover / bubble
    void UpdatePointerState();
    void SetHovering(bool on);
    void UpdateBubble(double now);
    void HideBubble();

    // idle behaviour
    void UpdateWander(double now);
    void UpdateRandomFace(double now);
    void ScheduleNextRandomFace(double now);
    int RandomExpression(int differentFrom) const;

    void RunFirstLaunchDialogs();

    HINSTANCE hinst_ = nullptr;
    HWND mainWnd_ = nullptr;
    HANDLE mutex_ = nullptr;
    bool ownsMutex_ = false;

    Settings settings_;
    PetWindow pet_;
    HoverBubble bubble_;
    SettingsWindow settingsWin_;
    AboutWindow aboutWin_;

    bool running_ = false;
    bool perched_ = false;
    bool perchedRight_ = false;
    int expression_ = 0;

    bool animating_ = false;
    bool displayOff_ = false;
    bool sessionLocked_ = false;

    // Idle wandering — the same numbers as the Android wander loop.
    double wanderTargetX_ = -1;
    double wanderX_ = 0;
    double wanderWaitUntil_ = 0;
    static constexpr double kWanderMargin = 14.0;
    static constexpr double kWanderStep = 4.0;

    // Random expressions.
    double nextFaceChangeAt_ = 0;

    // Hover.
    bool hovering_ = false;
    double hoverBeganAt_ = 0;
    int lastBubbleSecond_ = -1;
    std::wstring lastBubbleSentence_;
    RECT lastBubbleAnchor_{};

    HICON trayIcon_ = nullptr;
    HPOWERNOTIFY powerNotify_ = nullptr;
    bool sessionNotify_ = false;
    UINT taskbarCreatedMsg_ = 0;
};

}  // namespace bfy

/// Hand a request to the copy of her that is already running.
bool SignalRunningInstance(unsigned int message);

#endif  // BAIFANYU_APP_H
