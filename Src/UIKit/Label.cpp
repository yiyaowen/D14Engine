#include "Common/Precompile.h"

#include "UIKit/Label.h"

#include "UIKit/Application.h"

namespace d14engine::uikit
{
    Label::Label(
        WstrParam text,
        D2D_RECT_F rect,
        ComPtrParam<IDWriteTextFormat> format,
        const SolidStyle& foreground,
        const SolidStyle& background)
        :
        Panel(rect, Resu::SOLID_COLOR_BRUSH),
        m_text(text),
        format(format),
        foreground(foreground),
        background(background)
    {
        m_textMetrics = GetTextLayoutMetrics(text);
    }

    void Label::SetText(WstrViewParam text)
    {
        m_textMetrics = GetTextLayoutMetrics(m_text = text);
    }

    void Label::SetText(Wstring&& text)
    {
        m_textMetrics = GetTextLayoutMetrics(m_text = text);
    }

    void Label::AppendTextFragment(WstrViewParam fragment)
    {
        m_textMetrics = GetTextLayoutMetrics(m_text.append(fragment.data(), fragment.size()));
    }

    void Label::EraseTextFragment(const CharacterRange& range)
    {
        size_t validOffset = std::clamp<size_t>(range.offset, 0, m_text.size());
        size_t validCount = std::clamp<size_t>(range.count, 0, m_text.size() - validOffset);

        m_textMetrics = GetTextLayoutMetrics(m_text.erase(validOffset, validCount));
    }

    void Label::InsertTextFragment(WstrViewParam fragment, size_t offset)
    {
        m_textMetrics = GetTextLayoutMetrics(m_text.insert(offset, fragment.data(), fragment.size()));
    }

    void Label::ModifyTextFragment(WstrViewParam fragment, const CharacterRange& range)
    {
        size_t validOffset = std::clamp<size_t>(range.offset, 0, m_text.size());
        size_t validCount = std::clamp<size_t>(range.count, 0, std::min(m_text.size() - validOffset, fragment.size()));

        for (size_t i = 0; i < validCount; ++i)
        {
            m_text[i + validOffset] = fragment[i];
        }
        m_textMetrics = GetTextLayoutMetrics(m_text);
    }

    const Wstring& Label::Text()
    {
        return m_text;
    }

    const DWRITE_TEXT_METRICS& Label::TextLayoutMetrics()
    {
        return m_textMetrics;
    }

    DWRITE_TEXT_METRICS Label::GetTextLayoutMetrics(
        OptParam<WstringView> text,
        UINT32 characterOffset,
        UINT32 characterCount,
        OptParam<float> maxWidth,
        OptParam<float> maxHeight)
    {
        auto string = text.has_value() ? text.value().data() : m_text.c_str();
        auto stringLength = (UINT32)(text.has_value() ? text.value().size() : m_text.size());

        characterOffset = std::min(characterOffset, stringLength);
        characterCount = std::min(characterCount, stringLength - characterOffset);

        ComPtr<IDWriteTextLayout> layout;
        THROW_IF_FAILED(Application::APP->MainRenderer()->dWriteFactory->CreateTextLayout(
            string + characterOffset,
            characterCount,
            format.Get(),
            maxWidth.has_value() ? maxWidth.value() : Width(),
            maxHeight.has_value() ? maxHeight.value() : Height(),
            &layout));

        layout->SetTextAlignment(alignment.horizontal);
        layout->SetParagraphAlignment(alignment.vertical);

        DWRITE_TEXT_METRICS textMetrics;
        layout->GetMetrics(&textMetrics);

        return textMetrics;
    }

    size_t Label::GetNearestCharacterGapIndex(float selfCoordOffsetX)
    {
        size_t characterGapIndex = 0;

        float previousOffsetX = FLT_MAX;
        float previousDistance = FLT_MAX;

        // Search the closest character gap iteratively.
        for (size_t i = 0; i <= m_text.size(); ++i)
        {
            // Query text length with specified character count.
            auto metrics = GetTextLayoutMetrics(std::nullopt, 0, (UINT32)i);
            // Don't use width here. The trailing whitespace should also be taken into account.
            float currentOffsetX = metrics.left + metrics.widthIncludingTrailingWhitespace;

            float currentDistance = std::abs(currentOffsetX - selfCoordOffsetX);

            // Current distance is larger than previous means the gap is moving away from the point,
            // which also means that the previous gap is exactly the closest one to the point,
            // so we simply terminate the search and select the previous gap as the final result.
            if (currentDistance >= previousDistance)
            {
                characterGapIndex = i - 1;
                break;
            }
            else // Not the closest gap, so continue searching.
            {
                characterGapIndex = i;
            }
            previousOffsetX = currentOffsetX;
            previousDistance = currentDistance;
        }
        return characterGapIndex;
    }

    void Label::OnRendererDrawD2D1ObjectHelper(Renderer* rndr)
    {
        // There's no need to restore the color and opacity for the brush,
        // since they will always be reset at the beginning of next draw call.

        Resu::SOLID_COLOR_BRUSH->SetColor(background.color);
        Resu::SOLID_COLOR_BRUSH->SetOpacity(background.opacity);

        Panel::DrawBackground(rndr);

        // Also no need to restore the alignments for the text format.

        THROW_IF_FAILED(format->SetTextAlignment(alignment.horizontal));
        THROW_IF_FAILED(format->SetParagraphAlignment(alignment.vertical));

        Resu::SOLID_COLOR_BRUSH->SetColor(foreground.color);
        Resu::SOLID_COLOR_BRUSH->SetOpacity(foreground.opacity);

        rndr->d2d1DeviceContext->DrawTextW(
            m_text.c_str(),
            (UINT32)m_text.size(),
            format.Get(),
            m_absoluteRect,
            Resu::SOLID_COLOR_BRUSH.Get(),
            options,
            measuringMode);
    }

    void Label::OnChangeThemeHelper(WstrViewParam themeName)
    {
        Panel::OnChangeThemeHelper(themeName);

        if (themeName == L"Light")
        {
            foreground.color = D2D1::ColorF{ 0x000000 };
            foreground.opacity = 1.0f;

            background.color = D2D1::ColorF{ 0x000000 };
            background.opacity = 0.0f;
        }
        else if (themeName == L"Dark")
        {
            // Don't use full-white. The color should be softer in dark mode.
            foreground.color = D2D1::ColorF{ 0xe5e5e5 };
            foreground.opacity = 1.0f;

            background.color = D2D1::ColorF{ 0x000000 };
            background.opacity = 0.0f;
        }
    }
}