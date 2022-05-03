#include "Precompile.h"

#include "UI/Label.h"

#include "UI/Application.h"

namespace d14engine::ui
{
    Label::Label(
        WstrParam text,
        D2D_RECT_F rect,
        D2D1_COLOR_F textColor,
        float textOpaque,
        D2D1_COLOR_F solidColor,
        float solidColorOpaque,
        ComPtrParam<IDWriteTextFormat> format)
        :
        Panel(rect, UIResu::SOLID_COLOR_BRUSH),
        SolidColorStyle(solidColor, solidColorOpaque),
        m_text(text),
        textColor(textColor),
        textOpaque(textOpaque),
        format(format)
    {
        m_textMetrics = GetTextLayoutMetrics();
    }

    DWRITE_TEXT_METRICS Label::GetTextLayoutMetrics()
    {
        ComPtr<IDWriteTextLayout> layout;
        Application::RNDR->dWriteFactory->CreateTextLayout(
            m_text.c_str(),
            (UINT32)m_text.size(),
            format.Get(),
            Width(),
            Height(),
            &layout);

        DWRITE_TEXT_METRICS metrics;
        layout->GetMetrics(&metrics);

        return metrics;
    }

    float Label::MinimalWidth()
    {
        return m_textMetrics.width;
    }

    float Label::MinimalHeight()
    {
        return m_textMetrics.height;
    }

    void Label::SetText(WstrParam text)
    {
        m_text = text;
        m_textMetrics = GetTextLayoutMetrics();
    }

    Wstring Label::Text()
    {
        return m_text;
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
        UIResu::SOLID_COLOR_BRUSH->SetOpacity(textOpaque);

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