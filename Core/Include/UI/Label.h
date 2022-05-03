#pragma once

#include "Precompile.h"

#include "Panel.h"
#include "SolidColorStyle.h"
#include "UIResourceUtils.h"

namespace d14engine::ui
{
    struct Label : Panel, SolidColorStyle
    {
#define FORMAT_DEFAULT_VALUE UIResu::TEXT_FORMATS.at(L"微软雅黑/Normal/16")

        Label(
            WstrParam text,
            D2D_RECT_F rect,
            D2D1_COLOR_F textColor = (D2D1::ColorF)D2D1::ColorF::Black,
            float textOpaque = 1.0f,
            D2D1_COLOR_F solidColor = (D2D1::ColorF)D2D1::ColorF::White,
            float solidOpaque = 0.0f,
            ComPtrParam<IDWriteTextFormat> format = FORMAT_DEFAULT_VALUE);

#undef FORMAT_DEFAULT_VALUE

        D2D1_COLOR_F textColor;

        float textOpaque;

        ComPtr<IDWriteTextFormat> format;

        struct Alignment
        {
            DWRITE_TEXT_ALIGNMENT horizontal = DWRITE_TEXT_ALIGNMENT_CENTER;
            DWRITE_PARAGRAPH_ALIGNMENT vertical = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
        }
        alignment;

        D2D1_DRAW_TEXT_OPTIONS options = D2D1_DRAW_TEXT_OPTIONS_NONE;

        DWRITE_MEASURING_MODE measuringMode = DWRITE_MEASURING_MODE_NATURAL;

        DWRITE_TEXT_METRICS GetTextLayoutMetrics();

    private:
        Wstring m_text;

        DWRITE_TEXT_METRICS m_textMetrics;

    public:
        Wstring Text();
        void SetText(WstrParam text);

    public:
        // Override interface methods.

        // IDrawObject2D
        void OnRendererDrawD2D1Object(Renderer* rndr) override;

        // IUIObject
        float MinimalWidth() override;

        float MinimalHeight() override;
    };
}