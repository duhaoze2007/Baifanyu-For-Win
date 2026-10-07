# 白饭鱼 / BaifanYu for Windows v1.0.0 — Release Notes / 发布说明

| Item / 项目 | Detail / 详情 |
|---|---|
| Version / 版本 | 1.0.0 (first Windows release / 首个 Windows 版本) |
| Release date / 发布日期 | October 7, 2026 / 2026 年 10 月 7 日 |
| Artifact / 产物 | `BaifanYu.exe` — one self-contained file, nothing to install / 单文件、免安装 |
| Requirements / 系统要求 | Windows 10 or later, 64-bit / Windows 10 或更高版本，64 位 |
| Size / 体积 | ~13 MB (all artwork, sounds and the icon are inside the .exe) |
| License / 许可 | MIT (code) — artwork is community fan art, non-commercial only / MIT（代码）—— 美术资源为社区同人创作，仅限非商业使用 |
| Ported from / 移植自 | [LIN428924379/baifanyu](https://github.com/LIN428924379/baifanyu) — Android 0.7, keeping the feature set of the [macOS port](https://github.com/duhaoze2007/BaifanYu-For-Mac) |

---

## English

**白饭鱼 / BaifanYu** — a rice-eating whale girl who lives on your Windows desktop. A tiny
borderless window floats above everything: drag her anywhere, drop her at the left or right edge of
the screen and she hangs there with only her head showing, so she never blocks what you are looking
at.

> **Thank you, [@LIN428924379](https://github.com/LIN428924379).** This is a port of your Android app
> [白饭鱼 / baifanyu](https://github.com/LIN428924379/baifanyu). The concept, the character, the
> artwork and the interaction design are all yours — this release only re-implements the code for
> Windows. 感谢原作者把《白饭鱼》开源出来。

### What's new — the Windows version

This is the first Windows release, so everything is new. It keeps the whole Android behaviour set and
every feature the macOS port added:

- 🖱️ **Drag** her anywhere — she tilts in the direction you are dragging
- 📌 **Perch on a screen edge** — drag her to the left or right edge and let go: she hangs there,
  head rotated 90°, only her head visible; drag her back in to make her float again
- 🙂 **Six expressions** per skin, swapped by clicking her (the whole artwork swaps, so there are no
  seams)
- 🎲 **Random expressions** — while she idles she changes face by herself every 18–75 s
- 💬 **Hover bubble** — rest the pointer on her and she stops wandering while a small bubble shows
  today's date, a live clock and a random kind word (16 per language)
- 🛑 **Wandering pauses when you hover her** and resumes the moment the pointer leaves
- 👗 **Two skins**: bowl-head (饭盆头) and maid outfit (女仆装)
- 🦆 **Rubber-duck squeak** on click — three real duck samples, can be turned off
- 🚶 **Idle wandering** — she crawls around on her own, and settles when you touch, drag, perch or
  hover her
- 🎚️ **Three sliders**: floating height (120–480 pt), head width while perched (56–160 pt), amount of
  motion (30–150 %)
- 🖱️ **Right-click / press-and-hold menu** on her body: skin · next expression · Settings · call her
  back
- 🧊 **Notification-area resident** — no taskbar button; her face sits by the clock and everything is
  reachable from there
- 👻 **Per-pixel click-through** — the transparent margin around her never swallows a click meant for
  the window behind her
- 🚀 **Launch at login** support
- 🌐 **Trilingual UI**: English / 简体中文 / 繁體中文 (follows the system, or set it manually)
- 🔋 **Stops rendering when you cannot see her** — screen off, session locked, or another window on
  top of her
- 🔒 **100 % offline** — no network code, no analytics, no tracking, no account
- 📜 MIT licensed code, **no third-party dependencies**, one `.exe`
- 🖥️ **Per-monitor DPI aware**

### What changed from the Android app and the macOS port

| Android original | macOS port | This Windows port |
|---|---|---|
| Foreground service + `TYPE_APPLICATION_OVERLAY` | `NSPanel` (borderless, non-activating, floating, all Spaces) | `WS_POPUP` + `WS_EX_LAYERED \| WS_EX_TOPMOST \| WS_EX_TOOLWINDOW \| WS_EX_NOACTIVATE`, `UpdateLayeredWindow` for real per-pixel alpha |
| "Grant *display over other apps*" | not needed | not needed |
| Persistent notification with 收回 / 设置 | menu bar dropdown | notification-area icon with the same menu |
| Settings screen in the app | Settings window (⌘,) + native About | Settings window + About window, reachable from the tray menu or `--settings` / `--about` |
| "Add to autostart, no battery restrictions" advice | launch-at-login toggle | `HKCU\...\CurrentVersion\Run` toggle |
| — | per-pixel click-through | `WS_EX_TRANSPARENT` toggled from the alpha of the pixel under the pointer |
| — | hover bubble + random expressions | the same |
| dp-sized | pt-sized | pt × monitor DPI, so she looks the same size at 100 %, 150 %, 200 % |
| Stopped rendering on window visibility | occlusion + screen sleep + session lock | console-display-state and session-lock notifications, plus a `WindowFromPoint` occlusion poll |

The animation maths, the edge-snap threshold, the head-only perch crop and the wander timings are a
direct port of the original — the owner's numbers, reproduced on Windows.

### Installation

There is none.

1. Put `BaifanYu.exe` wherever you like
2. Run it — her face appears in the notification area and she floats on the desktop
3. The first launch shows the copyright notice and the privacy notice once
4. (Optional) Turn on **Launch at login** from her tray menu

She needs no installer, no runtime, no admin rights and no permission prompts. Her settings live in
`HKCU\Software\BaifanYu`; deleting that key resets her completely.

### How to use

- **Hold her and drag** anywhere; **drag to the left/right screen edge and let go** to hang her there
- **Click her** for another expression (and a squeak)
- **Rest the pointer on her** — she stops, and a bubble shows the date, the time and a kind word
- **Right-click her** (or press and hold) for skin / next expression / settings / call her back
- The **notification-area fish** is home base: bring her out or call her back, switch skin, toggle
  squeak / wandering / random expressions / launch at login, Settings, About, Quit
- `BaifanYu.exe --settings` and `BaifanYu.exe --about` open those windows directly

### Privacy

Completely offline. No network code at all — no analytics, no tracking, no account, no uploads. Her
position, size, expressions and sounds are stored in your own registry hive
(`HKCU\Software\BaifanYu`) on this PC only.

### Known limitations

- Inherited from the original: the six expressions were generated in separate passes, so the hair edge
  can shift by a pixel or two when she changes face, and her hair does not swing (no mesh
  deformation — only whole-body transforms)
- **Perched geometry, deliberately fixed.** The macOS port sizes the perched head with
  `cropH/cropW` — the *reciprocal* of the rotated crop's real aspect — which stretches the head
  sideways. This port uses the Android app's own ratio (`(hx1-hx0)·srcW / ((hy1-hy0)·srcH)`), so the
  perched head has the same proportions as on Android. Verified against both source trees; noted here
  because it is the one place this port does not match macOS numerically.
- The duck squeak uses `PlaySound`, which plays one sample at a time: poking her faster than 60 ms
  apart restarts the sample instead of layering it. The macOS port's `AVAudioPlayer` pool overlapped
  them; in practice the 60 ms throttle makes this inaudible.
- Occlusion is detected by polling `WindowFromPoint` once a second (Windows has no
  `occlusionState`), so she may keep animating for up to a second after being fully covered.
- The settings window uses the light theme; only the hover bubble follows the Windows dark mode.
- Inherited from the original's rule: everything is written with whole-body transforms, so she is
  never deformed, merely moved, tilted and squashed.

### Credits & license

- **Original Android app**: [LIN428924379/baifanyu](https://github.com/LIN428924379/baifanyu) — MIT,
  © 2026 LIN428924379. Concept, character, artwork and interaction design all belong to the original
  author
- **macOS port**: [duhaoze2007/BaifanYu-For-Mac](https://github.com/duhaoze2007/BaifanYu-For-Mac) —
  whose feature set this port keeps in full
- **Artwork** (standing art + 6 expressions per skin): community fan art from the original project,
  **not** covered by MIT — personal, non-commercial use only
- **Duck squeaks**: [Mixkit](https://mixkit.co/free-sound-effects/duck/) (Mixkit Free License)
- **Windows code**: MIT, © 2026 Du Haoze — see [LICENSE](LICENSE)

---

## 中文

**白饭鱼 / BaifanYu** —— 一只吃白饭的鲸鱼娘，住在你的 Windows 桌面上。她以一个极小的无边框窗口浮在所有
东西之上：按住她能拖到任意位置，拖到屏幕左右边缘松手，她就扒在边上、只露一个脑袋，绝不挡住你看的东西。

> **感谢原作者 [@LIN428924379](https://github.com/LIN428924379)。** 本版本是 Android 应用[《白饭鱼》](https://github.com/LIN428924379/baifanyu)的
> Windows 移植。创意、角色形象、立绘和交互设计全部属于原作者，这次只是把代码为 Windows 重写了一遍。

### 新内容 —— Windows 版

这是首个 Windows 版本，所以全部都是新的。它完整保留了原 Android 版的行为，以及 macOS 版加的所有功能：

- 🖱️ **拖拽** —— 拖到哪算哪，拖动时朝拖动方向倾斜
- 📌 **扒边** —— 拖到屏幕左右边缘松手，她扒在边上、头旋转 90°、只露一个脑袋；从边缘往里拖就回到悬空
- 🙂 **每套皮肤 6 个表情**，点她循环切换（整张立绘切换，没有接缝）
- 🎲 **随机换表情** —— 闲着的时候她自己每隔 18~75 秒换一个表情
- 💬 **悬停气泡** —— 鼠标停在她身上，她就不动了，旁边浮出一个小气泡，写着今天的日期、实时走秒的时钟和
  一句随机的关心话（每种语言 16 句）
- 🛑 **鼠标一放到她身上，自动溜达立刻暂停**，指针移开马上继续
- 👗 **两套皮肤**：饭盆头 / 女仆装
- 🦆 **小黄鸭音效** —— 点击时随机播一个真实鸭叫，可关闭
- 🚶 **自动溜达** —— 闲着的时候她自己爬来爬去；碰她、拖她、扒边、鼠标停在她身上时不动
- 🎚️ **三档调节**：悬空大小（120–480 pt）、扒边时脑袋宽度（56–160 pt）、动态幅度（30–150 %）
- 🖱️ **右键 / 长按菜单**：皮肤 · 换表情 · 设置 · 收回她
- 🧊 **通知区常驻** —— 没有任务栏按钮，她的脸就在右下角时钟旁边，所有操作都在那里
- 👻 **像素级点击穿透** —— 她周围的透明边距不会吞掉本该给后面窗口的点击
- 🚀 **支持登录时自动启动**
- 🌐 **三语界面**：English / 简体中文 / 繁體中文（跟随系统，也可手动指定）
- 🔋 **看不见她的时候真的停止渲染** —— 屏幕关闭、会话锁定、或者她上面盖了别的窗口时
- 🔒 **完全离线** —— 无网络代码、无统计、无追踪、不要账号
- 📜 MIT 开源代码，**零第三方依赖**，就一个 `.exe`
- 🖥️ **每显示器 DPI 感知**

### 与 Android 原版、macOS 版的差异

| Android 原版 | macOS 版 | 这个 Windows 版 |
|---|---|---|
| 前台服务 + `TYPE_APPLICATION_OVERLAY` | `NSPanel`（无边框、不抢焦点、悬浮、跨所有空间） | `WS_POPUP` + `WS_EX_LAYERED \| WS_EX_TOPMOST \| WS_EX_TOOLWINDOW \| WS_EX_NOACTIVATE`，用 `UpdateLayeredWindow` 实现真正的逐像素透明 |
| 需要授予「显示在其他应用上层」权限 | 不需要 | 不需要 |
| 常驻通知里的「收回 / 设置」 | 菜单栏下拉 | 通知区图标菜单，动作一致 |
| 应用内设置页 | 独立设置窗口（⌘,）+ 原生「关于」 | 独立设置窗口 + 「关于」窗口，从通知区菜单或 `--settings` / `--about` 打开 |
| 小米/华为/OPPO/vivo 要手动加自启动、关省电限制 | 「登录时自动启动」开关 | 写 `HKCU\...\CurrentVersion\Run` 的开关 |
| — | 像素级点击穿透 | 用指针所在像素的 alpha 切换 `WS_EX_TRANSPARENT` |
| — | 悬停气泡 + 自动换表情 | 一样有 |
| 用 dp 定尺寸 | 用 pt 定尺寸 | pt × 显示器 DPI，所以 100 % / 150 % / 200 % 缩放下她看起来一样大 |
| 靠窗口可见性停渲染 | 遮挡 + 屏幕休眠 + 会话锁定通知 | 显示器开关与会话锁定通知，外加每秒一次的 `WindowFromPoint` 遮挡探测 |

动画数学、边缘吸附阈值、扒边时只画脑袋的裁切、溜达的节拍，都是原版的直接移植 —— 原作者定的数值，在
Windows 上原样复现。

### 安装方法

不需要安装。

1. 把 `BaifanYu.exe` 放到你想放的地方
2. 双击运行 —— 她的脸出现在右下角通知区，同时浮在桌面上
3. 首次启动会展示一次版权声明与隐私声明
4. （可选）从她的通知区菜单打开「登录时自动启动」

免安装、免运行库、不要管理员权限、没有任何权限弹窗。她的设置存在 `HKCU\Software\BaifanYu`，
删掉这个注册表项就等于完全重置。

### 怎么玩

- **按住她拖**到任意位置；**拖到屏幕左右边缘松手**，她就扒在边上
- **点她一下**换一个表情（顺便吱一声）
- **鼠标停在她身上**：她停下不动，旁边浮出日期、时间和一句关心话
- **右键点她**（或长按）弹出：皮肤 / 换表情 / 设置 / 收回她
- **右下角通知区的小鱼**是她的家：让她出现或收回、换皮肤、开关音效 / 溜达 / 随机换表情 / 登录自启、
  设置、关于、退出
- `BaifanYu.exe --settings` 和 `BaifanYu.exe --about` 可以直接打开这两个窗口

### 隐私

完全离线。整个程序没有任何网络代码 —— 无统计、无追踪、不要账号、不上传。她的位置、大小、表情和音效
都只存在这台电脑你自己的注册表项里（`HKCU\Software\BaifanYu`）。

### 已知限制

- 继承自原版：6 张表情立绘是分两次独立生成的，所以换表情时头发边缘可能有一两个像素的抖动；头发是「硬」的，
  不会跟着甩（目前只有整体变换，没有网格形变）
- **扒边比例是刻意修过的。** macOS 版用 `cropH/cropW` 来定扒边脑袋的尺寸，而这个值是旋转后裁切块真实宽高比的
  *倒数*，会把脑袋横向拉长。本移植改用 Android 原版自己的比例
  （`(hx1-hx0)·srcW / ((hy1-hy0)·srcH)`），所以扒边时脑袋的比例和 Android 一致。这一点对着两份源码都核对过，
  写在这里是因为它是本移植唯一一处数值上不与 macOS 版一致的地方。
- 鸭叫用 `PlaySound`，同一时刻只能播一个样本：连续点击快于 60 ms 时会重播而不是叠加。macOS 版用
  `AVAudioPlayer` 池可以叠加；实际上 60 ms 的节流让这一点听不出来。
- 遮挡靠每秒轮询一次 `WindowFromPoint` 判断（Windows 没有 `occlusionState`），所以被完全盖住后她可能还会再动
  最多一秒
- 设置窗口用的是浅色主题，只有悬停气泡跟随 Windows 深色模式
- 继承自原版的规则：一切都是整体变换，她只会移动、倾斜、压扁，不会被形变

### 致谢与许可

- **原 Android 应用**：[LIN428924379/baifanyu](https://github.com/LIN428924379/baifanyu) —— MIT，
  © 2026 LIN428924379。创意、角色、立绘、交互设计全部属于原作者
- **macOS 版**：[duhaoze2007/BaifanYu-For-Mac](https://github.com/duhaoze2007/BaifanYu-For-Mac) ——
  本移植完整保留了它的功能集
- **美术资源**（立绘 + 每套 6 个表情）：来自原项目的社区同人创作，**不在 MIT 许可范围内**，
  仅限个人非商业使用
- **小黄鸭音效**：[Mixkit](https://mixkit.co/free-sound-effects/duck/)（Mixkit Free License）
- **Windows 代码**：MIT，© 2026 Du Haoze —— 见 [LICENSE](LICENSE)
