#include "Precompile.h"

#include "UI/CheckBox.h"

#include "Renderer/MathUtils.h"
using namespace d14engine::renderer;

#include "UI/UIResourceUtils.h"

namespace d14engine::ui
{
    CheckBox::CheckBox(bool isThreeState, const D2D1_RECT_F& rect, float roundRadius)
        :
        Panel(rect, UIResu::SOLID_COLOR_BRUSH)
    {
        roundRadiusX = roundRadiusY = roundRadius;

        m_state = { State::ActiveFlag::Unchecked, State::ButtonFlag::Idle };

        EnableThreeState(isThreeState);
    }

    bool CheckBox::IsThreeState()
    {
        return m_isThreeState;
    }

    void CheckBox::EnableThreeState(bool value)
    {
        m_state.activeFlag = State::ActiveFlag::Unchecked;

        if (m_isThreeState = value)
        {
            m_stateTransitionMap =
            {
                { State::ActiveFlag::Unchecked, State::ActiveFlag::Intermidiate },
                { State::ActiveFlag::Intermidiate, State::ActiveFlag::Checked },
                { State::ActiveFlag::Checked, State::ActiveFlag::Unchecked }
            };
        }
        else // Use two-state model.
        {
            m_stateTransitionMap =
            {
                { State::ActiveFlag::Unchecked, State::ActiveFlag::Checked },
                { State::ActiveFlag::Checked, State::ActiveFlag::Unchecked }
            };
        }
    }

    void CheckBox::DrawIntermidiateIcon(Renderer* rndr)
    {
        auto& style = iconStyle.intermidiate;

        D2D1_RECT_F iconRect = {};
        iconRect.left = m_absoluteRect.left + (Width() - style.size.width) * 0.5f;
        iconRect.right = iconRect.left + style.size.width;
        iconRect.top = m_absoluteRect.top + (Height() - style.size.height) * 0.5f;
        iconRect.bottom = iconRect.top + style.size.height;

        auto& setting = appearances[m_state.Index()];

        UIResu::SOLID_COLOR_BRUSH->SetColor(setting.foregroundColor);
        UIResu::SOLID_COLOR_BRUSH->SetOpacity(setting.foregroundOpacity);

        rndr->d2d1DeviceContext->FillRectangle(iconRect, UIResu::SOLID_COLOR_BRUSH.Get());
    }

    void CheckBox::DrawCheckedIcon(Renderer* rndr)
    {
        auto& style = iconStyle.checked;

        auto position = AbsolutePosition();
        auto point0 = Mathu::Offset(position, style.tickLine0.point0);
        auto point1 = Mathu::Offset(position, style.tickLine0.point1);
        auto point2 = Mathu::Offset(position, style.tickLine1.point0);
        auto point3 = Mathu::Offset(position, style.tickLine1.point1);

        auto& setting = appearances[m_state.Index()];

        UIResu::SOLID_COLOR_BRUSH->SetColor(setting.foregroundColor);
        UIResu::SOLID_COLOR_BRUSH->SetOpacity(setting.foregroundOpacity);

        rndr->d2d1DeviceContext->DrawLine(point0, point1, UIResu::SOLID_COLOR_BRUSH.Get(), style.strokeWidth);
        rndr->d2d1DeviceContext->DrawLine(point2, point3, UIResu::SOLID_COLOR_BRUSH.Get(), style.strokeWidth);
    }

    void CheckBox::OnRendererDrawD2D1ObjectHelper(Renderer* rndr)
    {
        auto& setting = appearances[m_state.Index()];

        // Background
        UIResu::SOLID_COLOR_BRUSH->SetColor(setting.backgroundColor);
        UIResu::SOLID_COLOR_BRUSH->SetOpacity(setting.backgroundOpacity);

        if (brush != nullptr)
        {
            rndr->d2d1DeviceContext->FillRoundedRectangle(
                { m_absoluteRect, roundRadiusX, roundRadiusY }, UIResu::SOLID_COLOR_BRUSH.Get());
        }
        if (bitmap != nullptr)
        {
            rndr->d2d1DeviceContext->DrawBitmap(bitmap.Get(), AbsoluteRect(), bitmapOpacity);
        }

        // Icon
        if (m_state.activeFlag == State::ActiveFlag::Intermidiate) DrawIntermidiateIcon(rndr);
        else if (m_state.activeFlag == State::ActiveFlag::Checked) DrawCheckedIcon(rndr);

        // Outline
        UIResu::SOLID_COLOR_BRUSH->SetColor(setting.strokeColor);
        UIResu::SOLID_COLOR_BRUSH->SetOpacity(setting.strokeOpacity);

        auto innerRect = Mathu::Stretch(m_absoluteRect, { -setting.strokeWidth * 0.5f, -setting.strokeWidth * 0.5f });

        rndr->d2d1DeviceContext->DrawRoundedRectangle(
            { innerRect, roundRadiusX, roundRadiusY }, UIResu::SOLID_COLOR_BRUSH.Get(), setting.strokeWidth);
    }

    void CheckBox::OnChangeThemeHelper(WstrViewParam themeName)
    {
        Panel::OnChangeThemeHelper(themeName);

        if (themeName == L"Light")
        {
            appearances[(size_t)State::Flag::UncheckedIdle] =
            {
                { 0.98f, 0.98f, 0.98f, 1.0f }, // background color
                1.0f, // background opacity
                appearances[(size_t)State::Flag::UncheckedIdle].bitmap, // bitmap
                1.0f, // bitmap opacity
                (D2D1::ColorF)D2D1::ColorF::Black, // foreground color
                0.0f, // foreground opacity
                { 0.7f, 0.7f, 0.7f, 1.0f }, // stroke color
                1.0f, // stroke opacity
                1.0f // stroke width
            };
            appearances[(size_t)State::Flag::UncheckedHover] =
            {
                { 0.95f, 0.95f, 0.95f, 1.0f }, // background color
                1.0f, // background opacity
                appearances[(size_t)State::Flag::UncheckedHover].bitmap, // bitmap
                1.0f, // bitmap opacity
                (D2D1::ColorF)D2D1::ColorF::Black, // foreground color
                0.0f, // foreground opacity
                { 0.7f, 0.7f, 0.7f, 1.0f }, // stroke color
                1.0f, // stroke opacity
                1.0f // stroke width
            };
            appearances[(size_t)State::Flag::UncheckedDown] =
            {
                { 0.95f, 0.95f, 0.95f, 1.0f }, // background color
                1.0f, // background opacity
                appearances[(size_t)State::Flag::UncheckedDown].bitmap, // bitmap
                1.0f, // bitmap opacity
                (D2D1::ColorF)D2D1::ColorF::Black, // foreground color
                0.0f, // foreground opacity
                { 0.8f, 0.8f, 0.8f, 1.0f }, // stroke color
                1.0f, // stroke opacity
                1.0f // stroke width
            };
            ComPtr<ID2D1Bitmap1> originalBitmap;
            {
                originalBitmap = appearances[(size_t)State::Flag::CheckedIdle].bitmap;

                appearances[(size_t)State::Flag::IntermidiateIdle] =
                appearances[(size_t)State::Flag::CheckedIdle] =
                {
                    { 0.78f, 0.12f, 0.2f, 1.0f }, // background color
                    1.0f, // background opacity
                    appearances[(size_t)State::Flag::IntermidiateIdle].bitmap, // bitmap
                    1.0f, // bitmap opacity
                    { 0.98f, 0.98f, 0.98f, 1.0f }, // foreground color
                    1.0f, // foreground opacity
                    (D2D1::ColorF)D2D1::ColorF::Black, // stroke color
                    0.0f, // stroke opacity
                    0.0f // stroke width
                };
                appearances[(size_t)State::Flag::CheckedIdle].bitmap = originalBitmap;
            }
            {
                originalBitmap = appearances[(size_t)State::Flag::CheckedHover].bitmap;

                appearances[(size_t)State::Flag::IntermidiateHover] =
                appearances[(size_t)State::Flag::CheckedHover] =
                {
                    { 0.82f, 0.1f, 0.22f, 1.0f }, // background color
                    1.0f, // background opacity
                    appearances[(size_t)State::Flag::IntermidiateHover].bitmap, // bitmap
                    0.9f, // bitmap opacity
                    { 0.98f, 0.98f, 0.98f, 1.0f }, // foreground color
                    0.9f, // foreground opacity
                    (D2D1::ColorF)D2D1::ColorF::Black, // stroke color
                    0.0f, // stroke opacity
                    0.0f // stroke width
                };
                appearances[(size_t)State::Flag::CheckedHover].bitmap = originalBitmap;
            }
            {
                originalBitmap = appearances[(size_t)State::Flag::CheckedDown].bitmap;

                appearances[(size_t)State::Flag::IntermidiateDown] =
                appearances[(size_t)State::Flag::CheckedDown] =
                {
                    (D2D1::ColorF)D2D1::ColorF::Crimson, // background color
                    1.0f, // background opacity
                    appearances[(size_t)State::Flag::IntermidiateDown].bitmap, // bitmap
                    0.8f, // bitmap opacity
                    { 0.98f, 0.98f, 0.98f, 1.0f }, // foreground color
                    0.8f, // foreground opacity
                    (D2D1::ColorF)D2D1::ColorF::Black, // stroke color
                    0.0f, // stroke opacity
                    0.0f // stroke width
                };
                appearances[(size_t)State::Flag::CheckedDown].bitmap = originalBitmap;
            }
        }
        else if (themeName == L"Dark")
        {
            appearances[(size_t)State::Flag::UncheckedIdle] =
            {
                { 0.06f, 0.06f, 0.06f, 1.0f }, // background color
                1.0f, // background opacity
                appearances[(size_t)State::Flag::UncheckedIdle].bitmap, // bitmap
                1.0f, // bitmap opacity
                (D2D1::ColorF)D2D1::ColorF::Black, // foreground color
                0.0f, // foreground opacity
                { 0.4f, 0.4f, 0.4f, 1.0f }, // stroke color
                1.0f, // stroke opacity
                1.0f // stroke width
            };
            appearances[(size_t)State::Flag::UncheckedHover] =
            {
                { 0.08f, 0.08f, 0.08f, 1.0f }, // background color
                1.0f, // background opacity
                appearances[(size_t)State::Flag::UncheckedHover].bitmap, // bitmap
                1.0f, // bitmap opacity
                (D2D1::ColorF)D2D1::ColorF::Black, // foreground color
                0.0f, // foreground opacity
                { 0.4f, 0.4f, 0.4f, 1.0f }, // stroke color
                1.0f, // stroke opacity
                1.0f // stroke width
            };
            appearances[(size_t)State::Flag::UncheckedDown] =
            {
                { 0.1f, 0.1f, 0.1f, 1.0f }, // background color
                1.0f, // background opacity
                appearances[(size_t)State::Flag::UncheckedDown].bitmap, // bitmap
                1.0f, // bitmap opacity
                (D2D1::ColorF)D2D1::ColorF::Black, // foreground color
                0.0f, // foreground opacity
                { 0.3f, 0.3f, 0.3f, 1.0f }, // stroke color
                1.0f, // stroke opacity
                1.0f // stroke width
            };
            ComPtr<ID2D1Bitmap1> originalBitmap;
            {
                originalBitmap = appearances[(size_t)State::Flag::CheckedIdle].bitmap;

                appearances[(size_t)State::Flag::IntermidiateIdle] =
                appearances[(size_t)State::Flag::CheckedIdle] =
                {
                    { 0.2f, 0.62f, 0.39f, 1.0f }, // background color
                    1.0f, // background opacity
                    appearances[(size_t)State::Flag::IntermidiateIdle].bitmap, // bitmap
                    1.0f, // bitmap opacity
                    { 0.1f, 0.1f, 0.1f, 1.0f }, // foreground color
                    1.0f, // foreground opacity
                    (D2D1::ColorF)D2D1::ColorF::Black, // stroke color
                    0.0f, // stroke opacity
                    0.0f // stroke width
                };
                appearances[(size_t)State::Flag::CheckedIdle].bitmap = originalBitmap;
            }
            {
                originalBitmap = appearances[(size_t)State::Flag::CheckedHover].bitmap;

                appearances[(size_t)State::Flag::IntermidiateHover] =
                appearances[(size_t)State::Flag::CheckedHover] =
                {
                    { 0.2f, 0.62f, 0.39f, 1.0f }, // background color
                    0.9f, // background opacity
                    appearances[(size_t)State::Flag::IntermidiateHover].bitmap, // bitmap
                    1.0f, // bitmap opacity
                    { 0.1f, 0.1f, 0.1f, 1.0f }, // foreground color
                    1.0f, // foreground opacity
                    (D2D1::ColorF)D2D1::ColorF::Black, // stroke color
                    0.0f, // stroke opacity
                    0.0f // stroke width
                };
                appearances[(size_t)State::Flag::CheckedHover].bitmap = originalBitmap;
            }
            {
                originalBitmap = appearances[(size_t)State::Flag::CheckedDown].bitmap;

                appearances[(size_t)State::Flag::IntermidiateDown] =
                appearances[(size_t)State::Flag::CheckedDown] =
                {
                    { 0.2f, 0.62f, 0.39f, 1.0f }, // background color
                    0.8f, // background opacity
                    appearances[(size_t)State::Flag::IntermidiateDown].bitmap, // bitmap
                    1.0f, // bitmap opacity
                    { 0.1f, 0.1f, 0.1f, 1.0f }, // foreground color
                    1.0f, // foreground opacity
                    (D2D1::ColorF)D2D1::ColorF::Black, // stroke color
                    0.0f, // stroke opacity
                    0.0f // stroke width
                };
                appearances[(size_t)State::Flag::CheckedDown].bitmap = originalBitmap;
            }
        }
    }

    bool CheckBox::OnMouseButtonHelper(MouseButtonEvent& e)
    {
        if (e.status.LeftDown())
        {
            m_state.buttonFlag = State::ButtonFlag::Down;

            m_hasLeftPressed = true;
        }
        else if (e.status.LeftUp())
        {
            if (m_hasLeftPressed)
            {
                m_state.buttonFlag = State::ButtonFlag::Hover;

                m_hasLeftPressed = false;

                // Change current active state.
                m_state.activeFlag = m_stateTransitionMap[m_state.activeFlag];

                // Trigger state changing event.
                Event e = {};
                e.flag = m_state.activeFlag;

                OnStateChange(e);
            }
        }
        return Panel::OnMouseButtonHelper(e);
    }

    bool CheckBox::OnMouseEnterHelper(MouseMoveEvent& e)
    {
        m_state.buttonFlag = State::ButtonFlag::Hover;

        return Panel::OnMouseEnterHelper(e);
    }

    bool CheckBox::OnMouseLeaveHelper(MouseMoveEvent& e)
    {
        m_state.buttonFlag = State::ButtonFlag::Idle;

        m_hasLeftPressed = false;

        return Panel::OnMouseLeaveHelper(e);
    }
}