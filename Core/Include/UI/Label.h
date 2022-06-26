#pragma once

#include "Precompile.h"

#include "Panel.h"
#include "SolidStyle.h"
#include "UIResourceUtils.h"

namespace d14engine::ui
{
    struct Label : Panel, SolidStyle
    {
#define FORMAT_DEFAULT_VALUE UIResu::TEXT_FORMATS.at(L"微软雅黑/Light/16")

        Label(
            WstrParam text,
            D2D_RECT_F rect,
            ComPtrParam<IDWriteTextFormat> format = FORMAT_DEFAULT_VALUE,
            D2D1_COLOR_F foregroundColor = (D2D1::ColorF)D2D1::ColorF::Black,
            float foregroundOpacity = 1.0f,
            D2D1_COLOR_F backgroundColor = (D2D1::ColorF)D2D1::ColorF::White,
            float backgroundOpacity = 0.0f);

#undef FORMAT_DEFAULT_VALUE

        ComPtr<IDWriteTextFormat> format = {};

        D2D1_COLOR_F foregroundColor = {};
        float foregroundOpacity = {};

        struct Alignment
        {
            DWRITE_TEXT_ALIGNMENT horizontal = DWRITE_TEXT_ALIGNMENT_CENTER;
            DWRITE_PARAGRAPH_ALIGNMENT vertical = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
        }
        alignment = {};

        D2D1_DRAW_TEXT_OPTIONS options = D2D1_DRAW_TEXT_OPTIONS_CLIP;

        DWRITE_MEASURING_MODE measuringMode = DWRITE_MEASURING_MODE_NATURAL;

    private:
        Wstring m_text = {};

        DWRITE_TEXT_METRICS m_textMetrics = {};

    public:
        const Wstring& Text();
        void SetText(WstrViewParam text);
        void SetText(Wstring&& text);

        struct CharacterRange { size_t offset, count; };

        void AppendTextFragment(WstrViewParam fragment);
        void EraseTextFragment(const CharacterRange& range);
        void InsertTextFragment(WstrViewParam fragment, size_t offset);
        void ModifyTextFragment(WstrViewParam fragment, const CharacterRange& range);

        const DWRITE_TEXT_METRICS& TextLayoutMetrics();

        // This method is equal to TextLayoutMetrics when called with default parameters,
        // but the text layout metrics is recomputed internally at the cost of performance.
        DWRITE_TEXT_METRICS GetTextLayoutMetrics(
            OptParam<WstringView> text = std::nullopt,
            UINT32 characterOffset = 0,
            UINT32 characterCount = UINT32_MAX,
            OptParam<float> maxWidth = std::nullopt,
            OptParam<float> maxHeight = std::nullopt);

        // TODO: add support for searching multiline text's nearest character gap index.
        size_t GetNearestCharacterGapIndex(float selfCoordOffsetX);

    public:
        // Override interface methods.

        // IDrawObject2D
        void OnRendererDrawD2D1ObjectHelper(Renderer* rndr) override;

        // Panel
        void OnChangeThemeHelper(WstrViewParam themeName) override;
    };
}