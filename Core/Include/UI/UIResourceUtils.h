#pragma once

#include "Precompile.h"

namespace d14engine::ui
{
    struct UIResourceUtils
    {
        static void Initialize();

    public:
        // Pick the alternative character if "shift" key pressed.
        struct PrintableCharacter { WCHAR normal, alternative; };

        using KeyboardLayoutMap = std::unordered_map<int /* Virtual-Key Code */, PrintableCharacter>;

        static KeyboardLayoutMap US_KEYBOARD_LAYOUT; // US standard keyboard layout.

    public:
        static Optional<Wstring> GetClipboardText(HWND hWndNewOwner = nullptr);

        static void SetClipboardText(WstrViewParam content, HWND hWndNewOwner = nullptr);

    private:
        struct SystemFontName { Wstring family, locale; };

        struct SystemFontNameSetHash
        {
            size_t operator() (const SystemFontName& elem) const
            {
                auto wstrhash = std::hash<Wstring>();
                // Simply add all hash values together without special optimization.
                return wstrhash(elem.family) + wstrhash(elem.locale);
            }
        };
        struct SystemFontNameSetEqual
        {
            bool operator() (const SystemFontName& left, const SystemFontName& right) const
            {
                return left.family == right.family && left.locale == right.locale;
            }
        };
        using SystemFontNameSet = std::unordered_set<SystemFontName, SystemFontNameSetHash, SystemFontNameSetEqual>;

        static SystemFontNameSet SYSTEM_FONT_NAMES;

        using TextFormatMap = std::unordered_map<Wstring, ComPtr<IDWriteTextFormat>>;

    public:
        static void UpdateSystemFontNames();

        static TextFormatMap TEXT_FORMATS;

        static void LoadBasicSystemTextFormats();

        static void LoadSystemTextFormat(
            WstrParam textFormatName,
            WstrParam fontFamilyName,
            FLOAT fontSize,
            WstrParam localeName,
            DWRITE_FONT_WEIGHT fontWeight = DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE fontStyle = DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH fontStretch = DWRITE_FONT_STRETCH_NORMAL);

        // Some resources can be customized at runtime, so there's no need to create multiple copies.
        // For example, a solid color brush's color and opacity can be changed before drawing,
        // so we can use the same brush in everywhere and change its properties dynamically,
        // in which way a lot of duplicated memory spaces can be saved to improve performance.

    public:
        static void LoadCommonBrushes();

        static ComPtr<ID2D1SolidColorBrush> SOLID_COLOR_BRUSH;

        static void LoadCommonEffects();

        static ComPtr<ID2D1Effect> SHADOW_EFFECT;
    };
    // Introduce this alias to avoid too long prefix.
    using UIResu = UIResourceUtils;
}