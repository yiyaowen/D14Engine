#include "Common/Precompile.h"

#include "UIKit/CheckBox.h"

#include "Renderer/MathUtils.h"
using namespace d14engine::renderer;

#include "UIKit/ResourceUtils.h"

namespace d14engine::uikit
{
    CheckBox::CheckBox(bool isTripleState, const D2D1_RECT_F& rect, float roundRadius)
        :
        Panel(rect, Resu::SOLID_COLOR_BRUSH),
        ClickablePanel(rect, Resu::SOLID_COLOR_BRUSH),
        mask(Mathu::Rounding(Width()), Mathu::Rounding(Height()))
    {
        m_takeOverChildrenDrawing = true;

        roundRadiusX = roundRadiusY = roundRadius;

        m_state = { State::ActiveFlag::Unchecked, State::ButtonFlag::Idle };

        EnableTripleState(isTripleState);
    }

    bool CheckBox::IsTripleState()
    {
        return m_isTripleState;
    }

    void CheckBox::EnableTripleState(bool value)
    {
        m_state.activeFlag = State::ActiveFlag::Unchecked;

        if (m_isTripleState = value)
        {
            m_stateTransitionMap =
            {
                { State::ActiveFlag::Unchecked, State::ActiveFlag::Intermidiate },
                { State::ActiveFlag::Intermidiate, State::ActiveFlag::Checked },
                { State::ActiveFlag::Checked, State::ActiveFlag::Unchecked }
            };
        }
        else // Use double-state model.
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

        Resu::SOLID_COLOR_BRUSH->SetColor(setting.foreground.color);
        Resu::SOLID_COLOR_BRUSH->SetOpacity(setting.foreground.opacity);

        rndr->d2d1DeviceContext->FillRectangle(iconRect, Resu::SOLID_COLOR_BRUSH.Get());
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

        Resu::SOLID_COLOR_BRUSH->SetColor(setting.foreground.color);
        Resu::SOLID_COLOR_BRUSH->SetOpacity(setting.foreground.opacity);

        rndr->d2d1DeviceContext->DrawLine(point0, point1, Resu::SOLID_COLOR_BRUSH.Get(), style.width);
        rndr->d2d1DeviceContext->DrawLine(point2, point3, Resu::SOLID_COLOR_BRUSH.Get(), style.width);
    }

    void CheckBox::OnRendererDrawD2D1LayerHelper(Renderer* rndr)
    {
        ClickablePanel::OnRendererDrawD2D1LayerHelper(rndr);

        // Hide children by default (no need to prepare children layers).
        mask.BeginMaskDraw(rndr->d2d1DeviceContext.Get(), D2D1::Matrix3x2F::Translation(-m_absoluteRect.left, -m_absoluteRect.top));
        {
            auto& setting = appearances[m_state.Index()];

            // Background
            Resu::SOLID_COLOR_BRUSH->SetColor(setting.background.color);
            Resu::SOLID_COLOR_BRUSH->SetOpacity(setting.background.opacity);

            if (brush != nullptr)
            {
                rndr->d2d1DeviceContext->FillRoundedRectangle(
                    { m_absoluteRect, roundRadiusX, roundRadiusY }, Resu::SOLID_COLOR_BRUSH.Get());
            }
            if (bitmap != nullptr)
            {
                rndr->d2d1DeviceContext->DrawBitmap(bitmap.Get(), AbsoluteRect(), bitmapOpacity);
            }

            // Icon
            if (m_state.activeFlag == State::ActiveFlag::Intermidiate) DrawIntermidiateIcon(rndr);
            else if (m_state.activeFlag == State::ActiveFlag::Checked) DrawCheckedIcon(rndr);

            // Outline
            Resu::SOLID_COLOR_BRUSH->SetColor(setting.stroke.color);
            Resu::SOLID_COLOR_BRUSH->SetOpacity(setting.stroke.opacity);

            auto innerRect = Mathu::Stretch(m_absoluteRect, { -setting.stroke.width * 0.5f, -setting.stroke.width * 0.5f });

            rndr->d2d1DeviceContext->DrawRoundedRectangle(
                { innerRect, roundRadiusX, roundRadiusY }, Resu::SOLID_COLOR_BRUSH.Get(), setting.stroke.width);
        }
        mask.EndMaskDraw(rndr->d2d1DeviceContext.Get());
    }

    void CheckBox::OnRendererDrawD2D1ObjectHelper(Renderer* rndr)
    {
        rndr->d2d1DeviceContext->DrawBitmap(mask.bitmap.Get(), m_absoluteRect, mask.opacity);
    }

    void CheckBox::OnSizeHelper(SizeEvent& e)
    {
        ClickablePanel::OnSizeHelper(e);

        mask.LoadMaskBitmap(Mathu::Rounding(e.size.width), Mathu::Rounding(e.size.height));
    }

    void CheckBox::OnChangeThemeHelper(WstrViewParam themeName)
    {
        ClickablePanel::OnChangeThemeHelper(themeName);

        if (themeName == L"Light")
        {
            appearances[(size_t)State::Flag::UncheckedIdle] =
            {
                appearances[(size_t)State::Flag::UncheckedIdle].bitmap, // bitmap
                1.0f, // bitmap opacity

                // foreground
                {
                    D2D1::ColorF{ 0x000000 }, // color
                    0.0f // opacity
                },
                // background
                {
                    D2D1::ColorF{ 0xf2f2f2 }, // color
                    1.0f // opacity
                },
                // stroke
                {
                    1.0f, // width
                    D2D1::ColorF{ 0x8f8f8f }, // color
                    1.0f // opacity
                }
            };
            appearances[(size_t)State::Flag::UncheckedHover] =
            {
                appearances[(size_t)State::Flag::UncheckedHover].bitmap, // bitmap
                1.0f, // bitmap opacity

                // foreground
                {
                    D2D1::ColorF{ 0x000000 }, // color
                    0.0f // opacity
                },
                // background
                {
                    D2D1::ColorF{ 0xebebeb }, // color
                    1.0f // opacity
                },
                // stroke
                {
                    1.0f, // width
                    D2D1::ColorF{ 0x8f8f8f }, // color
                    1.0f // opacity
                }
            };
            appearances[(size_t)State::Flag::UncheckedDown] =
            {
                appearances[(size_t)State::Flag::UncheckedDown].bitmap, // bitmap
                1.0f, // bitmap opacity

                // foreground
                {
                    D2D1::ColorF{ 0x000000 }, // color
                    0.0f // opacity
                },
                // background
                {
                    D2D1::ColorF{ 0xe3e3e3 }, // color
                    1.0f // opacity
                },
                // stroke
                {
                    1.0f, // width
                    D2D1::ColorF{ 0xbcbcbc }, // color
                    1.0f // opacity
                }
            };
            ComPtr<ID2D1Bitmap1> originalBitmap;
            {
                originalBitmap = appearances[(size_t)State::Flag::CheckedIdle].bitmap;

                appearances[(size_t)State::Flag::IntermidiateIdle] =
                appearances[(size_t)State::Flag::CheckedIdle] =
                {
                    appearances[(size_t)State::Flag::IntermidiateIdle].bitmap, // bitmap
                    1.0f, // bitmap opacity

                    // foreground
                    {
                        D2D1::ColorF{ 0xfafafa }, // color
                        1.0f // opacity
                    },
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
                    }
                };
                appearances[(size_t)State::Flag::CheckedIdle].bitmap = originalBitmap;
            }
            {
                originalBitmap = appearances[(size_t)State::Flag::CheckedHover].bitmap;

                appearances[(size_t)State::Flag::IntermidiateHover] =
                appearances[(size_t)State::Flag::CheckedHover] =
                {
                    appearances[(size_t)State::Flag::IntermidiateHover].bitmap, // bitmap
                    1.0f, // bitmap opacity

                    // foreground
                    {
                        D2D1::ColorF{ 0xfafafa }, // color
                        1.0f // opacity
                    },
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
                    }
                };
                appearances[(size_t)State::Flag::CheckedHover].bitmap = originalBitmap;
            }
            {
                originalBitmap = appearances[(size_t)State::Flag::CheckedDown].bitmap;

                appearances[(size_t)State::Flag::IntermidiateDown] =
                appearances[(size_t)State::Flag::CheckedDown] =
                {
                    appearances[(size_t)State::Flag::IntermidiateDown].bitmap, // bitmap
                    1.0f, // bitmap opacity

                    // foreground
                    {
                        D2D1::ColorF{ 0xfafafa }, // color
                        1.0f // opacity
                    },
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
                    }
                };
                appearances[(size_t)State::Flag::CheckedDown].bitmap = originalBitmap;
            }
        }
        else if (themeName == L"Dark")
        {
            appearances[(size_t)State::Flag::UncheckedIdle] =
            {
                appearances[(size_t)State::Flag::UncheckedIdle].bitmap, // bitmap
                1.0f, // bitmap opacity

                // foreground
                {
                    D2D1::ColorF{ 0x000000 }, // color
                    0.0f // opacity
                },
                // background
                {
                    D2D1::ColorF{ 0x1f1f1f }, // color
                    1.0f // opacity
                },
                // stroke
                {
                    1.0f, // width
                    D2D1::ColorF{ 0x7a7a7a }, // color
                    1.0f // opacity
                }
            };
            appearances[(size_t)State::Flag::UncheckedHover] =
            {
                appearances[(size_t)State::Flag::UncheckedHover].bitmap, // bitmap
                1.0f, // bitmap opacity

                // foreground
                {
                    D2D1::ColorF{ 0x000000 }, // color
                    0.0f // opacity
                },
                // background
                {
                    D2D1::ColorF{ 0x262626 }, // color
                    1.0f // opacity
                },
                // stroke
                {
                    1.0f, // width
                    D2D1::ColorF{ 0x7a7a7a }, // color
                    1.0f // opacity
                }
            };
            appearances[(size_t)State::Flag::UncheckedDown] =
            {
                appearances[(size_t)State::Flag::UncheckedDown].bitmap, // bitmap
                1.0f, // bitmap opacity

                // foreground
                {
                    D2D1::ColorF{ 0x000000 }, // color
                    0.0f // opacity
                },
                // background
                {
                    D2D1::ColorF{ 0x1a1a1a }, // color
                    1.0f // opacity
                },
                // stroke
                {
                    1.0f, // width
                    D2D1::ColorF{ 0x4c4c4c }, // color
                    1.0f // opacity
                }
            };
            ComPtr<ID2D1Bitmap1> originalBitmap;
            {
                originalBitmap = appearances[(size_t)State::Flag::CheckedIdle].bitmap;

                appearances[(size_t)State::Flag::IntermidiateIdle] =
                appearances[(size_t)State::Flag::CheckedIdle] =
                {
                    appearances[(size_t)State::Flag::IntermidiateIdle].bitmap, // bitmap
                    1.0f, // bitmap opacity

                    // foreground
                    {
                        D2D1::ColorF{ 0x0d0d0d }, // color
                        1.0f // opacity
                    },
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
                    }
                };
                appearances[(size_t)State::Flag::CheckedIdle].bitmap = originalBitmap;
            }
            {
                originalBitmap = appearances[(size_t)State::Flag::CheckedHover].bitmap;

                appearances[(size_t)State::Flag::IntermidiateHover] =
                appearances[(size_t)State::Flag::CheckedHover] =
                {
                    appearances[(size_t)State::Flag::IntermidiateHover].bitmap, // bitmap
                    1.0f, // bitmap opacity

                    // foreground
                    {
                        D2D1::ColorF{ 0x0d0d0d }, // color
                        1.0f // opacity
                    },
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
                    }
                };
                appearances[(size_t)State::Flag::CheckedHover].bitmap = originalBitmap;
            }
            {
                originalBitmap = appearances[(size_t)State::Flag::CheckedDown].bitmap;

                appearances[(size_t)State::Flag::IntermidiateDown] =
                appearances[(size_t)State::Flag::CheckedDown] =
                {
                    appearances[(size_t)State::Flag::IntermidiateDown].bitmap, // bitmap
                    1.0f, // bitmap opacity

                    // foreground
                    {
                        D2D1::ColorF{ 0x0d0d0d }, // color
                        1.0f // opacity
                    },
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
                    }
                };
                appearances[(size_t)State::Flag::CheckedDown].bitmap = originalBitmap;
            }
        }
    }

    bool CheckBox::OnMouseEnterHelper(MouseMoveEvent& e)
    {
        m_state.buttonFlag = State::ButtonFlag::Hover;

        return ClickablePanel::OnMouseEnterHelper(e);
    }

    bool CheckBox::OnMouseLeaveHelper(MouseMoveEvent& e)
    {
        m_state.buttonFlag = State::ButtonFlag::Idle;

        return ClickablePanel::OnMouseLeaveHelper(e);
    }

    void CheckBox::SetEnabled(bool value)
    {
        ClickablePanel::SetEnabled(value);

        mask.opacity = value ? 1.0f : 0.45f;

        m_state.buttonFlag = State::ButtonFlag::Idle;
    }

    void CheckBox::OnMouseButtonPressHelper(ClickablePanel::Event& e)
    {
        ClickablePanel::OnMouseButtonPressHelper(e);

        m_state.buttonFlag = State::ButtonFlag::Down;
    }

    void CheckBox::OnMouseButtonReleaseHelper(ClickablePanel::Event& e)
    {
        ClickablePanel::OnMouseButtonReleaseHelper(e);

        if (e.Left())
        {
            m_state.buttonFlag = State::ButtonFlag::Hover;

            // Change current active state.
            m_state.activeFlag = m_stateTransitionMap[m_state.activeFlag];

            // Trigger state changing event.
            StatefulObjectType::Event soe = {};
            soe.flag = m_state.activeFlag;

            OnStateChange(soe);
        }
    }
}