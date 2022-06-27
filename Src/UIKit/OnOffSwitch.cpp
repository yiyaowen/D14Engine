#include "Common/Precompile.h"

#include "UIKit/OnOffSwitch.h"

#include "Renderer/MathUtils.h"
using namespace d14engine::renderer;

#include "UIKit/AnimationUtils.h"
#include "UIKit/Application.h"
#include "UIKit/ResourceUtils.h"

namespace d14engine::uikit
{
    OnOffSwitch::OnOffSwitch(const D2D1_RECT_F& rect, float roundRadius)
        :
        Panel(rect, Resu::SOLID_COLOR_BRUSH)
    {
        roundRadiusX = roundRadiusY = roundRadius;

        m_state = { State::ActiveFlag::Off, State::ButtonFlag::Idle };
    }

    void OnOffSwitch::OnRendererUpdateObject2DHelper(Renderer* rndr)
    {
        Panel::OnRendererUpdateObject2DHelper(rndr);

        auto deltaSecs = (float)rndr->timer->deltaSecs;

        if (m_targetActiveState != State::ActiveFlag::Finished)
        {
            auto& onDownSetting = appearances[(size_t)State::Flag::OnDown].handle;
            auto& offDownSetting = appearances[(size_t)State::Flag::OffDown].handle;

            float firstOffset = onDownSetting.DisplacementToLeft(Width());
            float secondOffset = offDownSetting.DisplacementToLeft(Width());

            float handleWidth = onDownSetting.size.width;
            // Take the width of the handle itself into consideration.
            float totalDistance = std::abs(firstOffset - secondOffset) - handleWidth;

            float dx = Animu::AccelUniformDecelMotion(
                m_currHandleDisplacement,
                deltaSecs,
                totalDistance,
                handleVariableSpeedSecs,
                handleUniformSpeedSecs);

            // FIXME: To avoid too complex condition-branch here, we have assumed that
            // the off-down point is on the left and the on-down point is on the right.
            if (m_targetActiveState == State::ActiveFlag::On)
            {
                m_currHandleOffsetToLeft = secondOffset + dx;
            }
            else if (m_targetActiveState == State::ActiveFlag::Off)
            {
                m_currHandleOffsetToLeft = firstOffset - handleWidth - dx;
            }
            // Stop animation when the handle arrives destination.
            if ((m_currHandleDisplacement = dx) == totalDistance)
            {
                m_targetActiveState = State::ActiveFlag::Finished;
                Application::APP->DecreaseAnimateCount();
            }
        }
    }

    void OnOffSwitch::OnRendererDrawD2D1ObjectHelper(Renderer* rndr)
    {
        float selfWidth = Width(), selfHeight = Height();
        auto& selfRect = AbsoluteRect();

        auto& setting = appearances[m_state.Index()];
        // Use pointer instead of reference since this variable might be changed later.
        auto handleSettingPtr = &setting.handle;

        // Background
        Resu::SOLID_COLOR_BRUSH->SetColor(setting.backgroundColor);
        Resu::SOLID_COLOR_BRUSH->SetOpacity(setting.backgroundOpacity);

        Panel::DrawBackground(rndr);

        // Stroke
        Resu::SOLID_COLOR_BRUSH->SetColor(setting.strokeColor);
        Resu::SOLID_COLOR_BRUSH->SetOpacity(setting.strokeOpacity);

        auto innerRect = Mathu::Stretch(m_absoluteRect, { -setting.strokeWidth * 0.5f, -setting.strokeWidth * 0.5f });

        rndr->d2d1DeviceContext->DrawRoundedRectangle(
            { innerRect, roundRadiusX, roundRadiusY }, Resu::SOLID_COLOR_BRUSH.Get(), setting.strokeWidth);

        // Handle
        D2D1_RECT_F handleRect = {};

        if (m_targetActiveState == State::ActiveFlag::Finished)
        {
            if (handleSettingPtr->leftOffset.has_value())
            {
                handleRect.left = handleSettingPtr->leftOffset.value();
                handleRect.right = handleRect.left + handleSettingPtr->size.width;
            }
            else if (handleSettingPtr->rightOffset.has_value())
            {
                handleRect.right = selfWidth - handleSettingPtr->rightOffset.value();
                handleRect.left = handleRect.right - handleSettingPtr->size.width;
            }
        }
        else // The handle is performing animation.
        {
            size_t index = 0;
            if (m_targetActiveState == State::ActiveFlag::On)
            {
                index = (size_t)State::Flag::OnDown;
            }
            else index = (size_t)State::Flag::OffDown;

            handleSettingPtr = &appearances[index].handle;

            handleRect.left = m_currHandleOffsetToLeft;
            handleRect.right = handleRect.left + handleSettingPtr->size.width;
        }
        handleRect.top = (selfHeight - handleSettingPtr->size.height) * 0.5f;
        handleRect.bottom = handleRect.top + handleSettingPtr->size.height;

        handleRect = SelfCoordToAbsolute(handleRect);

        Resu::SOLID_COLOR_BRUSH->SetColor(handleSettingPtr->color);
        Resu::SOLID_COLOR_BRUSH->SetOpacity(handleSettingPtr->opacity);

        rndr->d2d1DeviceContext->FillRoundedRectangle(
            { handleRect, handleSettingPtr->roundRadius, handleSettingPtr->roundRadius }, Resu::SOLID_COLOR_BRUSH.Get());
    }

    void OnOffSwitch::OnChangeThemeHelper(WstrViewParam themeName)
    {
        Panel::OnChangeThemeHelper(themeName);

        if (themeName == L"Light")
        {
            appearances[(size_t)State::Flag::OnIdle] =
            {
                { 0.78f, 0.12f, 0.2f, 1.0f }, // background color
                1.0f, // background opacity
                (D2D1::ColorF)D2D1::ColorF::Black, // stroke color
                0.0f, // stroke opacity
                0.0f, // stroke width
                { // handle appearance
                    { 0.98f, 0.98f, 0.98f, 1.0f }, // color
                    1.0f, // opacity
                    { 16.0f, 16.0f }, // size
                    8.0f, // round radius
                    std::nullopt, // left offset
                    4.0f, // right offset
                }
            };
            appearances[(size_t)State::Flag::OnHover] =
            {
                { 0.82f, 0.1f, 0.22f, 1.0f }, // background color
                1.0f, // background opacity
                (D2D1::ColorF)D2D1::ColorF::Black, // stroke color
                0.0f, // stroke opacity
                0.0f, // stroke width
                { // handle appearance
                    { 0.98f, 0.98f, 0.98f, 1.0f }, // color
                    1.0f, // opacity
                    { 18.0f, 18.0f }, // size
                    9.0f, // round radius
                    std::nullopt, // left offset
                    4.0f, // right offset
                }
            };
            appearances[(size_t)State::Flag::OnDown] =
            {
                (D2D1::ColorF)D2D1::ColorF::Crimson, // background color
                1.0f, // background opacity
                (D2D1::ColorF)D2D1::ColorF::Black, // stroke color
                0.0f, // stroke opacity
                0.0f, // stroke width
                { // handle appearance
                    { 0.98f, 0.98f, 0.98f, 1.0f }, // color
                    1.0f, // opacity
                    { 20.0f, 18.0f }, // size
                    9.0f, // round radius
                    std::nullopt, // left offset
                    4.0f, // right offset
                }
            };
            appearances[(size_t)State::Flag::OffIdle] =
            {
                { 0.98f, 0.98f, 0.98f, 1.0f }, // background color
                1.0f, // background opacity
                { 0.7f, 0.7f, 0.7f, 1.0f }, // stroke color
                1.0f, // stroke opacity
                1.5f, // stroke width
                { // handle appearance
                    { 0.7f, 0.7f, 0.7f, 1.0f }, // color
                    1.0f, // opacity
                    { 16.0f, 16.0f }, // size
                    8.0f, // round radius
                    4.0f, // left offset
                    std::nullopt, // right offset
                }
            };
            appearances[(size_t)State::Flag::OffHover] =
            {
                { 0.96f, 0.96f, 0.96f, 1.0f }, // background color
                1.0f, // background opacity
                { 0.7f, 0.7f, 0.7f, 1.0f }, // stroke color
                1.0f, // stroke opacity
                1.5f, // stroke width
                { // handle appearance
                    { 0.7f, 0.7f, 0.7f, 1.0f }, // color
                    1.0f, // opacity
                    { 18.0f, 18.0f }, // size
                    9.0f, // round radius
                    4.0f, // left offset
                    std::nullopt, // right offset
                }
            };
            appearances[(size_t)State::Flag::OffDown] =
            {
                { 0.94f, 0.94f, 0.94f, 1.0f }, // background color
                1.0f, // background opacity
                { 0.7f, 0.7f, 0.7f, 1.0f }, // stroke color
                1.0f, // stroke opacity
                1.5f, // stroke width
                { // handle appearance
                    { 0.7f, 0.7f, 0.7f, 1.0f }, // color
                    1.0f, // opacity
                    { 20.0f, 18.0f }, // size
                    9.0f, // round radius
                    4.0f, // left offset
                    std::nullopt, // right offset
                }
            };
        }
        else if (themeName == L"Dark")
        {
            appearances[(size_t)State::Flag::OnIdle] =
            {
                { 0.2f, 0.62f, 0.39f, 1.0f }, // background color
                1.0f, // background opacity
                (D2D1::ColorF)D2D1::ColorF::Black, // stroke color
                0.0f, // stroke opacity
                0.0f, // stroke width
                { // handle appearance
                    { 0.1f, 0.1f, 0.1f, 1.0f }, // color
                    1.0f, // opacity
                    { 16.0f, 16.0f }, // size
                    8.0f, // round radius
                    std::nullopt, // left offset
                    4.0f, // right offset
                }
            };
            appearances[(size_t)State::Flag::OnHover] =
            {
                { 0.2f, 0.62f, 0.39f, 1.0f }, // background color
                0.9f, // background opacity
                (D2D1::ColorF)D2D1::ColorF::Black, // stroke color
                0.0f, // stroke opacity
                0.0f, // stroke width
                { // handle appearance
                    { 0.1f, 0.1f, 0.1f, 1.0f }, // color
                    1.0f, // opacity
                    { 18.0f, 18.0f }, // size
                    9.0f, // round radius
                    std::nullopt, // left offset
                    4.0f, // right offset
                }
            };
            appearances[(size_t)State::Flag::OnDown] =
            {
                { 0.2f, 0.62f, 0.39f, 1.0f }, // background color
                0.8f, // background opacity
                (D2D1::ColorF)D2D1::ColorF::Black, // stroke color
                0.0f, // stroke opacity
                0.0f, // stroke width
                { // handle appearance
                    { 0.1f, 0.1f, 0.1f, 1.0f }, // color
                    1.0f, // opacity
                    { 20.0f, 18.0f }, // size
                    9.0f, // round radius
                    std::nullopt, // left offset
                    4.0f, // right offset
                }
            };
            appearances[(size_t)State::Flag::OffIdle] =
            {
                { 0.06f, 0.06f, 0.06f, 1.0f }, // background color
                1.0f, // background opacity
                { 0.4f, 0.4f, 0.4f, 1.0f }, // stroke color
                1.0f, // stroke opacity
                1.5f, // stroke width
                { // handle appearance
                    { 0.4f, 0.4f, 0.4f, 1.0f }, // color
                    1.0f, // opacity
                    { 16.0f, 16.0f }, // size
                    8.0f, // round radius
                    4.0f, // left offset
                    std::nullopt, // right offset
                }
            };
            appearances[(size_t)State::Flag::OffHover] =
            {
                { 0.08f, 0.08f, 0.08f, 1.0f }, // background color
                1.0f, // background opacity
                { 0.4f, 0.4f, 0.4f, 1.0f }, // stroke color
                1.0f, // stroke opacity
                1.5f, // stroke width
                { // handle appearance
                    { 0.4f, 0.4f, 0.4f, 1.0f }, // color
                    1.0f, // opacity
                    { 18.0f, 18.0f }, // size
                    9.0f, // round radius
                    4.0f, // left offset
                    std::nullopt, // right offset
                }
            };
            appearances[(size_t)State::Flag::OffDown] =
            {
                { 0.1f, 0.1f, 0.1f, 1.0f }, // background color
                1.0f, // background opacity
                { 0.4f, 0.4f, 0.4f, 1.0f }, // stroke color
                1.0f, // stroke opacity
                1.5f, // stroke width
                { // handle appearance
                    { 0.4f, 0.4f, 0.4f, 1.0f }, // color
                    1.0f, // opacity
                    { 20.0f, 18.0f }, // size
                    9.0f, // round radius
                    4.0f, // left offset
                    std::nullopt, // right offset
                }
            };
        }
    }

    bool OnOffSwitch::OnMouseButtonHelper(MouseButtonEvent& e)
    {
        if (e.status.LeftDown())
        {
            m_state.buttonFlag = State::ButtonFlag::Down;

            m_hasLeftPressed = true;
        }
        else if (e.status.LeftUp())
        {
            if (m_hasLeftPressed && m_targetActiveState == State::ActiveFlag::Finished)
            {
                m_state.buttonFlag = State::ButtonFlag::Hover;

                m_hasLeftPressed = false;

                // Switch current active state.
                if (m_state.activeFlag == State::ActiveFlag::On)
                {
                    m_state.activeFlag = State::ActiveFlag::Off;
                }
                else m_state.activeFlag = State::ActiveFlag::On;

                // Trigger state changing event.
                Event e = {};
                e.flag = m_state.activeFlag;

                OnStateChange(e);

                // Update animation flag and condition.
                m_currHandleDisplacement = 0.0f;
                m_currHandleOffsetToLeft = 0.0f;

                m_targetActiveState = m_state.activeFlag;
                Application::APP->IncreaseAnimateCount();
            }
        }
        return Panel::OnMouseButtonHelper(e);
    }

    bool OnOffSwitch::OnMouseEnterHelper(MouseMoveEvent& e)
    {
        m_state.buttonFlag = State::ButtonFlag::Hover;

        return Panel::OnMouseEnterHelper(e);
    }

    bool OnOffSwitch::OnMouseLeaveHelper(MouseMoveEvent& e)
    {
        m_state.buttonFlag = State::ButtonFlag::Idle;

        m_hasLeftPressed = false;

        return Panel::OnMouseLeaveHelper(e);
    }
}