#include "Common/Precompile.h"

#include "UIKit/TextBox.h"

#include "Renderer/MathUtils.h"
using namespace d14engine::renderer;

#include "UIKit/Application.h"

namespace d14engine::uikit
{
    TextBox::TextBox(const D2D1_RECT_F& rect, float roundRadius)
        :
        Panel(rect, Resu::SOLID_COLOR_BRUSH),
        m_visibleTextRect({ roundRadius, 0.0f, Width() - roundRadius, Height() }),
        m_visibleTextMaskStyle(Mathu::Rounding(VisibleTextWidth()), Mathu::Rounding(VisibleTextHeight()))
    {
        m_takeOverChildrenDrawing = true;
        m_skipChangeChildrenTheme = true;

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
                    Label::Infinity(), // Single line.
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

            m_hiliteTextLabel->alignment.horizontal = DWRITE_TEXT_ALIGNMENT_LEADING;
        }
    }

    void TextBox::OnInitializeFinish()
    {
        Panel::OnInitializeFinish();

        // Display the hilite text above the normal text.
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

        SetIndicatorPosition(0);
        SetHiliteTextRange({ 0, 0 });

        OnTextChange();
    }

    void TextBox::SetText(Wstring&& text)
    {
        m_textLabel->SetText(std::move(text));

        SetIndicatorPosition(0);
        SetHiliteTextRange({ 0, 0 });

        OnTextChange();
    }

    void TextBox::AppendTextFragment(WstrViewParam fragment)
    {
        m_textLabel->AppendTextFragment(fragment);

        OnTextChange();
    }

    void TextBox::EraseTextFragment(const CharacterRange& range)
    {
        m_textLabel->EraseTextFragment(range);

        OnTextChange();
    }

    void TextBox::InsertTextFragment(WstrViewParam fragment, size_t offset)
    {
        m_textLabel->InsertTextFragment(fragment, offset);

        OnTextChange();
    }

    void TextBox::ModifyTextFragment(WstrViewParam fragment, const CharacterRange& range)
    {
        m_textLabel->ModifyTextFragment(fragment, range);

        OnTextChange();
    }

    void TextBox::SetTextForegroundColor(const D2D1_COLOR_F& color)
    {
        m_textLabel->foreground.color = color;
    }

    void TextBox::SetTextForegroundOpacity(float opacity)
    {
        m_textLabel->foreground.opacity = opacity;
    }

    void TextBox::SetTextBackgroundColor(const D2D1_COLOR_F& color)
    {
        m_textLabel->background.color = color;
    }

    void TextBox::SetTextBackgroundOpacity(float opacity)
    {
        m_textLabel->background.opacity = opacity;
    }

    const Wstring& TextBox::HiliteText()
    {
        return m_hiliteTextLabel->Text();
    }

    void TextBox::SetHiliteText(WstrViewParam text)
    {
        m_hiliteTextLabel->SetText(text);
        
        m_textLabel->EraseTextFragment(m_hiliteTextRange);
        m_textLabel->InsertTextFragment(text, m_hiliteTextRange.offset);

        OnTextChange();
    }

    void TextBox::SetHiliteText(Wstring&& text)
    {
        m_hiliteTextLabel->SetText(text);

        m_textLabel->EraseTextFragment(m_hiliteTextRange);
        m_textLabel->InsertTextFragment(text, m_hiliteTextRange.offset);

        OnTextChange();
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
        
        D2D1_RECT_F hiliteRect = m_textLabel->RelativeRect();

        // nullopt to use the internally value.
        m_hiliteTextBackgroundDrawInfo.startOffsetX = m_textLabel->GetTextLayoutMetrics(
            std::nullopt, 0, (UINT32)m_hiliteTextRange.offset).widthIncludingTrailingWhitespace;

        // end count = start count + middle count
        m_hiliteTextBackgroundDrawInfo.endOffsetX = m_textLabel->GetTextLayoutMetrics(
            std::nullopt, 0, (UINT32)(m_hiliteTextRange.offset + m_hiliteTextRange.count)).widthIncludingTrailingWhitespace;
        
        hiliteRect.left += m_hiliteTextBackgroundDrawInfo.startOffsetX;
        hiliteRect.top += indicatorExternalY;
        hiliteRect.bottom -= indicatorExternalY;
        
        // Use transform instead of move here since the hilite text label
        // might haven't been prepared yet (it was initialized all-zero).
        m_hiliteTextLabel->Transform(hiliteRect);
    }

    void TextBox::SetHiliteTextForegroundColor(const D2D1_COLOR_F& color)
    {
        m_hiliteTextLabel->foreground.color = color;
    }

    void TextBox::SetHiliteTextForegroundOpacity(float opacity)
    {
        m_hiliteTextLabel->foreground.opacity = opacity;
    }

    void TextBox::SetHiliteTextBackgroundColor(const D2D1_COLOR_F& color)
    {
        m_hiliteTextBackgroundDrawInfo.color = color;
    }

    void TextBox::SetHiliteTextBackgroundOpacity(float opacity)
    {
        m_hiliteTextBackgroundDrawInfo.opacity = opacity;
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

    void TextBox::OnTextChange()
    {
        if (f_onTextChangeOverride)
        {
            f_onTextChangeOverride(this);
        }
        else
        {
            if (f_onTextChangeBefore) f_onTextChangeBefore(this);

            OnTextChangeHelper();

            if (f_onTextChangeAfter) f_onTextChangeAfter(this);
        }
    }

    void TextBox::OnTextChangeHelper()
    {
        // TODO: add text box text changing logic.
    }

    void TextBox::TriggerNormalInput(WstrViewParam content)
    {
        if (m_hiliteTextRange.count > 0)
        {
            m_textLabel->EraseTextFragment(m_hiliteTextRange);
            m_textLabel->InsertTextFragment(content, m_hiliteTextRange.offset);

            SetIndicatorPosition(m_hiliteTextRange.offset + content.size());
            SetHiliteTextRange({ 0, 0 });

            OnTextChange();
        }
        else // Insert new string in place.
        {
            m_textLabel->InsertTextFragment(content, m_indicatorOffsetCount);

            SetIndicatorPosition(m_indicatorOffsetCount + content.size());

            OnTextChange();
        }
    }

    void TextBox::TriggerControlCommands(KeyboardEvent& e)
    {
        switch (e.vkey)
        {
        case 'A': // Select All
        {
            SetIndicatorPosition(m_textLabel->Text().size());
            SetHiliteTextRange({ 0, m_textLabel->Text().size() });
            break;
        }
        case 'C': PerformCopyCommand(); break;
        case 'X': PerformCutCommand(); break;
        case 'V': PerformPasteCommand(); break;
        default: break;
        }
    }

    void TextBox::PerformCopyCommand()
    {
        Resu::SetClipboardText(m_hiliteTextLabel->Text());
    }

    void TextBox::PerformCutCommand()
    {
        Resu::SetClipboardText(m_hiliteTextLabel->Text());

        m_textLabel->EraseTextFragment(m_hiliteTextRange);

        SetIndicatorPosition(m_hiliteTextRange.offset);
        SetHiliteTextRange({ 0, 0 });

        OnTextChange();
    }

    void TextBox::PerformPasteCommand()
    {
        auto content = Resu::GetClipboardText();

        if (content.has_value())
        {
            if (m_hiliteTextRange.count > 0)
            {
                m_textLabel->EraseTextFragment(m_hiliteTextRange);
                m_textLabel->InsertTextFragment(content.value(), m_hiliteTextRange.offset);

                SetIndicatorPosition(m_hiliteTextRange.offset + content.value().size());
                SetHiliteTextRange({ 0, 0 });

                OnTextChange();
            }
            else // Insert from indicator position.
            {
                m_textLabel->InsertTextFragment(content.value(), m_indicatorOffsetCount);

                SetIndicatorPosition(m_indicatorOffsetCount + content.value().size());

                OnTextChange();
            }
        }
    }

    void TextBox::OnRendererUpdateObject2DHelper(Renderer* rndr)
    {
        Panel::OnRendererUpdateObject2DHelper(rndr);

        auto deltaSecs = (float)rndr->timer->deltaSecs;
        
        // Blink vertical indicator.
        if ((m_indicatorBlinkElapsedSecs += deltaSecs) >= indicatorBlinkIntervalSecs)
        {
            m_indicatorBlinkElapsedSecs = 0.0f;
            m_isIndicatorVisible = !m_isIndicatorVisible;
        }
    }

    void TextBox::OnRendererDrawD2D1LayerHelper(Renderer* rndr)
    {
        Panel::OnRendererDrawD2D1LayerHelper(rndr);

        // Only need to prepare layers for built-in text labels.
        {
            if (m_textLabel->IsD2D1ObjectVisible())
            {
                m_textLabel->OnRendererDrawD2D1Layer(rndr);
            }
            if (m_hiliteTextLabel->IsD2D1ObjectVisible())
            {
                m_hiliteTextLabel->OnRendererDrawD2D1Layer(rndr);
            }
        }
        auto visibleTextAreaLeftTop = VisibleTextAbsolutePosition();

        // Clamp all characters into visible text area.
        m_visibleTextMaskStyle.BeginMaskDraw(rndr->d2d1DeviceContext.Get(), D2D1::Matrix3x2F::Translation(-visibleTextAreaLeftTop.x, -visibleTextAreaLeftTop.y));
        {
            if (m_textLabel->IsD2D1ObjectVisible())
            {
                m_textLabel->OnRendererDrawD2D1Object(rndr);
            }
            // Draw hilite range's background rectangle manually.
            {
                Resu::SOLID_COLOR_BRUSH->SetColor(m_hiliteTextBackgroundDrawInfo.color);
                Resu::SOLID_COLOR_BRUSH->SetOpacity(m_hiliteTextBackgroundDrawInfo.opacity);

                float hiliteBkgnWidth = m_hiliteTextBackgroundDrawInfo.endOffsetX -
                                        m_hiliteTextBackgroundDrawInfo.startOffsetX;

                rndr->d2d1DeviceContext->FillRectangle(
                    m_hiliteTextLabel->SelfCoordToAbsolute(
                    {
                        0.0f, 0.0f, hiliteBkgnWidth,
                        m_hiliteTextLabel->Height()
                    }),
                    Resu::SOLID_COLOR_BRUSH.Get());
            }
            if (m_hiliteTextLabel->IsD2D1ObjectVisible())
            {
                m_hiliteTextLabel->OnRendererDrawD2D1Object(rndr);
            }
        }
        m_visibleTextMaskStyle.EndMaskDraw(rndr->d2d1DeviceContext.Get());
    }

    void TextBox::OnRendererDrawD2D1ObjectHelper(Renderer* rndr)
    {
        // Background
        Resu::SOLID_COLOR_BRUSH->SetColor(background.color);
        Resu::SOLID_COLOR_BRUSH->SetOpacity(background.opacity);

        Panel::DrawBackground(rndr);

        // Text
        rndr->d2d1DeviceContext->DrawBitmap(m_visibleTextMaskStyle.bitmap.Get(), VisibleTextAbsoluteRect());

        // Indicator
        if (m_showIndicator && m_isIndicatorVisible)
        {
            Resu::SOLID_COLOR_BRUSH->SetColor(indicatorColor);
            Resu::SOLID_COLOR_BRUSH->SetOpacity(indicatorOpacity);

            D2D1_POINT_2F up = { m_indicatorPositionX, indicatorExternalY };
            D2D1_POINT_2F down = { m_indicatorPositionX, Height() - indicatorExternalY };

            rndr->d2d1DeviceContext->DrawLine(
                SelfCoordToAbsolute(up), SelfCoordToAbsolute(down), Resu::SOLID_COLOR_BRUSH.Get());
        }
    }

    void TextBox::OnSizeHelper(SizeEvent& e)
    {
        Panel::OnSizeHelper(e);

        // Although m_visibleTextRect.left is always 0, e.size.width might be less than roundRadiusX,
        // which will cause m_visibleTextRect.right less than m_visibleTextRect.left, and thus
        // VisibleTextWidth() returns a negative value and finally overflows after type-casting.
        m_visibleTextRect.right = std::max(e.size.width - roundRadiusX, m_visibleTextRect.left);
        m_visibleTextRect.bottom = e.size.height;

        // Since Direct2D will render all primitives by subpixel, so do rounding to get better result.
        m_visibleTextMaskStyle.LoadMaskBitmap(Mathu::Rounding(VisibleTextWidth()), Mathu::Rounding(VisibleTextHeight()));
    }

    void TextBox::OnChangeThemeHelper(WstrViewParam themeName)
    {
        Panel::OnChangeThemeHelper(themeName);

        if (themeName == L"Light")
        {
            background.color = D2D1::ColorF{ 0xffffff };
            background.opacity = 1.0f;

            indicatorColor = D2D1::ColorF{ 0x000000 };
            indicatorOpacity = 1.0f;

            SetTextForegroundColor(D2D1::ColorF{ 0x000000 });
            SetTextForegroundOpacity(1.0f);
            SetTextBackgroundColor(D2D1::ColorF{ 0x000000 });
            SetTextBackgroundOpacity(0.0f);

            SetHiliteTextForegroundColor(D2D1::ColorF{ 0x000000 });
            SetHiliteTextForegroundOpacity(1.0f);
            SetHiliteTextBackgroundColor(D2D1::ColorF{ 0xadd6ff });
            SetHiliteTextBackgroundOpacity(1.0f);
        }
        else if (themeName == L"Dark")
        {
            background.color = D2D1::ColorF{ 0x000000 };
            background.opacity = 1.0f;

            indicatorColor = D2D1::ColorF{ 0xffffff };
            indicatorOpacity = 1.0f;

            SetTextForegroundColor(D2D1::ColorF{ 0xe5e5e5 });
            SetTextForegroundOpacity(1.0f);
            SetTextBackgroundColor(D2D1::ColorF{ 0x000000 });
            SetTextBackgroundOpacity(0.0f);

            SetHiliteTextForegroundColor(D2D1::ColorF{ 0xe5e5e5 });
            SetHiliteTextForegroundOpacity(1.0f);
            SetHiliteTextBackgroundColor(D2D1::ColorF{ 0x264f78 });
            SetHiliteTextBackgroundOpacity(1.0f);
        }
    }

    bool TextBox::OnGetFocusHelper()
    {
        m_showIndicator = true;

        Application::APP->IncreaseAnimateCount();

        return Panel::OnGetFocusHelper();
    }

    bool TextBox::OnLoseFocusHelper()
    {
        m_showIndicator = false;
        SetIndicatorPosition(0);
        SetHiliteTextRange({ 0, 0 });

        Application::APP->DecreaseAnimateCount();

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

            float textAreaStartX = m_textLabel->Position().x;
            float hiliteRangeStartX = AbsoluteToSelfCoord(e.cursorPoint).x - textAreaStartX;

            m_initialCharacterGapIndex = m_textLabel->GetNearestCharacterGapIndex(hiliteRangeStartX);

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
        }
        if (e.buttonState.leftPressed && Application::APP->IsUIObjectFocused(weak_from_this()))
        {
            // Force to show the indicator when it has been relocated.
            // In general, this will provide a better user experience.
            m_isIndicatorVisible = true;
            m_indicatorBlinkElapsedSecs = 0.0f;

            float textAreaStartX = m_textLabel->Position().x;
            float hiliteRangeEndX = AbsoluteToSelfCoord(e.cursorPoint).x - textAreaStartX;

            size_t currCharacterGapIndex = m_textLabel->GetNearestCharacterGapIndex(hiliteRangeEndX);

            SetIndicatorPosition(currCharacterGapIndex);
            SetHiliteTextRange(
            {
                std::min(currCharacterGapIndex, m_initialCharacterGapIndex),
                (size_t)std::abs((int)currCharacterGapIndex - (int)m_initialCharacterGapIndex)
            });
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
                    // Cast to signed integer in case the indicator offset count underflows.
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
                    if (m_hiliteTextRange.count > 0) // Remove hilite text range.
                    {
                        m_textLabel->EraseTextFragment(m_hiliteTextRange);

                        SetIndicatorPosition(m_hiliteTextRange.offset);
                        SetHiliteTextRange({ 0, 0 });

                        OnTextChange();
                    }
                    else if (m_indicatorOffsetCount > 0) // Remove single character.
                    {
                        m_textLabel->EraseTextFragment({ m_indicatorOffsetCount - 1, 1 });

                        SetIndicatorPosition(m_indicatorOffsetCount - 1);

                        OnTextChange();
                    }
                    break;
                }
                case VK_DELETE:
                {
                    if (m_hiliteTextRange.count > 0) // Remove hilite text range.
                    {
                        m_textLabel->EraseTextFragment(m_hiliteTextRange);

                        SetIndicatorPosition(m_hiliteTextRange.offset);
                        SetHiliteTextRange({ 0, 0 });

                        OnTextChange();
                    }
                    else if (m_indicatorOffsetCount >= 0 && // Remove single character.
                             m_textLabel->Text().size() > 0)
                    {
                        m_textLabel->EraseTextFragment({ m_indicatorOffsetCount, 1 });

                        OnTextChange();
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
                {
                    if (e.LCTRL() || e.RCTRL())
                    {
                        TriggerControlCommands(e);
                    }
                    break;
                }}
                m_isIndicatorVisible = true;
                m_indicatorBlinkElapsedSecs = 0.0f;
            }
        }
        return Panel::OnKeyboardHelper(e);
    }

    Optional<COMPOSITIONFORM> TextBox::GetCompositionForm()
    {
        COMPOSITIONFORM form = {};
        form.dwStyle = CFS_POINT;
        form.ptCurrentPos =
        {
            (LONG)(m_absoluteRect.left + m_indicatorPositionX),
            (LONG)m_absoluteRect.bottom
        };
        return form;
    }

    void TextBox::OnInputStringHelper(WstrViewParam content)
    {
        TextInputObject::OnInputStringHelper(content);

        // Ignore those unprintable control characters.
        if (content.size() != 1 || content[0] >= L' ')
        {
            TriggerNormalInput(content);
        }
    }
}