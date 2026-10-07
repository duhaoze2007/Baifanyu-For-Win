# 白饭鱼 / BaifanYu v1.0.0 — Windows

**A rice-eating whale girl who lives on your Windows desktop.**
**Native C++ · Offline · No account · MIT code · one self-contained `.exe`**

**一只吃白饭的鲸鱼娘，住在你的 Windows 桌面上。原生 C++ · 完全离线 · 不要账号 · MIT 代码 · 单文件 `.exe`**

| | |
|---|---|
| Download / 下载 | `BaifanYu.exe` (attached below / 见下方附件) |
| Requirements / 系统要求 | Windows 10 or later, 64-bit / Windows 10 或更高版本（64 位） |
| Artifact / 产物 | one ~13 MB `.exe` — no installer, no runtime, no admin rights / 一个约 13 MB 的 exe，免安装、免运行库、不要管理员权限 |
| License / 许可 | MIT (code) · artwork is community fan art, non-commercial only / MIT（代码）· 美术资源仅限个人非商业使用 |

---

## English

### What she is

She floats on your desktop as a tiny borderless window — always on top, never in your taskbar,
never stealing focus from what you are typing in. Drag her anywhere; drop her at the left or right
edge of the screen and she hangs there with only her head showing, so she never blocks what you are
looking at. Click her for another expression and a rubber-duck squeak. Rest the pointer on her and
she stops, showing a bubble with the date, a live clock and a random kind word.

### Highlights

- **Floats above everything**, including full-screen apps — no taskbar button, her face lives in the
  notification area
- **Drag** her anywhere with a tilt in the direction you are dragging; **perch** on either screen edge
  (head rotated 90°, cropped to her head)
- **Two skins** — bowl-head (饭盆头) and maid outfit (女仆装) — **6 expressions** each, cycled by clicking
- **Random expressions** — she changes face by herself every 18–75 s
- **Hover bubble** — today's date, a live clock and one of 16 kind words per language
- **Idle wandering**, paused the moment you hover, touch, drag or perch her (the original's timings)
- **Three sliders** — floating height (120–480 pt), perched head width (56–160 pt), amount of motion (30–150 %)
- **Per-pixel click-through** — her transparent margin never swallows a click meant for the window behind her
- **Notification-area resident** with launch-at-login, and a right-click menu on her body
- **Trilingual UI** — English / 简体中文 / 繁體中文, follows the system or a manual override
- **Per-monitor DPI aware** — she is the same size at 100 %, 150 % and 200 %
- **Stops rendering when you cannot see her** — display off, session locked, or she is covered
- **100 % offline** — no network code at all: no analytics, no tracking, no account, no uploads
- **Zero third-party dependencies** — native C++ / Win32 + GDI+, everything bundled into one `.exe`

### Run it

1. Download `BaifanYu.exe` below and put it anywhere you like
2. Double-click it — her face appears by the clock and she floats on the desktop
3. The first launch shows the copyright notice and the privacy notice once

### How to play

- **Hold her and drag** anywhere; **drag to the left/right edge and let go** to hang her there
- **Click her** for another expression (and a squeak)
- **Rest the pointer on her** — she stops, and a bubble shows the date, the time and a kind word
- **Right-click her** (or press and hold) for skin / next expression / settings / call her back
- The **notification-area fish** is home base: bring her out or call her back, switch skin, toggle the
  squeak / wandering / random expressions / launch at login, Settings, About, Quit
- `BaifanYu.exe --settings` and `BaifanYu.exe --about` open those windows directly

### Verified on a real desktop, not just "it compiles"

- click → expression swaps · perch → head-only crop with correct proportions · hover → bubble with a
  live clock
- notification-area icon and its full localized menu · settings window · about window · first-run notices
- 150 % DPI: 240 pt → 360 px artwork, 296 × 416 window · clean build from scratch in ~50 s
- `objdump` confirms the only imported DLLs are Windows system libraries

### Notes

- **One deliberate difference from the macOS port.** It sizes the perched head with `cropH/cropW`,
  which is the *reciprocal* of the rotated crop's real aspect and stretches her head sideways. This
  release uses the Android app's own ratio instead, so the perched proportions match Android. It is
  the only place this port does not match macOS numerically.
- The duck squeak uses `PlaySound`, which plays one sample at a time; macOS's `AVAudioPlayer` pool
  layered them. The 60 ms throttle makes the difference inaudible.
- Occlusion is a 1-second `WindowFromPoint` poll — Windows has no equivalent of `occlusionState`.
- The settings window uses the light theme; only the hover bubble follows Windows' dark mode.
- Inherited from the original: the six expressions were generated in separate passes, so her hair edge
  can shift by a pixel or two when she changes face.

### Privacy

Completely offline. No network code at all. Her position, size, expressions and sounds live in
`HKCU\Software\BaifanYu` on this PC only — deleting that key resets her completely.

### Credits & license

A Windows port of [LIN428924379/baifanyu](https://github.com/LIN428924379/baifanyu) (Android), keeping
the full feature set of [duhaoze2007/BaifanYu-For-Mac](https://github.com/duhaoze2007/BaifanYu-For-Mac).

**Thank you, [@LIN428924379](https://github.com/LIN428924379)** — the concept, the character, the
artwork and the interaction design are all yours.

| Part | Origin |
|---|---|
| Original app, concept, interaction design | LIN428924379/baifanyu (MIT) |
| Standing art & 6 expressions per skin | the original author's assets — community fan art, **not** MIT |
| Duck squeaks | Mixkit (Mixkit Free License) |
| Windows code (C++ / Win32 / GDI+) | this repository, MIT, © 2026 Du Haoze |

See [RELEASE_NOTES.md](https://github.com/duhaoze2007/BaifanYu-For-Win/blob/main/RELEASE_NOTES.md) for the
full version-by-version notes and the Android ↔ macOS ↔ Windows comparison table.

---

## 中文

### 她是什么

她以一个极小的无边框窗口浮在你的桌面上 —— 永远在最上层，不占任务栏，点她也不会抢走你正在打字那个窗口的
焦点。按住她可以拖到任意位置；拖到屏幕左右边缘松手，她就扒在边上、只露一个脑袋，绝不挡住你看的东西。
点她一下换一个表情，顺便吱一声（小黄鸭音效）。鼠标停在她身上她就不动了，旁边浮出一个小气泡，写着今天的
日期、实时走秒的时钟和一句随机的关心话。

### 功能亮点

- **浮在所有窗口之上**，全屏应用上也照样在 —— 没有任务栏按钮，她的脸就挂在右下角通知区
- **拖拽**任意移动，拖动时朝拖动方向倾斜；**扒边** —— 拖到屏幕左右边缘松手，头旋转 90°、只露脑袋
- **两套皮肤** —— 饭盆头 / 女仆装 —— 每套 **6 个表情**，点她循环切换
- **随机换表情** —— 闲着的时候她自己每隔 18~75 秒换一个
- **悬停气泡** —— 今天的日期、实时走秒的时钟，以及每种语言 16 句随机关心话
- **自动溜达** —— 鼠标一放到她身上、碰她、拖她、扒边就立刻暂停（用的是原作者定的节拍）
- **三档调节** —— 悬空大小（120–480 pt）、扒边时脑袋宽度（56–160 pt）、动态幅度（30–150 %）
- **像素级点击穿透** —— 她周围的透明边距不会吞掉本该给后面窗口的点击
- **通知区常驻**，支持登录时自动启动，右键她身上还能弹出快捷菜单
- **三语界面** —— English / 简体中文 / 繁體中文，跟随系统或手动指定
- **每显示器 DPI 感知** —— 100 % / 150 % / 200 % 缩放下她看起来一样大
- **看不见她的时候真的停止渲染** —— 屏幕关闭、会话锁定、或者她被别的窗口盖住时
- **完全离线** —— 无网络代码、无统计、无追踪、不要账号、不上传
- **零第三方依赖** —— 原生 C++ / Win32 + GDI+，所有资源都打进这一个 `.exe` 里

### 怎么跑起来

1. 下载下方的 `BaifanYu.exe`，放到你想放的地方
2. 双击运行 —— 她的脸出现在右下角时钟旁边，同时浮在桌面上
3. 首次启动会展示一次版权声明与隐私声明

### 怎么玩

- **按住她拖**到任意位置；**拖到屏幕左右边缘松手**，她就扒在边上
- **点她一下**换一个表情（顺便吱一声）
- **鼠标停在她身上**：她停下不动，旁边浮出日期、时间和一句关心话
- **右键点她**（或长按）弹出：皮肤 / 换表情 / 设置 / 收回她
- **右下角通知区的小鱼**是她的家：让她出现或收回、换皮肤、开关音效 / 溜达 / 随机换表情 / 登录自启、
  设置、关于、退出
- `BaifanYu.exe --settings` 和 `BaifanYu.exe --about` 可以直接打开这两个窗口

### 在真实桌面上验证过，不只是「能编译」

- 点击 → 换表情 · 扒边 → 只露脑袋且比例正确 · 悬停 → 气泡里时钟真的在走
- 通知区图标及其中文菜单 · 设置窗口 · 关于窗口 · 首次启动的两份声明
- 150 % 缩放：240 pt → 360 px 立绘，窗口 296 × 416 · 从零干净构建约 50 秒
- `objdump` 确认导入表里只有 Windows 系统 DLL

### 说明

- **有一处是刻意和 macOS 版不一样的。** macOS 版用 `cropH/cropW` 来定扒边脑袋的尺寸，而这个值是旋转后
  裁切块真实宽高比的 *倒数*，会把脑袋横向拉长。本版本改用 Android 原版自己的比例，所以扒边时的比例和
  Android 一致。这是本移植唯一一处数值上不与 macOS 版一致的地方。
- 鸭叫用 `PlaySound`，同一时刻只能播一个样本；macOS 版用 `AVAudioPlayer` 池可以叠加。60 ms 的节流让这个
  差别听不出来。
- 遮挡判定是每秒轮询一次 `WindowFromPoint` —— Windows 没有 `occlusionState` 这种东西。
- 设置窗口用的是浅色主题，只有悬停气泡跟随 Windows 深色模式。
- 继承自原版：6 张表情立绘是分两次独立生成的，换表情时头发边缘可能有一两个像素的抖动。

### 隐私

完全离线，没有任何网络代码。她的位置、大小、表情和音效只存在这台电脑的
`HKCU\Software\BaifanYu` 里 —— 删掉这个注册表项就等于完全重置她。

### 致谢与许可

本版本是 [LIN428924379/baifanyu](https://github.com/LIN428924379/baifanyu)（Android）的 Windows 移植，
并完整保留了 [duhaoze2007/BaifanYu-For-Mac](https://github.com/duhaoze2007/BaifanYu-For-Mac) 的功能集。

**感谢原作者 [@LIN428924379](https://github.com/LIN428924379)** —— 创意、角色、立绘和交互设计全部属于你。

| 部分 | 出处 |
|---|---|
| 原应用、创意、交互设计 | LIN428924379/baifanyu（MIT） |
| 立绘与每套 6 个表情 | 原作者的素材 —— 社区同人创作，**不在 MIT 范围内** |
| 小黄鸭音效 | Mixkit（Mixkit Free License） |
| Windows 代码（C++ / Win32 / GDI+） | 本仓库，MIT，© 2026 Du Haoze |

完整的版本说明和 Android ↔ macOS ↔ Windows 对照表见
[RELEASE_NOTES.md](https://github.com/duhaoze2007/BaifanYu-For-Win/blob/main/RELEASE_NOTES.md)。
