#include "Precompile.h"

#include "UI/TextBox.h"

#include "UI/Application.h"

namespace d14engine::ui
{
    TextBox::TextBox(const D2D1_RECT_F& rect, float roundRadius)
        :
        Panel(rect, UIResu::SOLID_COLOR_BRUSH),
        MaskStyle((UINT)(Width() - 2.0f * roundRadius), (UINT)Height()),
        SolidStyle((D2D1::ColorF)D2D1::ColorF::White, 1.0f),
        m_visibleTextRect({ roundRadius, 0.0f, Width() - roundRadius, Height() })
    {
        isFocusable = true;

        roundRadiusX = roundRadiusY = roundRadius;

        // Normal Text
        {
            m_textLabel = MakeUIObject<Label>(
                L"",
                D2D1_RECT_F
                {
                    m_visibleTextRect.left,
                    m_visibleTextRect.top,
                    FLT_MAX, // Single line.
                    m_visibleTextRect.bottom
                });

            m_textLabel->alignment.horizontal = DWRITE_TEXT_ALIGNMENT_LEADING;

            m_textLabel->f_onParentSizeBefore = [this](Panel* p, SizeEvent& e)
            {
                p->Resize(p->Width(), e.size.height);
            };
        }
        // Hilite Text
        {
            m_hiliteTextLabel = MakeUIObject<Label>(L"", D2D1_RECT_F{ 0.0f, 0.0f, 0.0f, 0.0f });

            m_hiliteTextLabel->textColor = (D2D1::ColorF)D2D1::ColorF::White;
            m_hiliteTextLabel->solidColor = (D2D1::ColorF)D2D1::ColorF::CornflowerBlue;
            m_hiliteTextLabel->solidColorOpaque = 1.0f;

            m_hiliteTextLabel->alignment.horizontal = DWRITE_TEXT_ALIGNMENT_LEADING;
        }
    }

    void TextBox::OnInitializeFinish()
    {
        // Show hilite text above normal text.
        m_textLabel->SetD2D1ObjectPriority(0);
        m_hiliteTextLabel->SetD2D1ObjectPriority(1);

        m_textLabel->SetParent(shared_from_this());
        m_hiliteTextLabel->SetParent(shared_from_this());
    }

    const Wstring& TextBox::Text()
    {
        return m_textLabel->Text();
    }

    void TextBox::SetText(WstrViewParam text)
    {
        m_textLabel->SetText(text);

        SetHiliteTextRange({ 0, 0 });
        SetIndicatorPosition(0);
    }

    void TextBox::SetText(Wstring&& text)
    {
        m_textLabel->SetText(std::move(text));

        SetHiliteTextRange({ 0, 0 });
        SetIndicatorPosition(0);
    }

    void TextBox::AppendTextFragment(WstrViewParam fragment)
    {
        m_textLabel->AppendTextFragment(fragment);
    }

    void TextBox::EraseTextFragment(const CharacterRange& range)
    {
        m_textLabel->EraseTextFragment(range);
    }

    void TextBox::InsertTextFragment(WstrViewParam fragment, size_t offset)
    {
        m_textLabel->InsertTextFragment(fragment, offset);
    }

    void TextBox::ModifyTextFragment(WstrViewParam fragment, const CharacterRange& range)
    {
        m_textLabel->ModifyTextFragment(fragment, range);
    }

    void TextBox::SetTextForegroundColor(const D2D1_COLOR_F& color)
    {
        m_textLabel->textColor = color;
    }

    void TextBox::SetTextForegroundColorOpaque(float opaque)
    {
        m_textLabel->textColorOpaque = opaque;
    }

    void TextBox::SetTextBackgroundColor(const D2D1_COLOR_F& color)
    {
        m_textLabel->solidColor = color;
    }

    void TextBox::SetTextBackgroundColorOpaque(float opaque)
    {
        m_textLabel->solidColorOpaque = opaque;
    }

    const Wstring& TextBox::HiliteText()
    {
        return m_hiliteTextLabel->Text();
    }

    void TextBox::SetHiliteText(WstrViewParam text)
    {
        m_hiliteTextLabel->SetText(text);
        
        EraseTextFragment(m_hiliteTextRange);
        InsertTextFragment(text, m_hiliteTextRange.offset);
    }

    void TextBox::SetHiliteText(Wstring&& text)
    {
        m_hiliteTextLabel->SetText(text);

        EraseTextFragment(m_hiliteTextRange);
        InsertTextFragment(text, m_hiliteTextRange.offset);
    }

    const TextBox::CharacterRange& TextBox::HiliteTextRange()
    {
        return m_hiliteTextRange;
    }

    void TextBox::SetHiliteTextRange(const CharacterRange& range)
    {
        auto& normalText = m_textLabel->Text();
        auto normalTextLen = normalText.size();

        m_hiliteTextRange.offset = std::clamp<size_t>(range.offset, 0, std::max<size_t>(normalTextLen - 1, 0));
        m_hiliteTextRange.count = std::clamp<size_t>(range.count, 0, normalTextLen - m_hiliteTextRange.offset);

        auto hiliteText = normalText.substr(m_hiliteTextRange.offset, m_hiliteTextRange.count);

        m_hiliteTextLabel->SetText(std::move(hiliteText));
        
        float textAreaStartX = m_textLabel->Position().x;

        // nullopt to use the internally value.
        float startCharacterOffsetX = m_textLabel->GetTextLayoutMetrics(
            std::nullopt, 0, (UINT32)m_hiliteTextRange.offset).widthIncludingTrailingWhitespace;
        // end count = start count + middle count
        float endCharacterOffsetX = m_textLabel->GetTextLayoutMetrics(
            std::nullopt, 0, (UINT32)(m_hiliteTextRange.offset + m_hiliteTextRange.count)).widthIncludingTrailingWhitespace;
        
        float hiliteRectLeft = textAreaStartX + startCharacterOffsetX;
        // FIXME: why is the queried size not enough to contain the text? 
        // This might be caused by the floating point error introduced when convert between rectangles.
        // I'd like to add a small increment to the queried width to fix this error for the time being.
        float hiliteRectWidth = endCharacterOffsetX - startCharacterOffsetX;
        if (hiliteRectWidth != 0.0f) hiliteRectWidth += 0.1f; // No need to adjust for empty hilite text.

        // The hilite rectangle should have the same height with the indicator.
        float hiliteRectTop = indicatorExternalY;
        float hiliteRectHeight = Height() - 2.0f * indicatorExternalY;

        m_hiliteTextLabel->Transform(hiliteRectLeft, hiliteRectTop, hiliteRectWidth, hiliteRectHeight);
    }

    void TextBox::SetHiliteTextForegroundColor(const D2D1_COLOR_F& color)
    {
        m_hiliteTextLabel->textColor = color;
    }

    void TextBox::SetHiliteTextForegroundColorOpaque(float opaque)
    {
        m_hiliteTextLabel->textColorOpaque = opaque;
    }

    void TextBox::SetHiliteTextBackgroundColor(const D2D1_COLOR_F& color)
    {
        m_hiliteTextLabel->solidColor = color;
    }

    void TextBox::SetHiliteTextBackgroundColorOpaque(float opaque)
    {
        m_hiliteTextLabel->solidColorOpaque = opaque;
    }

    void TextBox::SetTextFormat(ComPtrParam<IDWriteTextFormat> format)
    {
        m_textLabel->format = m_hiliteTextLabel->format = format;

        // Because texts with different formats might have different geometry metrics,
        // we have to update the hilite text's position and size to show it correctly.
        SetHiliteTextRange(m_hiliteTextRange);
    }

    void TextBox::SetIndicatorPosition(size_t characterOffset)
    {
        size_t normalTextLen = m_textLabel->Text().size();
        // Recompute the count of characters ahead of the indicator.
        m_indicatorOffsetCount = std::clamp<size_t>(characterOffset, 0, normalTextLen);

        // Relocate the indicator and clamp it to the visible text area.
        auto textAreaLeftTop = m_textLabel->Position();
        auto metrics = m_textLabel->GetTextLayoutMetrics(std::nullopt, 0, (UINT32)m_indicatorOffsetCount);

        // Don't use width here. The trailing whitespace should also be taken into account.
        float indicatorPositionX = textAreaLeftTop.x + metrics.widthIncludingTrailingWhitespace;

        float visibleTextAreaStartX = VisibleTextPosition().x;
        float visibleTextAreaEndX = visibleTextAreaStartX + VisibleTextWidth();

        m_indicatorPositionX = std::clamp(indicatorPositionX, visibleTextAreaStartX, visibleTextAreaEndX);

        float deltaX = 0.0f;
        // Move backend labels to show the visible text range.
        if (indicatorPositionX < visibleTextAreaStartX)
        {
            deltaX = visibleTextAreaStartX - indicatorPositionX;
        }
        else if (indicatorPositionX > visibleTextAreaEndX)
        {
            deltaX = visibleTextAreaEndX - indicatorPositionX;
        }
        auto hiliteLeftTop = m_hiliteTextLabel->Position();

        m_textLabel->Move(textAreaLeftTop.x + deltaX, textAreaLeftTop.y);
        m_hiliteTextLabel->Move(hiliteLeftTop.x + deltaX, hiliteLeftTop.y);
    }

    D2D1_RECT_F TextBox::VisibleTextAbsoluteRect()
    {
        return SelfCoordToAbsolute(m_visibleTextRect);
    }

    D2D1_POINT_2F TextBox::VisibleTextPosition()
    {
        return { m_visibleTextRect.left, m_visibleTextRect.top };
    }

    D2D1_POINT_2F TextBox::VisibleTextAbsolutePosition()
    {
        return SelfCoordToAbsolute(VisibleTextPosition());
    }

    float TextBox::VisibleTextWidth()
    {
        return m_visibleTextRect.right - m_visibleTextRect.left;
    }

    float TextBox::VisibleTextHeight()
    {
        return m_visibleTextRect.bottom - m_visibleTextRect.top;
    }

    D2D1_SIZE_F TextBox::VisibleTextSize()
    {
        return { VisibleTextWidth(), VisibleTextHeight() };
    }

    size_t TextBox::GetNearestCharacterGapIndex(float x)
    {
        size_t characterGapIndex = 0;

        float textAreaStartX = m_textLabel->Position().x;

        float previousOffsetX = FLT_MAX;
        float previousDistance = FLT_MAX;

        size_t normalTextLen = m_textLabel->Text().size();
        // Search the closest character gap iteratively.
        for (size_t i = 0; i <= normalTextLen; ++i)
        {
            // Query text length with specified character count.
            auto metrics = m_textLabel->GetTextLayoutMetrics(std::nullopt, 0, (UINT32)i);
            // Don't use width here. The trailing whitespace should also be taken into account.
            float currentOffsetX = textAreaStartX + metrics.widthIncludingTrailingWhitespace;

            float currentDistance = std::abs(currentOffsetX - x);

            // Current distance is larger than previous means the gap is moving away from the ibeam,
            // which also means that the previous gap is exactly the closest one to the ibeam,
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

    void TextBox::OnRendererUpdateObject2D(Renderer* rndr)
    {
        auto deltaSecs = (float)rndr->timer->deltaSecs;
        
        // Blink vertical indicator.
        if ((m_indicatorBlinkElapsedSecs += deltaSecs) >= indicatorBlinkIntervalSecs)
        {
            m_indicatorBlinkElapsedSecs = 0.0f;
            m_isIndicatorVisible = !m_isIndicatorVisible;
        }
    }

    void TextBox::OnRendererDrawD2D1Layer(Renderer* rndr)
    {
        Panel::DrawChildrenLayers(rndr);

        auto visibleTextAreaLeftTop = VisibleTextAbsolutePosition();
        // Children on Mask
        BeginDrawOnMask(rndr->d2d1DeviceContext.Get(), D2D1::Matrix3x2F::Translation(-visibleTextAreaLeftTop.x, -visibleTextAreaLeftTop.y));
        {
            Panel::DrawChildrenObjects(rndr);
        }
        EndDrawOnMask(rndr->d2d1DeviceContext.Get());
    }

    void TextBox::OnRendererDrawD2D1Object(Renderer* rndr)
    {
        // Background
        UIResu::SOLID_COLOR_BRUSH->SetColor(solidColor);
        UIResu::SOLID_COLOR_BRUSH->SetOpacity(solidColorOpaque);

        Panel::DrawBackground(rndr);

        // Text
        PostMaskToScene(rndr->d2d1DeviceContext.Get(), VisibleTextAbsoluteRect());

        // Indicator
        if (m_showIndicator && m_isIndicatorVisible)
        {
            UIResu::SOLID_COLOR_BRUSH->SetColor((D2D1::ColorF)D2D1::ColorF::Black);
            UIResu::SOLID_COLOR_BRUSH->SetOpacity(1.0f);

            D2D1_POINT_2F up = { m_indicatorPositionX, indicatorExternalY };
            D2D1_POINT_2F down = { m_indicatorPositionX, Height() - indicatorExternalY };

            rndr->d2d1DeviceContext->DrawLine(
                SelfCoordToAbsolute(up), SelfCoordToAbsolute(down), UIResu::SOLID_COLOR_BRUSH.Get());
        }
    }

    void TextBox::OnSizeHelper(SizeEvent& e)
    {
        Panel::OnSizeHelper(e);

        m_visibleTextRect.right = e.size.width - roundRadiusX;
        m_visibleTextRect.bottom = e.size.height;
    }

    bool TextBox::OnGetFocusHelper()
    {
        m_showIndicator = true;

        Application::IncreaseAnimateCount();

        return Panel::OnGetFocusHelper();
    }

    bool TextBox::OnLoseFocusHelper()
    {
        m_showIndicator = false;
        SetIndicatorPosition(0);
        SetHiliteTextRange({ 0, 0 });

        Application::DecreaseAnimateCount();

        return Panel::OnLoseFocusHelper();
    }

    bool TextBox::OnMouseButtonHelper(MouseButtonEvent& e)
    {
        if (e.status.LeftDown())
        {
            // Force to show the indicator when it has been relocated.
            // In general, this will provide a better user experience.
            m_isIndicatorVisible = true;
            m_indicatorBlinkElapsedSecs = 0.0f;

            m_initialCharacterGapIndex = GetNearestCharacterGapIndex(AbsoluteToSelfCoord(e.cursorPoint).x);

            SetIndicatorPosition(m_initialCharacterGapIndex);
            SetHiliteTextRange({ 0, 0 });
        }
        return Panel::OnMouseButtonHelper(e);
    }

    bool TextBox::OnMouseMoveHelper(MouseMoveEvent& e)
    {
        // When the text box is focused, it will keep receiving mouse-move events
        // even thought the cursor is out of the hit area, so we must check again
        // to make sure the ibeam icon would only be shown when cursor in the box.
        if (IsHit(e.cursorPoint))
        {
            Application::APP->MainCursor()->SetIcon(Cursor::IconIndex::Ibeam);

            if (e.buttonState.leftPressed)
            {
                // Force to show the indicator when it has been relocated.
                // In general, this will provide a better user experience.
                m_isIndicatorVisible = true;
                m_indicatorBlinkElapsedSecs = 0.0f;

                size_t currCharacterGapIndex = GetNearestCharacterGapIndex(AbsoluteToSelfCoord(e.cursorPoint).x);

                SetIndicatorPosition(currCharacterGapIndex);
                SetHiliteTextRange(
                {
                    std::min(currCharacterGapIndex, m_initialCharacterGapIndex),
                    (size_t)std::abs((int)currCharacterGapIndex - (int)m_initialCharacterGapIndex)
                });
            }
        }
        return Panel::OnMouseMoveHelper(e);
    }

    bool TextBox::OnKeyboardHelper(KeyboardEvent& e)
    {
        if (Application::APP->IsUIObjectFocused(weak_from_this()))
        {
            if (e.status.Pressed())
            {
                switch (e.vkey)
                {
                case VK_HOME:
                {
                    SetIndicatorPosition(0);
                    SetHiliteTextRange({ 0, 0 });
                    break;
                }
                case VK_LEFT:
                {
                    // In case the indicator offset count underflows.
                    SetIndicatorPosition((size_t)std::max((int)m_indicatorOffsetCount - 1, 0));
                    SetHiliteTextRange({ 0, 0 });
                    break;
                }
                case VK_RIGHT:
                {
                    SetIndicatorPosition(m_indicatorOffsetCount + 1);
                    SetHiliteTextRange({ 0, 0 });
                    break;
                }
                case VK_END:
                {
                    SetIndicatorPosition((int)m_textLabel->Text().size());
                    SetHiliteTextRange({ 0, 0 });
                    break;
                }
                case VK_BACK:
                {
                    if (m_indicatorOffsetCount > 0)
                    {
                        SetIndicatorPosition(m_indicatorOffsetCount - 1);
                        SetHiliteTextRange({ 0, 0 });

                        auto& originText = m_textLabel->Text();
                        Wstring headText(originText.begin(), originText.begin() + m_indicatorOffsetCount);
                        Wstring tailText(originText.begin() + m_indicatorOffsetCount + 1, originText.end());

                        m_textLabel->SetText(headText + tailText);
                    }
                    break;
                }
                case VK_DELETE:
                {
                    SetHiliteTextRange({ 0, 0 });

                    auto& originText = m_textLabel->Text();
                    Wstring headText(originText.begin(), originText.begin() + m_indicatorOffsetCount);

                    if (m_indicatorOffsetCount < originText.size())
                    {
                        Wstring tailText(originText.begin() + m_indicatorOffsetCount + 1, originText.end());
                        m_textLabel->SetText(headText + tailText);
                    }
                    else // Already at the right most end.
                    {
                        m_textLabel->SetText(headText);
                    }
                    break;
                }
                case VK_RETURN:
                {
                    Application::APP->FocusUIObjectLater({});
                    // We don't want to blink the indicator when lose focus,
                    // so return directly to skip resetting the indicator.
                    return Panel::OnKeyboardHelper(e);
                }
                default:
                    break;
                }
                m_isIndicatorVisible = true;
                m_indicatorBlinkElapsedSecs = 0.0f;
            }
        }
        return Panel::OnKeyboardHelper(e);
    }
}