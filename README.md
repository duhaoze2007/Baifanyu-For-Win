<p align="center">
  <img src="docs/icon.png" width="128" alt="白饭鱼 / BaifanYu">
</p>

<h1 align="center">白饭鱼 / BaifanYu</h1>

<p align="center">
  A rice-eating whale girl who lives on your Windows desktop
  <br>
  <b>Native C++ · Offline · No account · MIT code · One .exe</b>
</p>

<p align="center">
  <i>A Windows port of the Android desktop pet
  <a href="https://github.com/LIN428924379/baifanyu">白饭鱼 / baifanyu</a> by
  <a href="https://github.com/LIN428924379">LIN428924379</a>,
  keeping every feature of the
  <a href="https://github.com/duhaoze2007/Baifanyu-For-Mac">macOS port</a></i>
  <br>
  <b>Thank you, LIN428924379 — the original idea, the character, the artwork and the
  interaction design are all yours. 感谢原作者，这个移植只是把你的作品搬到了 Windows 上。</b>
</p>

---

## What she is / 她是什么

She floats on your desktop as a tiny borderless window. Drag her anywhere; drop her at the left or right screen edge and she hangs there with only her head showing, so she never blocks what you are looking at. Click her for another expression and a rubber-duck squeak. Rest the pointer on her and she stops, showing a bubble with the date, a live clock and a random kind word; leave her alone and she wanders a little and changes expression by herself now and then.

她以一个极小的无边框窗口浮在桌面上。按住她可以拖到任意位置；拖到屏幕左右边缘松手，她会扒在边上、只露一个脑袋，绝不挡住你看的东西。点她一下换一个表情，顺便吱一声（小黄鸭音效）。鼠标停在她身上她就不动了，旁边浮出一个小气泡写着日期、时间和一句随机的关心话；没人理她的时候她会自己爬两下、时不时自己换个表情。

<p align="center">
  <img src="docs/expressions.png" width="760" alt="two skins, standing art + six expressions each">
</p>

---

## Features / 功能

- **Floats above everything** — no taskbar button; her face sits in the notification area
- **Drag** her anywhere — she tilts in the direction you are dragging
- **Perch on a screen edge** — drag her to the left or right edge and let go: she hangs there, head rotated 90°, only her head visible
- **Six expressions** per skin, cycled by clicking her (whole artwork swaps, so no seams)
- **Random expressions** — while she idles she changes face by herself every 18–75 s
- **Rest the pointer on her** and she stops wandering; a bubble appears with today's date, a live clock and a random kind word
- **Two skins**: bowl-head (饭盆头) and maid outfit (女仆装)
- **Rubber-duck squeak** on click, three real duck samples, can be switched off
- **Idle wandering** — she crawls around on her own; she stops when you touch, drag, perch her, or leave the pointer resting on her
- **Three sliders**: floating height, head width while perched, amount of motion
- **Per-pixel click-through** — the transparent margin around her never swallows a click meant for the window behind her
- **Right-click menu** on her body: skin / next expression / settings / call her back
- **Notification-area menu** — bring her out, skin, toggles, settings, about, quit
- **Launch at login** support
- **Stops rendering when you cannot see her** — screen off, session locked, or she is covered
- **Trilingual UI**: English / 简体中文 / 繁體中文 (follows the system or a manual override)
- **Per-monitor DPI aware** — her size in points follows the display scale, like dp on Android
- **100 % offline** — no network code at all: no analytics, no tracking, no account
- **MIT licensed code**, zero third-party dependencies, single self-contained `.exe`

---

- **浮在所有窗口之上** —— 没有任务栏按钮，她的脸就挂在右下角通知区
- **拖拽** —— 拖到哪算哪，拖动时朝拖动方向倾斜
- **扒边** —— 拖到屏幕左右边缘松手，她扒在边上、头旋转 90°、只露一个脑袋
- **每套皮肤 6 个表情**，点她循环切换（整张立绘切换，没有接缝）
- **随机换表情** —— 闲着的时候她自己每隔 18~75 秒换一个表情
- **鼠标停在她身上**：她就不走了，旁边浮出一个小气泡，写今天的日期、实时的时间和一句随机的关心话
- **两套皮肤**：饭盆头 / 女仆装
- **小黄鸭音效**：点击时随机播一个真实鸭叫，可关
- **自动溜达** —— 闲着的时候她自己爬来爬去；碰她、拖她、趴边、鼠标停在她身上时不动
- **三档调节**：悬空大小 / 扒边时脑袋宽度 / 动态幅度
- **像素级点击穿透** —— 她周围的透明边距不会吞掉本该给后面窗口的点击
- **右键菜单**：皮肤 / 换表情 / 设置 / 收回她
- **通知区菜单**：让她出现、换皮肤、各种开关、设置、关于、退出
- **支持登录时自动启动**
- **看不见她的时候真的停止渲染** —— 屏幕关闭、会话锁定、或者她被别的窗口盖住时
- **三语界面**：English / 简体中文 / 繁體中文（跟随系统或手动切换）
- **每显示器 DPI 感知** —— 她的尺寸跟着屏幕缩放走，和 Android 的 dp 一个意思
- **完全离线**：没有任何网络代码，无统计、无追踪、不要账号
- **MIT 开源代码**，零第三方依赖，单文件 `.exe`

---

## Requirements / 系统要求

- Windows 10 or later (64-bit) / Windows 10 或更高版本（64 位）
- **To run**: nothing — no installer, no runtime, no admin rights / **运行**：什么都不用装，免安装、免运行库、不要管理员权限
- **To build**: a MinGW-w64 toolchain (`g++`, `windres`) on `PATH` / **构建**：`PATH` 上要有 MinGW-w64 工具链（`g++`、`windres`）

Detailed version notes, the Android/macOS/Windows comparison table and the known
limitations: **[RELEASE_NOTES.md](RELEASE_NOTES.md)**

版本说明、Android/macOS/Windows 差异对照表和已知限制见 **[RELEASE_NOTES.md](RELEASE_NOTES.md)**。

---

## Build / 构建

```bash
bash build.sh          # → BaifanYu.exe
./BaifanYu.exe
```

`build.bat` is the same thing for cmd.exe / double-click. The script compiles the
C++ sources and runs `windres` over `src/app.rc`, which packs the artwork, the duck
samples, the icon and the DPI manifest into the executable — the result is one
self-contained `.exe` (about 13 MB), the Windows answer to the macOS port's
resource-bundled `.app`.

On MSYS2:

```bash
pacman -S mingw-w64-ucrt-x86_64-gcc      # g++ + windres
bash build.sh
```

`build.sh` builds in release mode by default; `bash build.sh debug` gives you an
unstripped `-Og` build.

> **Note on non-ASCII paths.** `windres` and `ld` shell their helper processes out
> with ANSI command lines, so they cannot cope with a project path that contains
> non-ASCII characters (a Chinese user name, for instance — `gcc` itself is fine).
> When `build.sh` detects that, it mirrors `src/` and `assets/` into an ASCII
> staging directory (`C:\ProgramData\BaifanYu-build` by default), builds there and
> copies the executable back. Set `BAIFANYU_STAGE` to choose the directory.

脚本以 release 模式构建，把立绘、音效、图标和 DPI 清单全部打进 `BaifanYu.exe`，所以产物就是一个约 13 MB 的独立 exe —— 相当于 macOS 版那个把资源打包进去的 `.app`。

> **路径含非 ASCII 字符的说明**：`windres` 和 `ld` 会用 ANSI 命令行去启动子进程，
> 所以项目路径里如果有中文（比如中文用户名）它们会失败（`gcc` 本身没问题）。
> `build.sh` 检测到这种情况时会把 `src/` 和 `assets/` 镜像到一个纯 ASCII 目录
> （默认 `C:\ProgramData\BaifanYu-build`）里构建，再把 exe 拷回来。
> 用 `BAIFANYU_STAGE` 可以指定这个目录。

---

## How to use / 怎么用

1. Run `BaifanYu.exe` — the first time you get the copyright notice and the privacy notice
2. **Hold her and drag** to move her; **drag to the left or right screen edge and let go** to make her hang there
3. **Click her** for another expression (and a squeak)
4. **Rest the pointer on her** — she stops, and a bubble shows the date, a live clock and a random kind word; move away and she keeps wandering
5. **Right-click her** (or press and hold) for skin / next expression / settings / call her back
6. Her face in the **notification area** is home base: bring her out or call her back, switch skin, toggle the squeak, the wandering and the random expressions, launch at login, **Settings** or **About**, **Quit**
7. Closing the settings window leaves her on your desktop — she quits from the notification-area menu or with **Quit BaifanYu**
8. `BaifanYu.exe --settings` / `--about` open those windows directly (handy for a
   Start-menu shortcut); if she is already running, that copy opens them for you

---

## Windows port notes / 移植说明

The Android original is a foreground service with a `TYPE_APPLICATION_OVERLAY`
window; the macOS port made that an `NSPanel`. On Windows the equivalent is a
**layered, non-activating, tool-window popup**:

| Piece / 部件 | Android | macOS port | This port |
|---|---|---|---|
| The floating window | foreground service + `TYPE_APPLICATION_OVERLAY` | `NSPanel`, `.nonactivatingPanel`, `level = .floating` | `WS_POPUP` + `WS_EX_LAYERED \| WS_EX_TOPMOST \| WS_EX_TOOLWINDOW \| WS_EX_NOACTIVATE`, pixels pushed with `UpdateLayeredWindow` |
| "Display over other apps" permission | needed | not needed | not needed |
| Persistent notification / menu bar | notification with 收回 / 设置 | menu bar dropdown + Settings window | notification-area icon with the same menu + a Settings window |
| Per-pixel click-through | — | `ignoresMouseEvents` toggled per pixel | `WS_EX_TRANSPARENT` toggled from the alpha of the pixel under the pointer |
| Rendering stops when invisible | `onWindowVisibilityChanged` | occlusion / screen-sleep / session-lock notifications | `WM_POWERBROADCAST` (console display state), `WM_WTSSESSION_CHANGE` (lock), plus a `WindowFromPoint` occlusion check |
| Sizes | dp | pt | pt × monitor DPI (`GetDpiForWindow`) |

Three things from the original's "three hard rules for floating windows" carry
over and are implemented:

| Android rule | Windows equivalent |
|---|---|
| The window must hug the pet, never a full-screen transparent layer | the popup is resized to exactly her bounding box, per state |
| The window must be non-focusable | `WS_EX_NOACTIVATE` + `WM_MOUSEACTIVATE → MA_NOACTIVATE`; clicking her never steals focus from what you are typing in |
| Rendering must really stop when invisible | the 16 ms frame timer is torn down on display-off, session lock and occlusion; a 1 s watchdog brings it back |

The geometry is a direct port of the Android `PetView` — the same floating bob
(`0.022 · amp · sin(2πt/2.4)`), the same breathing squash, the same drag tilt, the
same `min(56 pt, width × 25 %)` edge-snap threshold, the same head-only crop while
perched, and the same idle-wander numbers (4 pt per tick, 0.8 s settle, 1.2–3.6 s
rest, 5 s after a touch). The sliders keep their original ranges (120–480 / 56–160 /
30–150 %) and defaults (240 / 96 / 75 %).

While she hangs on an edge, the same artwork is reused: rotated 90° and cropped to
her head, exactly as on Android — no extra "perched" asset is needed.

---

## Project structure / 项目结构

```
src/
  main.cpp          entry point, --settings / --about, single-instance handoff
  common.h/.cpp     includes, clock, RNG, embedded-resource loading
  resource.h        resource ids shared with app.rc
  app.rc            the embedded artwork, sounds, icon and manifest
  app.manifest      per-monitor DPI + common controls v6 + asInvoker
  settings.h/.cpp   HKCU\Software\BaifanYu (same keys & numbers as Android)
  l10n.h/.cpp       trilingual strings + the hover bubble's kind words
  ui.h/.cpp         UI font choice, light/dark detection
  artwork.h/.cpp    skins, PNG decoding, pre-scaling, the 90° perch rotation
  pet.h/.cpp        the layered pet window: drawing, animation, mouse
  bubble.h/.cpp     the hover bubble (date / clock / kind word)
  settingswin.*     the settings window
  aboutwin.*        the About window
  app.h/.cpp        the controller: state, notification icon, menus, wander,
                    perching, random expressions, the frame loop
assets/
  skins/            basin_* / maid_* (standing art + 6 expressions each)
  sounds/           duck1..3.wav (Mixkit Free License)
  icons/            app.ico (16→256), AppIcon_1024.png
docs/               icon.png, expressions.png
```

---

## Credits & thanks / 致谢与许可

### Thank you / 特别感谢

**This project is a port. Nothing here would exist without
[LIN428924379/baifanyu](https://github.com/LIN428924379/baifanyu) — the original Android app by
[@LIN428924379](https://github.com/LIN428924379).**

**All credit for the concept, the character, the artwork and the interaction design belongs to the
original author. This Windows port reuses their artwork, their interaction design and their
animation maths, and only re-implements the code for Windows. Thank you for building it and for
releasing it under the MIT license. 感谢原作者把《白饭鱼》开源出来 —— 这个 Windows 版只是把她的家从手机
搬到了 Windows 上，创意、角色、立绘、动作设计全部属于原作者。**

It also follows the
[macOS port by @duhaoze2007](https://github.com/duhaoze2007/Baifanyu-For-Mac),
whose feature set (hover bubble, random expressions, per-pixel click-through,
trilingual UI, launch at login, stop-rendering-when-invisible) this port keeps in full.

### What comes from where / 各部分出处

| Part / 部分 | Origin / 出处 |
|---|---|
| Original app, concept, interaction design / 原应用、创意、交互设计 | [LIN428924379/baifanyu](https://github.com/LIN428924379/baifanyu) (MIT) |
| Standing art & 6 expressions per skin / 立绘与 6 个表情 | The original author's assets (community fan art, **not** MIT) |
| Duck squeaks / 小黄鸭音效 | [Mixkit](https://mixkit.co/free-sound-effects/duck/) (Mixkit Free License) |
| Character / 角色形象 | DeepSeek-community whale girl, as delivered with the original app |
| macOS port / macOS 移植 | [duhaoze2007/BaifanYu-For-Mac](https://github.com/duhaoze2007/BaifanYu-For-Mac) |
| Windows code (C++ / Win32 / GDI+) / Windows 代码 | This repository, MIT, © 2026 Du Haoze |

### License / 许可证

- **Code**: MIT — see [LICENSE](LICENSE)
- **Artwork** (`assets/skins/*.png`, `assets/icons/*`, `docs/`): community fan art from the original
  project, **not** covered by MIT — personal, non-commercial use only
- **代码**：MIT —— 见 [LICENSE](LICENSE)
- **美术资源**：来自原项目的社区同人创作，**不在 MIT 许可范围内**，仅限个人非商业使用

Windows version © 2026 Du Haoze · macOS version © 2026 Du Haoze · Original Android app © 2026 LIN428924379
