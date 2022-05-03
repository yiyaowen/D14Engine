#include "Precompile.h"

#include "UI/UIResourceUtils.h"

#include "UI/Application.h"

namespace d14engine::ui
{
    void UIResourceUtils::Initialize()
    {
        UpdateSystemFontNames();

        LoadBasicSystemTextFormats();

        LoadCommonBrushes();

        LoadCommonEffects();
    }

    void UIResourceUtils::UpdateSystemFontNames()
    {
        SYSTEM_FONT_NAMES.clear();

        ComPtr<IDWriteFontCollection1> fontCollection;
        THROW_IF_FAILED(Application::RNDR->dWriteFactory->
            GetSystemFontCollection(FALSE, &fontCollection, TRUE));

        std::vector<ComPtr<IDWriteFontFamily>>
            fontFamilies(fontCollection->GetFontFamilyCount());

        for (UINT i = 0; i < fontFamilies.size(); ++i)
        {
            THROW_IF_FAILED(fontCollection->GetFontFamily(i, &fontFamilies[i]));
        }

        for (auto& family : fontFamilies)
        {
            ComPtr<IDWriteLocalizedStrings> familyNames;
            THROW_IF_FAILED(family->GetFamilyNames(&familyNames));

            UINT familyCount = familyNames->GetCount();
            for (UINT i = 0; i < familyCount; ++i)
            {
                UINT length = 0;

                // The string size should include space for terminating null character;
                // however, the length getter's return doesn't count that for us,
                // so we do "++length" when construct the string to allocate enough space.

                THROW_IF_FAILED(familyNames->GetLocaleNameLength(i, &length));
                Wstring localeName(++length, L'\0');
                THROW_IF_FAILED(familyNames->GetLocaleName(i, localeName.data(), length));

                THROW_IF_FAILED(familyNames->GetStringLength(i, &length));
                Wstring fontName(++length, L'\0');
                THROW_IF_FAILED(familyNames->GetString(i, fontName.data(), length));

                // These names will be used to build the container later, 
                // so we must erase terminating null character manually.

                localeName.erase(localeName.size() - 1);
                fontName.erase(fontName.size() - 1);

                SYSTEM_FONT_NAMES.insert({ fontName, localeName });
            }
        }
    }

    UIResourceUtils::SystemFontNameSet UIResourceUtils::SYSTEM_FONT_NAMES;

    void UIResourceUtils::LoadBasicSystemTextFormats()
    {
        // 简体中文 UI 默认字体
        LoadSystemTextFormat(L"微软雅黑/Normal/16", L"微软雅黑", 16.0f, L"zh-cn");
        // English UI default
        LoadSystemTextFormat(L"Segoe/Normal/16", L"Segoe UI", 16.0f, L"en-us");
    }

    void UIResourceUtils::LoadSystemTextFormat(
        WstrParam textFormatName,
        WstrParam fontFamilyName,
        FLOAT fontSize,
        WstrParam localeName,
        DWRITE_FONT_WEIGHT fontWeight,
        DWRITE_FONT_STYLE fontStyle,
        DWRITE_FONT_STRETCH fontStretch)
    {
        THROW_IF_FAILED(Application::RNDR->dWriteFactory->CreateTextFormat(
            fontFamilyName.c_str(),
            nullptr,
            fontWeight,
            fontStyle,
            fontStretch,
            // 1 inch = 72 pt = 96 dip
            fontSize * 96.0f / 72.0f,
            localeName.c_str(),
            &TEXT_FORMATS[textFormatName]));
    }

    UIResourceUtils::TextFormatMap UIResourceUtils::TEXT_FORMATS;

    void UIResourceUtils::LoadCommonBrushes()
    {
        // Solid Color Brush
        THROW_IF_FAILED(Application::RNDR->d2d1DeviceContext->
            CreateSolidColorBrush((D2D1::ColorF)D2D1::ColorF::Black, &SOLID_COLOR_BRUSH));
    }

    ComPtr<ID2D1SolidColorBrush> UIResourceUtils::SOLID_COLOR_BRUSH;

    void UIResourceUtils::LoadCommonEffects()
    {
        // Shadow Effect
        THROW_IF_FAILED(Application::RNDR->d2d1DeviceContext->
            CreateEffect(CLSID_D2D1Shadow, &SHADOW_EFFECT));
    }

    ComPtr<ID2D1Effect> UIResourceUtils::SHADOW_EFFECT;
}