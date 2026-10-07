#include "settings.h"

namespace bfy {

namespace {

constexpr const wchar_t* kRunKey =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
constexpr const wchar_t* kRunValue = L"BaifanYu";

HKEY OpenSettingsKey(bool create) {
    HKEY key = nullptr;
    REGSAM access = KEY_READ | (create ? KEY_WRITE : 0);
    if (RegCreateKeyExW(HKEY_CURRENT_USER, kRegPath, 0, nullptr,
                        REG_OPTION_NON_VOLATILE, access, nullptr, &key,
                        nullptr) != ERROR_SUCCESS) {
        return nullptr;
    }
    return key;
}

DWORD ReadDword(HKEY key, const wchar_t* name, DWORD fallback) {
    DWORD type = 0, value = 0, size = sizeof(value);
    if (RegQueryValueExW(key, name, nullptr, &type, reinterpret_cast<LPBYTE>(&value),
                         &size) != ERROR_SUCCESS ||
        type != REG_DWORD) {
        return fallback;
    }
    return value;
}

void WriteDword(HKEY key, const wchar_t* name, DWORD value) {
    RegSetValueExW(key, name, 0, REG_DWORD, reinterpret_cast<const BYTE*>(&value),
                   sizeof(value));
}

double ReadDouble(HKEY key, const wchar_t* name, double fallback) {
    DWORD raw = ReadDword(key, name, 0xFFFFFFFFu);
    if (raw == 0xFFFFFFFFu) return fallback;
    return static_cast<double>(raw) / 1000.0;
}

void WriteDouble(HKEY key, const wchar_t* name, double value) {
    WriteDword(key, name, static_cast<DWORD>(value * 1000.0 + 0.5));
}

std::wstring ReadString(HKEY key, const wchar_t* name) {
    wchar_t buf[128] = {0};
    DWORD type = 0, size = sizeof(buf);
    if (RegQueryValueExW(key, name, nullptr, &type, reinterpret_cast<LPBYTE>(buf),
                         &size) != ERROR_SUCCESS ||
        type != REG_SZ) {
        return std::wstring();
    }
    return std::wstring(buf);
}

void WriteString(HKEY key, const wchar_t* name, const std::wstring& value) {
    RegSetValueExW(key, name, 0, REG_SZ,
                   reinterpret_cast<const BYTE*>(value.c_str()),
                   static_cast<DWORD>((value.size() + 1) * sizeof(wchar_t)));
}

Lang LangFromString(const std::wstring& s, Lang fallback) {
    if (s == L"en") return Lang::En;
    if (s == L"zh-Hans") return Lang::ZhHans;
    if (s == L"zh-Hant") return Lang::ZhHant;
    if (s == L"system") return Lang::System;
    return fallback;
}

const wchar_t* LangToString(Lang lang) {
    switch (lang) {
        case Lang::En: return L"en";
        case Lang::ZhHans: return L"zh-Hans";
        case Lang::ZhHant: return L"zh-Hant";
        case Lang::System:
        default: return L"system";
    }
}

}  // namespace

void Settings::Load() {
    HKEY key = OpenSettingsKey(false);
    if (!key) return;  // first run: keep the defaults

    hoverHeight = Clamp(ReadDouble(key, L"hover_height_dp", kDefaultHoverHeight),
                        kMinHoverHeight, kMaxHoverHeight);
    perchWidth = Clamp(ReadDouble(key, L"perch_width_dp", kDefaultPerchWidth),
                       kMinPerchWidth, kMaxPerchWidth);
    amplitude = Clamp(ReadDouble(key, L"amp_scale", kDefaultAmplitude), kMinAmplitude,
                      kMaxAmplitude);

    soundOn = ReadDword(key, L"sound_on", 1) != 0;
    wanderOn = ReadDword(key, L"wander_on", 1) != 0;
    randomFace = ReadDword(key, L"random_face", 1) != 0;

    skinIndex = static_cast<int>(ReadDword(key, L"skin_index", 0));
    running = ReadDword(key, L"running", 1) != 0;
    perched = ReadDword(key, L"perched", 0) != 0;
    perchedEdge = static_cast<int>(ReadDword(key, L"edge", 0));
    hasPosition = ReadDword(key, L"has_position", 0) != 0;
    windowX = static_cast<double>(static_cast<int32_t>(ReadDword(key, L"window_x", 0)));
    windowY = static_cast<double>(static_cast<int32_t>(ReadDword(key, L"window_y", 0)));
    language = LangFromString(ReadString(key, L"language"), Lang::System);
    firstRunDone = ReadDword(key, L"first_run_done", 0) != 0;

    RegCloseKey(key);
}

void Settings::Save() const {
    HKEY key = OpenSettingsKey(true);
    if (!key) return;

    WriteDouble(key, L"hover_height_dp", hoverHeight);
    WriteDouble(key, L"perch_width_dp", perchWidth);
    WriteDouble(key, L"amp_scale", amplitude);

    WriteDword(key, L"sound_on", soundOn ? 1 : 0);
    WriteDword(key, L"wander_on", wanderOn ? 1 : 0);
    WriteDword(key, L"random_face", randomFace ? 1 : 0);

    WriteDword(key, L"skin_index", static_cast<DWORD>(skinIndex));
    WriteDword(key, L"running", running ? 1 : 0);
    WriteDword(key, L"perched", perched ? 1 : 0);
    WriteDword(key, L"edge", static_cast<DWORD>(perchedEdge));
    WriteDword(key, L"has_position", hasPosition ? 1 : 0);
    WriteDword(key, L"window_x", static_cast<DWORD>(static_cast<int32_t>(windowX)));
    WriteDword(key, L"window_y", static_cast<DWORD>(static_cast<int32_t>(windowY)));
    WriteString(key, L"language", LangToString(language));
    WriteDword(key, L"first_run_done", firstRunDone ? 1 : 0);

    RegCloseKey(key);
}

void Settings::SavePosition() const {
    HKEY key = OpenSettingsKey(true);
    if (!key) return;
    WriteDword(key, L"window_x", static_cast<DWORD>(static_cast<int32_t>(windowX)));
    WriteDword(key, L"window_y", static_cast<DWORD>(static_cast<int32_t>(windowY)));
    WriteDword(key, L"has_position", hasPosition ? 1 : 0);
    RegCloseKey(key);
}

bool LaunchAtLoginEnabled() {
    HKEY key = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, kRunKey, 0, KEY_READ, &key) != ERROR_SUCCESS) {
        return false;
    }
    const std::wstring value = ReadString(key, kRunValue);
    RegCloseKey(key);
    return !value.empty();
}

bool SetLaunchAtLogin(bool enabled) {
    HKEY key = nullptr;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, kRunKey, 0, nullptr, 0, KEY_SET_VALUE, nullptr,
                        &key, nullptr) != ERROR_SUCCESS) {
        return false;
    }
    bool ok = true;
    if (enabled) {
        std::wstring command = L"\"" + ExecutablePath() + L"\"";
        ok = RegSetValueExW(key, kRunValue, 0, REG_SZ,
                            reinterpret_cast<const BYTE*>(command.c_str()),
                            static_cast<DWORD>((command.size() + 1) * sizeof(wchar_t))) ==
             ERROR_SUCCESS;
    } else {
        const LONG rc = RegDeleteValueW(key, kRunValue);
        ok = (rc == ERROR_SUCCESS || rc == ERROR_FILE_NOT_FOUND);
    }
    RegCloseKey(key);
    return ok;
}

}  // namespace bfy
