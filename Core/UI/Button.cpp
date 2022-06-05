#include "Precompile.h"

#include "UI/Button.h"

#include "Renderer/MathUtils.h"
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
        m_takeOverChildrenDrawing = true;

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
    }

    void Button::OnInitializeFinish()
    {
        Panel::OnInitializeFinish();

        // Note OnChangeThemeHelper has been called in OnInitializeFinish,
        // so we need to update the appearance settings immediately here.
        UpdateAppearanceSetting(State::Idle);

        textLabel->SetParent(shared_from_this());
    }

    void Button::UpdateAppearanceSetting(State state)
    {
        auto& setting = appearances[(size_t)state];

        backgroundColor = setting.backgroundColor;
        backgroundOpacity = setting.backgroundOpacity;

        bitmap = setting.bitmap;
        bitmapOpacity = setting.bitmapOpacity;

        textLabel->foregroundColor = setting.foregroundColor;
        textLabel->foregroundOpacity = setting.foregroundOpacity;

        strokeColor = setting.strokeColor;
        strokeOpacity = setting.strokeOpacity;
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

    void Button::OnPressHelper(Event& e)
    {
        if (e.Left()) UpdateAppearanceSetting(State::Down);
    }

    void Button::OnReleaseHelper(Event& e)
    {
        if (e.Left()) UpdateAppearanceSetting(State::Hover);
    }

    void Button::OnRendererDrawD2D1ObjectHelper(Renderer* rndr)
    {
        // Background
        UIResu::SOLID_COLOR_BRUSH->SetColor(backgroundColor);
        UIResu::SOLID_COLOR_BRUSH->SetOpacity(backgroundOpacity);

        if (brush != nullptr)
        {
            rndr->d2d1DeviceContext->FillRoundedRectangle(
                { m_absoluteRect, roundRadiusX, roundRadiusY }, UIResu::SOLID_COLOR_BRUSH.Get());
        }
        if (bitmap != nullptr)
        {
            rndr->d2d1DeviceContext->DrawBitmap(bitmap.Get(), SelfCoordToAbsolute(iconRect), bitmapOpacity);
        }

        // Text
        if (textLabel != nullptr)
        {
            textLabel->OnRendererDrawD2D1Object(rndr);
        }

        // Outline
        UIResu::SOLID_COLOR_BRUSH->SetColor(strokeColor);
        UIResu::SOLID_COLOR_BRUSH->SetOpacity(strokeOpacity);

        auto innerRect = Mathu::Stretch(m_absoluteRect, { -strokeWidth * 0.5f, -strokeWidth * 0.5f });

        rndr->d2d1DeviceContext->DrawRoundedRectangle(
            { innerRect, roundRadiusX, roundRadiusY }, UIResu::SOLID_COLOR_BRUSH.Get(), strokeWidth);
    }

    void Button::OnSizeHelper(SizeEvent& e)
    {
        Panel::OnSizeHelper(e);

        textLabel->OnParentSize(e);
    }

    void Button::OnChangeThemeHelper(WstrViewParam themeName)
    {
        Panel::OnChangeThemeHelper(themeName);

        if (themeName == L"Light")
        {
            appearances[(size_t)State::Idle] =
            {
                (D2D1::ColorF)D2D1::ColorF::Gray, // background color
                0.0f, // background opacity
                appearances[(size_t)State::Idle].bitmap, // bitmap
                1.0f, // bitmap opacity
                (D2D1::ColorF)D2D1::ColorF::Black, // foreground color
                1.0f, // foreground opacity
                (D2D1::ColorF)D2D1::ColorF::Black, // stroke color
                0.0f, // stroke opacity
                0.0f // stroke width
            };
            appearances[(size_t)State::Hover] =
            {
                (D2D1::ColorF)D2D1::ColorF::Gray, // background color
                0.1f, // background opacity
                appearances[(size_t)State::Hover].bitmap, // bitmap
                1.0f, // bitmap opacity
                (D2D1::ColorF)D2D1::ColorF::Black, // foreground color
                1.0f, // foreground opacity
                (D2D1::ColorF)D2D1::ColorF::Black, // stroke color
                0.0f, // stroke opacity
                0.0f // stroke width
            };
            appearances[(size_t)State::Down] =
            {
                (D2D1::ColorF)D2D1::ColorF::Gray, // background color
                0.1f, // background opacity
                appearances[(size_t)State::Down].bitmap, // bitmap
                0.5f, // bitmap opacity
                (D2D1::ColorF)D2D1::ColorF::Black, // foreground color
                0.5f, // foreground opacity
                (D2D1::ColorF)D2D1::ColorF::Black, // stroke color
                0.0f, // stroke opacity
                0.0f // stroke width
            };
        }
        else if (themeName == L"Dark")
        {
            appearances[(size_t)State::Idle] =
            {
                (D2D1::ColorF)D2D1::ColorF::Gray, // background color
                0.0f, // background opacity
                appearances[(size_t)State::Idle].bitmap, // bitmap
                1.0f, // bitmap opacity
                { 0.9f, 0.9f, 0.9f, 1.0f }, // text color
                1.0f, // foreground opacity
                (D2D1::ColorF)D2D1::ColorF::Black, // stroke color
                0.0f, // stroke opacity
                0.0f // stroke width
            };
            appearances[(size_t)State::Hover] =
            {
                (D2D1::ColorF)D2D1::ColorF::Gray, // background color
                0.1f, // background opacity
                appearances[(size_t)State::Hover].bitmap, // bitmap
                1.0f, // bitmap opacity
                { 0.9f, 0.9f, 0.9f, 1.0f }, // text color
                1.0f, // foreground opacity
                (D2D1::ColorF)D2D1::ColorF::Black, // stroke color
                0.0f, // stroke opacity
                0.0f // stroke width
            };
            appearances[(size_t)State::Down] =
            {
                (D2D1::ColorF)D2D1::ColorF::Gray, // background color
                0.1f, // background opacity
                appearances[(size_t)State::Down].bitmap, // bitmap
                0.5f, // bitmap opacity
                { 0.9f, 0.9f, 0.9f, 1.0f }, // text color
                0.5f, // foreground opacity
                (D2D1::ColorF)D2D1::ColorF::Black, // stroke color
                0.0f, // stroke opacity
                0.0f // stroke width
            };
        }
        UpdateAppearanceSetting(State::Idle);
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