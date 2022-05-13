#include "Precompile.h"

#include "UI/Label.h"

#include "UI/Application.h"

namespace d14engine::ui
{
    Label::Label(
        WstrParam text,
        D2D_RECT_F rect,
        D2D1_COLOR_F foregroundColor,
        float foregroundColorOpaque,
        D2D1_COLOR_F backgroundColor,
        float backgroundColorOpaque,
        ComPtrParam<IDWriteTextFormat> format)
        :
        Panel(rect, UIResu::SOLID_COLOR_BRUSH),
        SolidStyle(backgroundColor, backgroundColorOpaque),
        m_text(text),
        textColor(foregroundColor),
        textColorOpaque(foregroundColorOpaque),
        format(format)
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
        THROW_IF_FAILED(Application::RENDERER->dWriteFactory->CreateTextLayout(
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

    void Label::OnRendererDrawD2D1Object(Renderer* rndr)
    {
        // There's no need to restore the color and opaque value for the brush,
        // since they will always be reset at the beginning of next draw call.

        UIResu::SOLID_COLOR_BRUSH->SetColor(solidColor);
        UIResu::SOLID_COLOR_BRUSH->SetOpacity(solidColorOpaque);

        rndr->d2d1DeviceContext->FillRectangle(m_absoluteRect, UIResu::SOLID_COLOR_BRUSH.Get());

        // Also no need to restore the alignments for the text format.

        THROW_IF_FAILED(format->SetTextAlignment(alignment.horizontal));
        THROW_IF_FAILED(format->SetParagraphAlignment(alignment.vertical));

        UIResu::SOLID_COLOR_BRUSH->SetColor(textColor);
        UIResu::SOLID_COLOR_BRUSH->SetOpacity(textColorOpaque);

        rndr->d2d1DeviceContext->DrawTextW(
            m_text.c_str(),
            (UINT32)m_text.size(),
            format.Get(),
            m_absoluteRect,
            UIResu::SOLID_COLOR_BRUSH.Get(),
            options,
            measuringMode);
    }
}