#include "l10n.h"

#include <algorithm>

namespace bfy {

namespace {

struct Row {
    S key;
    const wchar_t* en;
    const wchar_t* hans;
    const wchar_t* hant;
};

// The table mirrors the macOS LocalizationManager.strings dictionary; wording is
// adapted where the platform differs (menu bar -> notification area, ⌘, -> the
// settings entry in the tray menu, Dock -> taskbar).
const Row kRows[] = {
    // ------------------------------------------------------------ identity
    {S::AppName, L"BaifanYu", L"白饭鱼", L"白飯魚"},
    {S::Tagline, L"A rice-eating whale girl, living on your Windows desktop",
     L"吃白饭的鲸鱼娘，挂在你的 Windows 桌面上",
     L"吃白飯的鯨魚娘，掛在你的 Windows 桌面上"},
    {S::Subtitle, L"Offline · No uploads · No account", L"不联网 · 不上传 · 不要账号",
     L"不聯網 · 不上傳 · 不要帳號"},

    // ------------------------------------------------------------ tray menu
    {S::ShowHer, L"Bring her out", L"让她出现", L"讓她出現"},
    {S::HideHer, L"Call her back", L"收回她", L"收回她"},
    {S::NextFace, L"Another expression", L"换一个表情", L"換一個表情"},
    {S::SkinMenu, L"Skin", L"皮肤", L"皮膚"},
    {S::Settings, L"Settings…", L"设置…", L"設定…"},
    {S::About, L"About BaifanYu", L"关于白饭鱼", L"關於白飯魚"},
    {S::Quit, L"Quit BaifanYu", L"退出白饭鱼", L"結束白飯魚"},
    {S::LaunchAtLogin, L"Launch at login", L"登录时自动启动", L"登入時自動啟動"},

    // ------------------------------------------------------------ skins
    {S::SkinBasin, L"Bowl-head", L"饭盆头", L"飯盆頭"},
    {S::SkinMaid, L"Maid outfit", L"女仆装", L"女僕裝"},

    // ------------------------------------------------------------ size & motion
    {S::SizeSection, L"Size & motion", L"调节", L"調節"},
    {S::SizeHint,
     L"Two separate sizes: while floating it is her overall height, while she hangs on a screen "
     L"edge only her head shows — that one is the head width.",
     L"大小分两处：悬空时看整体高度，扒在边上时只有脑袋露出来、看的是脑袋宽度。",
     L"大小分兩處：懸空時看整體高度，扒在邊上時只有腦袋露出來、看的是腦袋寬度。"},
    {S::HoverSize, L"Floating height", L"悬空大小", L"懸空大小"},
    {S::PerchWidthLabel, L"Head width while perched", L"扒边时脑袋宽度", L"扒邊時腦袋寬度"},
    {S::AmplitudeLabel, L"Amount of motion", L"动态幅度", L"動態幅度"},

    // ------------------------------------------------------------ behaviour
    {S::BehaviourSection, L"Behaviour", L"行为", L"行為"},
    {S::WanderTitle, L"Wander around", L"自动溜达", L"自動溜達"},
    {S::WanderHint,
     L"She crawls around on her own when idle (never while you touch, drag or perch her, and she "
     L"stops the moment the pointer rests on her)",
     L"闲着的时候她自己爬来爬去（鼠标停在她身上、碰她、拖她、趴边时不动）",
     L"閒著的時候她自己爬來爬去（滑鼠停在她身上、碰她、拖她、趴邊時不動）"},
    {S::RandomFaceTitle, L"Random expressions", L"随机换表情", L"隨機換表情"},
    {S::RandomFaceHint, L"She changes expression by herself every so often while she idles",
     L"闲着的时候她会自己变表情（隔十几秒到一分多钟换一个）",
     L"閒著的時候她自己換表情（隔十幾秒到一分多鐘換一個）"},
    {S::SoundTitle, L"Rubber-duck squeak", L"小黄鸭音效", L"小黃鴨音效"},
    {S::SoundHint, L"Click her and she squeaks", L"点她一下，吱一声", L"點她一下，吱一聲"},

    // ------------------------------------------------------------ skin section
    {S::SkinSection, L"Skin", L"皮肤", L"皮膚"},
    {S::SkinHint, L"Two looks, 6 expressions each", L"两套形象，每套 6 个表情",
     L"兩套形象，每套 6 個表情"},
    {S::TapToSwitch, L"Click to switch", L"点击切换", L"點擊切換"},

    // ------------------------------------------------------------ actions
    {S::ShowButton, L"Bring her out", L"让她出现", L"讓她出現"},
    {S::HideButton, L"Call her back", L"收回她", L"收回她"},
    {S::TestFaceButton, L"Try an expression", L"试一下换表情", L"試一下換表情"},
    {S::SheIsRunning,
     L"She is with you. Drag her to the left or right edge of the screen and let go — she will "
     L"hang there.",
     L"她正在陪你。拖到屏幕左右边缘松手，她会扒在边上。",
     L"她正在陪你。拖到螢幕左右邊緣放手，她會扒在邊上。"},
    {S::SheIsResting, L"Ready. Press the button below to let her out.",
     L"已就绪，点下面的按钮让她出现。", L"已就緒，點下面的按鈕讓她出現。"},

    // ------------------------------------------------------------ status
    {S::StatusSection, L"Her", L"她的状态", L"她的狀態"},
    {S::StatusRunning, L"Status: with you", L"状态：正在陪你", L"狀態：正在陪你"},
    {S::StatusReady, L"Status: resting", L"状态：休息中", L"狀態：休息中"},
    {S::StateHover, L"floating", L"悬空中", L"懸空中"},
    {S::StatePerchLeft, L"hanging on the left edge", L"扒在左边缘", L"扒在左邊緣"},
    {S::StatePerchRight, L"hanging on the right edge", L"扒在右边缘", L"扒在右邊緣"},

    // ------------------------------------------------------------ how to play
    {S::HowToTitle, L"How to play", L"怎么玩", L"怎麼玩"},
    {S::HowToBody,
     L"· Hold her and drag anywhere\n"
     L"· Drag to the left or right edge of the screen and let go — she hangs there, only her head "
     L"showing\n"
     L"· Drag her back from the edge to make her float again\n"
     L"· Click her for another expression\n"
     L"· Rest the pointer on her: she stops, and a bubble shows the date, the time and a random "
     L"kind word\n"
     L"· Right-click her (or press and hold) for skin / settings / call her back",
     L"· 按住她，拖到任意位置\n"
     L"· 拖到屏幕左右边缘松手 —— 她会扒在边上，只露一个脑袋看着你\n"
     L"· 从边缘往外拖，回到悬空状态\n"
     L"· 点她一下，换一个表情\n"
     L"· 鼠标停在她身上：她就不动了，旁边浮出一个小气泡，写今天的日期、时间，还有一句随机的关心\n"
     L"· 右键点她（或长按）弹出「皮肤 / 设置 / 收回」",
     L"· 按住她，拖到任意位置\n"
     L"· 拖到螢幕左右邊緣放手 —— 她會扒在邊上，只露一個腦袋看著你\n"
     L"· 從邊緣往外拖，回到懸空狀態\n"
     L"· 點她一下，換一個表情\n"
     L"· 滑鼠停在她身上：她就不動了，旁邊浮出一個小氣泡，寫今天的日期、時間，還有一句隨機的關心\n"
     L"· 右鍵點她（或長按）彈出「皮膚 / 設定 / 收回」"},

    // ------------------------------------------------------------ about
    {S::AboutBody,
     L"A tiny whale girl who floats on your desktop, eats white rice and squeaks when poked.",
     L"一只浮在你桌面上的鲸鱼娘，吃白饭，戳一下会吱一声。",
     L"一隻浮在你桌面上的鯨魚娘，吃白飯，戳一下會吱一聲。"},
    {S::VersionLabel, L"Version", L"版本", L"版本"},
    {S::Copyright, L"Windows port © 2026 Du Haoze · MIT License",
     L"Windows 版 © 2026 Du Haoze · MIT License", L"Windows 版 © 2026 Du Haoze · MIT License"},
    {S::MitLicense,
     L"Original Android app by LIN428924379 (MIT). Artwork belongs to its community authors and is "
     L"not covered by the MIT license.",
     L"原 Android 应用由 LIN428924379 创作（MIT）。美术资源属于社区同人创作，不在 MIT 许可范围内。",
     L"原 Android 應用由 LIN428924379 創作（MIT）。美術資源屬於社群同人創作，不在 MIT 授權範圍內。"},
    {S::Language, L"Language", L"语言", L"語言"},
    {S::FollowSystem, L"Follow System", L"跟随系统", L"跟隨系統"},
    {S::CreditsTitle, L"Credits", L"素材与致谢", L"素材與致謝"},
    {S::CreditsBody,
     L"Whale girl character from the DeepSeek community; standing art & expressions generated with "
     L"AI and cut out by the original author; duck squeaks from Mixkit (Mixkit Free License).",
     L"角色形象来自 DeepSeek 社区同人创作；立绘与表情由 AI 生成、原作者后期对齐去背；小黄鸭音效来自 "
     L"Mixkit（Mixkit Free License）。",
     L"角色形象來自 DeepSeek 社群同人創作；立繪與表情由 AI 生成、原作者後期對齊去背；小黃鴨音效來自 "
     L"Mixkit（Mixkit Free License）。"},
    {S::LicenseTitle, L"License", L"许可证", L"授權"},
    {S::LicenseBody, L"Code: MIT. Artwork: personal, non-commercial use only.",
     L"代码：MIT。美术资源：仅限个人非商业使用。", L"程式碼：MIT。美術資源：僅限個人非商業使用。"},

    // ------------------------------------------------------------ first launch
    {S::FirstRunCopyrightTitle, L"Copyright Notice", L"版权声明", L"版權聲明"},
    {S::FirstRunCopyrightBody,
     L"Windows version © 2026 Du Haoze. All rights reserved.\n\n"
     L"The code is licensed under the MIT License. The character artwork comes from the original "
     L"Android app 白饭鱼 by LIN428924379 — community fan art, not covered by MIT, for personal "
     L"non-commercial use only. Please do not use the artwork commercially.",
     L"Windows 版 © 2026 Du Haoze. 保留所有权利。\n\n"
     L"代码采用 MIT License 开源许可。角色美术资源来自原作者 LIN428924379 的 Android 应用《白饭鱼》—— "
     L"属于社区同人创作，不在 MIT 许可范围内，仅限个人非商业使用。请勿将美术资源用于商业用途。",
     L"Windows 版 © 2026 Du Haoze. 保留所有權利。\n\n"
     L"程式碼採用 MIT License 開源授權。角色美術資源來自原作者 LIN428924379 的 Android 應用《白飯魚》—— "
     L"屬於社群同人創作，不在 MIT 授權範圍內，僅限個人非商業使用。請勿將美術資源用於商業用途。"},
    {S::FirstRunPrivacyTitle, L"Privacy Notice", L"隐私声明", L"隱私聲明"},
    {S::FirstRunPrivacyBody,
     L"BaifanYu is completely offline. It has no network code at all: no analytics, no tracking, no "
     L"account, no uploads.\n\n"
     L"Everything — her position, size, expressions and squeaks — stays on this PC, stored in your "
     L"own registry hive (HKCU\\Software\\BaifanYu).",
     L"白饭鱼完全离线运行。它没有任何网络代码：无统计、无追踪、不要账号、不上传。\n\n"
     L"她的位置、大小、表情和音效全部保留在这台电脑上，存放在你自己的注册表项里"
     L"（HKCU\\Software\\BaifanYu）。",
     L"白飯魚完全離線執行。它沒有任何網路程式碼：無統計、無追蹤、不要帳號、不上傳。\n\n"
     L"她的位置、大小、表情和音效全部保留在這台電腦上，存放在你自己的登錄檔項裡"
     L"（HKCU\\Software\\BaifanYu）。"},
    {S::Agree, L"Agree", L"同意", L"同意"},
    {S::Disagree, L"Disagree", L"不同意", L"不同意"},

    // ------------------------------------------------------------ misc
    {S::PrivacyTitle, L"Privacy", L"隐私", L"隱私"},
    {S::PrivacyBody, L"Offline by design: no network, no analytics, no account.",
     L"完全离线：无网络、无统计、不要账号。", L"完全離線：無網路、無統計、不要帳號。"},
    {S::LoginItemFailed, L"Could not change the login item — please check your permissions.",
     L"无法修改登录项 —— 请检查权限。", L"無法修改登入項 —— 請檢查權限。"},
    {S::NotesTitle, L"Notes", L"小提示", L"小提示"},
    {S::NotesBody,
     L"She has no taskbar button — the notification-area icon is her home. Closing this window "
     L"leaves her on your desktop; quit from that icon's menu.",
     L"她没有任务栏按钮 —— 右下角通知区那只小鱼就是她的家。关掉这个窗口她还在桌面上，从那个图标退出。",
     L"她沒有工作列按鈕 —— 右下角通知區那隻小魚就是她的家。關掉這個視窗她還在桌面上，從那個圖示離開。"},
};

const wchar_t* const* Pick(const Row& row, Lang lang) {
    switch (lang) {
        case Lang::ZhHans: return &row.hans;
        case Lang::ZhHant: return &row.hant;
        case Lang::En:
        default: return &row.en;
    }
}

// 16 kind words per language, straight from the macOS port.
const wchar_t* const kCaringEn[] = {
    L"Take care of yourself — work can wait a minute",
    L"Drink some water, stop staring at the screen",
    L"You did well today. Stretch a little",
    L"Eat something proper — I'm right here with you",
    L"If you're tired, rest. The world keeps spinning",
    L"You're doing better than you think",
    L"Don't stay up too late, okay?",
    L"Look out the window for a moment",
    L"Take a deep breath. Slow is fine too",
    L"You don't have to carry everything alone",
    L"Stand up and walk around a bit",
    L"What would you like to eat today?",
    L"If you're upset, poke me for a squeak",
    L"No rush — we have time",
    L"You've worked hard. It's okay to slack off",
    L"Smile a little — I'm keeping an eye on you",
};

const wchar_t* const kCaringHans[] = {
    L"工作再忙也要照顾好自己哦",
    L"记得喝水，别一直盯着屏幕",
    L"今天也辛苦了，先伸个懒腰吧",
    L"饭要好好吃，我在这里陪你",
    L"累了就歇一会儿，天塌不下来",
    L"你已经做得很好了，真的",
    L"别熬太晚，明天还要继续加油",
    L"眼睛酸了就看看窗外",
    L"深呼吸一下，慢慢来也可以",
    L"有我在，不用什么都自己扛",
    L"记得站起来动一动，坐太久了",
    L"今天想吃点什么好的？",
    L"不高兴的话，就戳戳我吧",
    L"慢慢来，我们还有时间",
    L"你已经很努力了，允许自己偷个懒",
    L"笑一个吧，我一直看着你呢",
};

const wchar_t* const kCaringHant[] = {
    L"工作再忙也要照顧好自己喔",
    L"記得喝水，別一直盯著螢幕",
    L"今天也辛苦了，先伸個懶腰吧",
    L"飯要好好吃，我在這裡陪你",
    L"累了就歇一會兒，天塌不下來",
    L"你已經做得很好了，真的",
    L"別熬太晚，明天還要繼續加油",
    L"眼睛酸了就看看窗外",
    L"深呼吸一下，慢慢來也可以",
    L"有我在，不用什麼都自己扛",
    L"記得站起來動一動，坐太久了",
    L"今天想吃點什麼好的？",
    L"不高興的話，就戳戳我吧",
    L"慢慢來，我們還有時間",
    L"你已經很努力了，允許自己偷個懶",
    L"笑一個吧，我一直看著你呢",
};

const wchar_t* const* CaringPool(Lang lang, int* count) {
    switch (lang) {
        case Lang::ZhHans: *count = _countof(kCaringHans); return kCaringHans;
        case Lang::ZhHant: *count = _countof(kCaringHant); return kCaringHant;
        case Lang::En:
        default: *count = _countof(kCaringEn); return kCaringEn;
    }
}

/// Locale name used for date/time formatting.
const wchar_t* LocaleName(Lang lang) {
    switch (lang) {
        case Lang::ZhHans: return L"zh-CN";
        case Lang::ZhHant: return L"zh-TW";
        case Lang::En:
        default: return L"en-US";
    }
}

}  // namespace

L10n& L10n::Get() {
    static L10n inst;
    return inst;
}

void L10n::Init() { language_ = Lang::System; }

void L10n::SetLanguage(Lang lang) { language_ = lang; }

Lang L10n::Resolved() const {
    if (language_ != Lang::System) return language_;
    return DetectSystemLanguage();
}

std::wstring L10n::T(S key) const {
    const Lang lang = Resolved();
    for (const Row& row : kRows) {
        if (row.key == key) return std::wstring(*Pick(row, lang));
    }
    return std::wstring();
}

std::wstring L10n::FacesLoaded(int n) const {
    switch (Resolved()) {
        case Lang::ZhHans: return Format(L"表情素材：已加载 %d / 6", n);
        case Lang::ZhHant: return Format(L"表情素材：已載入 %d / 6", n);
        case Lang::En:
        default: return Format(L"Artwork loaded: %d / 6 expressions", n);
    }
}

std::wstring L10n::RandomCaringSentence(const std::wstring& previous) const {
    int count = 0;
    const wchar_t* const* pool = CaringPool(Resolved(), &count);
    if (count <= 0) return std::wstring();
    std::wstring pick = pool[RandomInt(0, count - 1)];
    for (int attempt = 0; attempt < 6 && pick == previous && count > 1; ++attempt) {
        pick = pool[RandomInt(0, count - 1)];
    }
    return pick;
}

std::wstring L10n::FullDate(const SYSTEMTIME& st) const {
    const Lang lang = Resolved();
    const wchar_t* picture = (lang == Lang::En) ? L"dddd, MMMM d, yyyy" : L"yyyy'年'M'月'd'日'dddd";
    wchar_t buf[128] = {0};
    if (GetDateFormatEx(LocaleName(lang), 0, &st, picture, buf, _countof(buf), nullptr) == 0) {
        return Format(L"%04d-%02d-%02d", st.wYear, st.wMonth, st.wDay);
    }
    return buf;
}

std::wstring L10n::MediumTime(const SYSTEMTIME& st) const {
    const Lang lang = Resolved();
    const wchar_t* picture = (lang == Lang::En) ? L"h:mm:ss tt" : L"HH:mm:ss";
    wchar_t buf[64] = {0};
    if (GetTimeFormatEx(LocaleName(lang), 0, &st, picture, buf, _countof(buf)) == 0) {
        return Format(L"%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
    }
    return buf;
}

std::wstring LanguageDisplayName(Lang lang) {
    switch (lang) {
        case Lang::En: return L"English";
        case Lang::ZhHans: return L"简体中文";
        case Lang::ZhHant: return L"繁體中文";
        case Lang::System: return L"";
    }
    return L"";
}

Lang DetectSystemLanguage() {
    wchar_t name[LOCALE_NAME_MAX_LENGTH] = {0};
    if (GetUserDefaultLocaleName(name, _countof(name)) == 0) return Lang::En;
    if (_wcsnicmp(name, L"zh", 2) != 0) return Lang::En;
    // Traditional Chinese locales: Taiwan, Hong Kong, Macau, or an explicit
    // Hant script subtag.
    if (_wcsnicmp(name, L"zh-TW", 5) == 0 || _wcsnicmp(name, L"zh-HK", 5) == 0 ||
        _wcsnicmp(name, L"zh-MO", 5) == 0 || wcsstr(name, L"Hant") != nullptr) {
        return Lang::ZhHant;
    }
    return Lang::ZhHans;
}

}  // namespace bfy
