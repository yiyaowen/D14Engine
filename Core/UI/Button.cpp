#include "Precompile.h"

#include "UI/Button.h"

#include "UI/Application.h"

namespace d14engine::ui
{
    Button::Button(
        WstrParam text,
        const D2D1_RECT_F& rect,
        float roundRadius,
        ComPtrParam<ID2D1Bitmap1> icon)
        :
        Panel(rect, UIResu::SOLID_COLOR_BRUSH),
        iconRect(SelfCoordRect())
    {
        roundRadiusX = roundRadiusY = roundRadius;

        // Create text label. Only icon is displayed when text is empty.
        if (!text.empty())
        {
            textLabel = MakeUIObject<Label>(text, SelfCoordRect());

            // Keep the text always in the center of the button.
            textLabel->f_onParentSizeAfter = [this](Panel* p, SizeEvent& e)
            {
                p->Resize(e.size.width, e.size.height);
            };
        }
        appearances[(size_t)State::Idle] =
        {
            (D2D1::ColorF)D2D1::ColorF::Gray, // solid color
            0.0f, // solid color opaque
            icon, // bitmap
            1.0f, // bitmap opaque
            (D2D1::ColorF)D2D1::ColorF::Black, // text color
            1.0f, // text color opaque
            (D2D1::ColorF)D2D1::ColorF::Black, // stroke color
            0.0f, // stroke color opaque
            0.0f // stroke width
        };
        appearances[(size_t)State::Hover] =
        {
            (D2D1::ColorF)D2D1::ColorF::Gray, // solid color
            0.1f, // solid color opaque
            icon, // bitmap
            1.0f, // bitmap opaque
            (D2D1::ColorF)D2D1::ColorF::Black, // text color
            1.0f, // text color opaque
            (D2D1::ColorF)D2D1::ColorF::Black, // stroke color
            0.0f, // stroke color opaque
            0.0f // stroke width
        };
        appearances[(size_t)State::Down] =
        {
            (D2D1::ColorF)D2D1::ColorF::Gray, // solid color
            0.1f, // solid color opaque
            icon, // bitmap
            0.5f, // bitmap opaque
            (D2D1::ColorF)D2D1::ColorF::Black, // text color
            0.5f, // text color opaque
            (D2D1::ColorF)D2D1::ColorF::Black, // stroke color
            0.0f, // stroke color opaque
            0.0f // stroke width
        };
    }

    void Button::OnInitializeFinish()
    {
        textLabel->SetParent(shared_from_this());

        // Place appearance updating here instead of the end of ctor
        // so that the derived class could be updated automatically.
        UpdateAppearanceSetting(State::Idle);
    }

    void Button::UpdateAppearanceSetting(State state)
    {
        auto& setting = appearances[(size_t)state];

        solidColor = setting.solidColor;
        solidColorOpaque = setting.solidColorOpaque;

        bitmap = setting.bitmap;
        bitmapOpaque = setting.bitmapOpaque;

        textLabel->textColor = setting.textColor;
        textLabel->textColorOpaque = setting.textColorOpaque;

        strokeColor = setting.strokeColor;
        strokeColorOpaque = setting.strokeColorOpaque;
        strokeWidth = setting.strokeWidth;
    }

    void Button::OnPress(Event& e)
    {
        if (f_onPressOverride)
        {
            f_onPressOverride(this, e);
        }
        else
        {
            if (f_onPressBefore) f_onPressBefore(this, e);

            OnPressHelper(e);

            if (f_onPressAfter) f_onPressAfter(this, e);
        }
    }

    void Button::OnPressHelper(Event& e)
    {
        UpdateAppearanceSetting(State::Down);
    }

    void Button::OnRelease(Event& e)
    {
        if (f_onReleaseOverride)
        {
            f_onReleaseOverride(this, e);
        }
        else
        {
            if (f_onReleaseBefore) f_onReleaseBefore(this, e);

            OnReleaseHelper(e);

            if (f_onReleaseAfter) f_onReleaseAfter(this, e);
        }
    }

    void Button::OnReleaseHelper(Event& e)
    {
        UpdateAppearanceSetting(State::Hover);
    }

    void Button::OnRendererDrawD2D1Object(Renderer* rndr)
    {
        // Background
        UIResu::SOLID_COLOR_BRUSH->SetColor(solidColor);
        UIResu::SOLID_COLOR_BRUSH->SetOpacity(solidColorOpaque);

        if (brush != nullptr)
        {
            rndr->d2d1DeviceContext->FillRoundedRectangle(
                { m_absoluteRect, roundRadiusX, roundRadiusY }, UIResu::SOLID_COLOR_BRUSH.Get());
        }
        if (bitmap != nullptr)
        {
            rndr->d2d1DeviceContext->DrawBitmap(bitmap.Get(), SelfCoordToAbsolute(iconRect), bitmapOpaque);
        }

        // Text
        if (textLabel != nullptr)
        {
            textLabel->OnRendererDrawD2D1Object(rndr);
        }

        // Outline
        UIResu::SOLID_COLOR_BRUSH->SetColor(strokeColor);
        UIResu::SOLID_COLOR_BRUSH->SetOpacity(strokeColorOpaque);

        rndr->d2d1DeviceContext->DrawRoundedRectangle(
            { m_absoluteRect, roundRadiusX, roundRadiusY }, UIResu::SOLID_COLOR_BRUSH.Get(), strokeWidth);
    }

    void Button::OnSizeHelper(SizeEvent& e)
    {
        textLabel->OnParentSize(e);
    }

    bool Button::OnMouseButtonHelper(MouseButtonEvent& e)
    {
        Event be = {};

        if (e.status.LeftDown() || e.status.RightDown() || e.status.MiddleDown())
        {
            if (e.status.LeftDown())
            {
                m_hasLeftPressed = true;
                be.flag = Event::Flag::Left;
            }
            else if (e.status.RightDown())
            {
                m_hasRightPressed = true;
                be.flag = Event::Flag::Right;
            }
            else if (e.status.MiddleDown())
            {
                m_hasMiddlePressed = true;
                be.flag = Event::Flag::Middle;
            }
            else be.flag = Event::Flag::Unknown;

            OnPress(be);
        }
        else if (m_hasLeftPressed || m_hasRightPressed || m_hasMiddlePressed)
        {
            if (e.status.LeftUp() || e.status.RightUp() || e.status.MiddleUp())
            {
                if (e.status.LeftUp())
                {
                    m_hasLeftPressed = false;
                    be.flag = Event::Flag::Left;
                }
                else if (e.status.RightUp())
                {
                    m_hasRightPressed = false;
                    be.flag = Event::Flag::Right;
                }
                else if (e.status.MiddleUp())
                {
                    m_hasMiddlePressed = false;
                    be.flag = Event::Flag::Middle;
                }
                else be.flag = Event::Flag::Unknown;

                OnRelease(be);
            }
        }
        return Panel::OnMouseButtonHelper(e);
    }

    bool Button::OnMouseEnterHelper(MouseEnterEvent& e)
    {
        UpdateAppearanceSetting(State::Hover);

        return Panel::OnMouseEnterHelper(e);
    }

    bool Button::OnMouseLeaveHelper(MouseLeaveEvent& e)
    {
        m_hasLeftPressed = m_hasRightPressed = m_hasMiddlePressed = false;

        UpdateAppearanceSetting(State::Idle);

        return Panel::OnMouseLeaveHelper(e);
    }
}