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
        Panel(rect, Resu::SOLID_COLOR_BRUSH),
        ClickablePanel(rect, Resu::SOLID_COLOR_BRUSH),
        mask(Mathu::Rounding(Width()), Mathu::Rounding(Height()))
    {
        m_takeOverChildrenDrawing = true;

        roundRadiusX = roundRadiusY = roundRadius;

        m_state = { State::ActiveFlag::Off, State::ButtonFlag::Idle };
    }

    void OnOffSwitch::OnRendererUpdateObject2DHelper(Renderer* rndr)
    {
        ClickablePanel::OnRendererUpdateObject2DHelper(rndr);

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

    void OnOffSwitch::OnRendererDrawD2D1LayerHelper(Renderer* rndr)
    {
        ClickablePanel::OnRendererDrawD2D1LayerHelper(rndr);

        // Hide children by default (no need to prepare children layers).
        mask.BeginMaskDraw(rndr->d2d1DeviceContext.Get(), D2D1::Matrix3x2F::Translation(-m_absoluteRect.left, -m_absoluteRect.top));
        {
            float selfWidth = Width(), selfHeight = Height();
            auto& selfRect = AbsoluteRect();

            auto& setting = appearances[m_state.Index()];
            // Use pointer instead of reference since this variable might be changed later.
            auto handleSettingPtr = &setting.handle;

            // Background
            Resu::SOLID_COLOR_BRUSH->SetColor(setting.background.color);
            Resu::SOLID_COLOR_BRUSH->SetOpacity(setting.background.opacity);

            Panel::DrawBackground(rndr);

            // Stroke
            Resu::SOLID_COLOR_BRUSH->SetColor(setting.stroke.color);
            Resu::SOLID_COLOR_BRUSH->SetOpacity(setting.stroke.opacity);

            auto innerRect = Mathu::Stretch(m_absoluteRect, { -setting.stroke.width * 0.5f, -setting.stroke.width * 0.5f });

            rndr->d2d1DeviceContext->DrawRoundedRectangle(
                { innerRect, roundRadiusX, roundRadiusY }, Resu::SOLID_COLOR_BRUSH.Get(), setting.stroke.width);

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
        mask.EndMaskDraw(rndr->d2d1DeviceContext.Get());
    }

    void OnOffSwitch::OnRendererDrawD2D1ObjectHelper(Renderer* rndr)
    {
        rndr->d2d1DeviceContext->DrawBitmap(mask.bitmap.Get(), m_absoluteRect, mask.opacity);
    }

    void OnOffSwitch::OnSizeHelper(SizeEvent& e)
    {
        ClickablePanel::OnSizeHelper(e);

        mask.LoadMaskBitmap(Mathu::Rounding(e.size.width), Mathu::Rounding(e.size.height));
    }

    void OnOffSwitch::OnChangeThemeHelper(WstrViewParam themeName)
    {
        ClickablePanel::OnChangeThemeHelper(themeName);

        if (themeName == L"Light")
        {
            appearances[(size_t)State::Flag::OnIdle] =
            {
                // background
                {
                    D2D1::ColorF{ 0xbf2424 }, // color
                    1.0f // opacity
                },
                // stroke
                {
                    0.0f, // width
                    D2D1::ColorF{ 0x000000 }, // color
                    0.0f // opacity
                },
                // handle appearance
                {
                    D2D1::ColorF{ 0xfafafa }, // color
                    1.0f, // opacity
                    { 16.0f, 16.0f }, // size
                    8.0f, // round radius
                    std::nullopt, // left offset
                    4.0f // right offset
                }
            };
            appearances[(size_t)State::Flag::OnHover] =
            {
                // background
                {
                    D2D1::ColorF{ 0xd92929 }, // color
                    1.0f // opacity
                },
                // stroke
                {
                    0.0f, // width
                    D2D1::ColorF{ 0x000000 }, // color
                    0.0f // opacity
                },
                // handle appearance
                {
                    D2D1::ColorF{ 0xfafafa }, // color
                    1.0f, // opacity
                    { 18.0f, 18.0f }, // size
                    9.0f, // round radius
                    std::nullopt, // left offset
                    4.0f // right offset
                }
            };
            appearances[(size_t)State::Flag::OnDown] =
            {
                // background
                {
                    D2D1::ColorF{ 0xcc2727 }, // color
                    1.0f // opacity
                },
                // stroke
                {
                    0.0f, // width
                    D2D1::ColorF{ 0x000000 }, // color
                    0.0f // opacity
                },
                // handle appearance
                {
                    D2D1::ColorF{ 0xfafafa }, // color
                    1.0f, // opacity
                    { 20.0f, 18.0f }, // size
                    9.0f, // round radius
                    std::nullopt, // left offset
                    4.0f // right offset
                }
            };
            appearances[(size_t)State::Flag::OffIdle] =
            {
                // background
                {
                    D2D1::ColorF{ 0xf3f3f3 }, // color
                    1.0f // opacity
                },
                // stroke
                {
                    1.5f, // width
                    D2D1::ColorF{ 0x8f8f8f }, // color
                    1.0f // opacity
                },
                // handle appearance
                {
                    D2D1::ColorF{ 0x5c5c5c }, // color
                    1.0f, // opacity
                    { 16.0f, 16.0f }, // size
                    8.0f, // round radius
                    4.0f, // left offset
                    std::nullopt // right offset
                }
            };
            appearances[(size_t)State::Flag::OffHover] =
            {
                // background
                {
                    D2D1::ColorF{ 0xebebeb }, // color
                    1.0f // opacity
                },
                // stroke
                {
                    1.5f, // width
                    D2D1::ColorF{ 0x8f8f8f }, // color
                    1.0f // opacity
                },
                // handle appearance
                {
                    D2D1::ColorF{ 0x575757 }, // color
                    1.0f, // opacity
                    { 18.0f, 18.0f }, // size
                    9.0f, // round radius
                    4.0f, // left offset
                    std::nullopt // right offset
                }
            };
            appearances[(size_t)State::Flag::OffDown] =
            {
                // background
                {
                    D2D1::ColorF{ 0xe3e3e3 }, // color
                    1.0f // opacity
                },
                // stroke
                {
                    1.5f, // width
                    D2D1::ColorF{ 0x8f8f8f }, // color
                    1.0f // opacity
                },
                // handle appearance
                {
                    D2D1::ColorF{ 0x525252 }, // color
                    1.0f, // opacity
                    { 20.0f, 18.0f }, // size
                    9.0f, // round radius
                    4.0f, // left offset
                    std::nullopt // right offset
                }
            };
        }
        else if (themeName == L"Dark")
        {
            appearances[(size_t)State::Flag::OnIdle] =
            {
                // background
                {
                    D2D1::ColorF{ 0x32995f }, // color
                    1.0f // opacity
                },
                // stroke
                {
                    0.0f, // width
                    D2D1::ColorF{ 0x000000 }, // color
                    0.0f // opacity
                },
                // handle appearance
                {
                    D2D1::ColorF{ 0x0d0d0d }, // color
                    1.0f, // opacity
                    { 16.0f, 16.0f }, // size
                    8.0f, // round radius
                    std::nullopt, // left offset
                    4.0f // right offset
                }
            };
            appearances[(size_t)State::Flag::OnHover] =
            {
                // background
                {
                    D2D1::ColorF{ 0x37a667 }, // color
                    1.0f // opacity
                },
                // stroke
                {
                    0.0f, // width
                    D2D1::ColorF{ 0x000000 }, // color
                    0.0f // opacity
                },
                // handle appearance
                {
                    D2D1::ColorF{ 0x0d0d0d }, // color
                    1.0f, // opacity
                    { 18.0f, 18.0f }, // size
                    9.0f, // round radius
                    std::nullopt, // left offset
                    4.0f // right offset
                }
            };
            appearances[(size_t)State::Flag::OnDown] =
            {
                // background
                {
                    D2D1::ColorF{ 0x2e8c57 }, // color
                    1.0f // opacity
                },
                // stroke
                {
                    0.0f, // width
                    D2D1::ColorF{ 0x000000 }, // color
                    0.0f // opacity
                },
                // handle appearance
                {
                    D2D1::ColorF{ 0x0d0d0d }, // color
                    1.0f, // opacity
                    { 20.0f, 18.0f }, // size
                    9.0f, // round radius
                    std::nullopt, // left offset
                    4.0f // right offset
                }
            };
            appearances[(size_t)State::Flag::OffIdle] =
            {
                // background
                {
                    D2D1::ColorF{ 0x1f1f1f }, // color
                    1.0f // opacity
                },
                // stroke
                {
                    1.5f, // width
                    D2D1::ColorF{ 0x7a7a7a }, // color
                    1.0f // opacity
                },
                // handle appearance
                {
                    D2D1::ColorF{ 0xa3a3a3 }, // color
                    1.0f, // opacity
                    { 16.0f, 16.0f }, // size
                    8.0f, // round radius
                    4.0f, // left offset
                    std::nullopt // right offset
                }
            };
            appearances[(size_t)State::Flag::OffHover] =
            {
                // background
                {
                    D2D1::ColorF{ 0x262626 }, // color
                    1.0f // opacity
                },
                // stroke
                {
                    1.5f, // width
                    D2D1::ColorF{ 0x7a7a7a }, // color
                    1.0f // opacity
                },
                // handle appearance
                {
                    D2D1::ColorF{ 0xa8a8a8 }, // color
                    1.0f, // opacity
                    { 18.0f, 18.0f }, // size
                    9.0f, // round radius
                    4.0f, // left offset
                    std::nullopt // right offset
                }
            };
            appearances[(size_t)State::Flag::OffDown] =
            {
                // background
                {
                    D2D1::ColorF{ 0x1a1a1a }, // color
                    1.0f // opacity
                },
                // stroke
                {
                    1.5f, // width
                    D2D1::ColorF{ 0x7a7a7a }, // color
                    1.0f // opacity
                },
                // handle appearance
                {
                    D2D1::ColorF{ 0x9e9e9e }, // color
                    1.0f, // opacity
                    { 20.0f, 18.0f }, // size
                    9.0f, // round radius
                    4.0f, // left offset
                    std::nullopt // right offset
                }
            };
        }
    }

    bool OnOffSwitch::OnMouseEnterHelper(MouseMoveEvent& e)
    {
        m_state.buttonFlag = State::ButtonFlag::Hover;

        return ClickablePanel::OnMouseEnterHelper(e);
    }

    bool OnOffSwitch::OnMouseLeaveHelper(MouseMoveEvent& e)
    {
        m_state.buttonFlag = State::ButtonFlag::Idle;

        return ClickablePanel::OnMouseLeaveHelper(e);
    }

    void OnOffSwitch::SetEnabled(bool value)
    {
        ClickablePanel::SetEnabled(value);

        mask.opacity = value ? 1.0f : 0.45f;

        m_state.buttonFlag = State::ButtonFlag::Idle;
    }

    void OnOffSwitch::OnMouseButtonPressHelper(ClickablePanel::Event& e)
    {
        ClickablePanel::OnMouseButtonPressHelper(e);

        m_state.buttonFlag = State::ButtonFlag::Down;
    }

    void OnOffSwitch::OnMouseButtonReleaseHelper(ClickablePanel::Event& e)
    {
        if (e.Left() && m_targetActiveState == State::ActiveFlag::Finished)
        {
            m_state.buttonFlag = State::ButtonFlag::Hover;

            // Switch current active state.
            if (m_state.activeFlag == State::ActiveFlag::On)
            {
                m_state.activeFlag = State::ActiveFlag::Off;
            }
            else m_state.activeFlag = State::ActiveFlag::On;

            // Trigger state changing event.
            StatefulObjectType::Event soe = {};
            soe.flag = m_state.activeFlag;

            OnStateChange(soe);

            // Update animation flag and condition.
            m_currHandleDisplacement = 0.0f;
            m_currHandleOffsetToLeft = 0.0f;

            m_targetActiveState = m_state.activeFlag;
            Application::APP->IncreaseAnimateCount();
        }
    }
}