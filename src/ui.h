// BaifanYu (白饭鱼) for Windows — shared UI helpers (fonts, theme).
#ifndef BAIFANYU_UI_H
#define BAIFANYU_UI_H

#include "common.h"

namespace bfy {

/// A CJK-capable UI font family that exists on this machine
/// (Microsoft YaHei UI -> Microsoft YaHei -> Segoe UI).
const wchar_t* UiFontFamily();

/// True when Windows is set to the dark app theme — the hover bubble and the
/// settings window follow it, the way the macOS port follows the system
/// appearance.
bool SystemUsesDarkMode();

/// A GDI font for the standard controls, sized in points.
HFONT CreateUiFont(int pointSize, bool bold = false);

}  // namespace bfy

#endif  // BAIFANYU_UI_H
