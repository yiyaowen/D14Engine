#include "Common/Precompile.h"

#include "UIKit/ScrollView.h"

#include "Renderer/MathUtils.h"
using namespace d14engine::renderer;

#include "UIKit/ResourceUtils.h"

namespace d14engine::uikit
{
    ScrollView::ScrollView(const D2D1_RECT_F& rect, ShrdPtrParam<Panel> content)
        :
        Panel(rect, Resu::SOLID_COLOR_BRUSH),
        ResizablePanel(rect, Resu::SOLID_COLOR_BRUSH),
        contentMask(Mathu::Rounding(Width()), Mathu::Rounding(Height())),
        m_content(content)
    {
        m_takeOverChildrenDrawing = true;

        SetResizable(false);

        background.opacity = stroke.opacity = 0.0f;
    }

    void ScrollView::OnInitializeFinish()
    {
        ResizablePanel::OnInitializeFinish();

        // Don't call this in the ctor since it uses shared_from_this internally.
        AddUIObject(m_content);
    }

    SharedPtr<Panel> ScrollView::Content()
    {
        return m_content;
    }

    void ScrollView::SetContent(ShrdPtrParam<Panel> content)
    {
        RemoveUIObject(m_content);
        AddUIObject(m_content = content);

        m_viewportOffset = { 0.0f, 0.0f };
    }

    const D2D1_POINT_2F& ScrollView::ViewportOffset()
    {
        return m_viewportOffset;
    }

    void ScrollView::SetViewportOffset(const D2D1_POINT_2F& absolute)
    {
        if (m_content != nullptr)
        {
            float selfWidth = Width(), selfHeight = Height();
            float contentWidth = m_content->Width(), contentHeight = m_content->Height();

            if (contentWidth > selfWidth)
            {
                m_viewportOffset.x = std::clamp(absolute.x, 0.0f, contentWidth - selfWidth);
            }
            else m_viewportOffset.x = 0.0f;

            if (contentHeight > selfHeight)
            {
                m_viewportOffset.y = std::clamp(absolute.y, 0.0f, contentHeight - selfHeight);
            }
            else m_viewportOffset.y = 0.0f;

            m_content->Move(-m_viewportOffset.x, -m_viewportOffset.y);
        }
        else m_viewportOffset = { 0.0f, 0.0f };

        // Trigger corresponding callback.
        OnViewportOffsetChange(m_viewportOffset);
    }

    D2D1_POINT_2F ScrollView::ViewportOffsetPercentage()
    {
        if (m_content != nullptr)
        {
            return
            {
                m_viewportOffset.x / (m_content->Width() - Width()),
                m_viewportOffset.y / (m_content->Height() - Height())
            };
        }
        else return { 0.0f, 0.0f };
    }

    void ScrollView::SetViewportOffsetPercentage(const D2D1_POINT_2F& relative)
    {
        if (m_content != nullptr)
        {
            D2D1_POINT_2F absoluteOffset =
            {
               relative.x * (m_content->Width() - Width()),
               relative.y * (m_content->Height() - Height())
            };
            SetViewportOffset(absoluteOffset);
        }
        else SetViewportOffset({ 0.0f, 0.0f });
    }

    bool ScrollView::IsControllingScrollBars()
    {
        return IsControllingHorzBar() || IsControllingVertBar();
    }

    bool ScrollView::IsControllingHorzBar()
    {
        return m_isHorzBarHover || m_isHorzBarDown;
    }

    bool ScrollView::IsControllingVertBar()
    {
        return m_isVertBarHover || m_isVertBarDown;
    }

    D2D1_RECT_F ScrollView::HorzBarRect(ScrollBarState state)
    {
        if (m_content != nullptr)
        {
            auto& setting = scrollBarAppearances[(size_t)state];

            float selfWidth = Width(), selfHeight = Height();
            float contentWidth = m_content->Width(), contentHeight = m_content->Height();

            if (contentWidth > selfWidth)
            {
                float horzStart = m_viewportOffset.x / contentWidth;
                float horzEnd = horzStart + selfWidth / contentWidth;

                return
                {
                    horzStart * selfWidth,
                    selfHeight - (setting.externalOffset + setting.internalSize),
                    horzEnd * selfWidth,
                    selfHeight - setting.externalOffset
                };
            }
        }
        return { 0.0f, 0.0f };
    }

    D2D1_RECT_F ScrollView::VertBarRect(ScrollBarState state)
    {
        if (m_content != nullptr)
        {
            auto& setting = scrollBarAppearances[(size_t)state];

            float selfWidth = Width(), selfHeight = Height();
            float contentWidth = m_content->Width(), contentHeight = m_content->Height();

            if (contentHeight > selfHeight)
            {
                float vertStart = m_viewportOffset.y / contentHeight;
                float vertEnd = vertStart + selfHeight / contentHeight;

                return
                {
                    selfWidth - (setting.externalOffset + setting.internalSize),
                    vertStart * selfHeight,
                    selfWidth - setting.externalOffset,
                    vertEnd * selfHeight
                };
            }
        }
        return { 0.0f, 0.0f };
    }

    ScrollView::ScrollBarState ScrollView::GetHorzBarState(bool isHover, bool isDown)
    {
        if (m_isHorzBarDown) return ScrollBarState::Down;
        else if (m_isHorzBarHover) return ScrollBarState::Hover;
        else return ScrollBarState::Idle;
    }

    ScrollView::ScrollBarState ScrollView::GetVertBarState(bool isHover, bool isDown)
    {
        if (m_isVertBarDown) return ScrollBarState::Down;
        else if (m_isVertBarHover) return ScrollBarState::Hover;
        else return ScrollBarState::Idle;
    }

    void ScrollView::OnViewportOffsetChange(const D2D1_POINT_2F& offset)
    {
        if (f_onViewportOffsetChangeOverride)
        {
            f_onViewportOffsetChangeOverride(this, offset);
        }
        else
        {
            if (f_onViewportOffsetChangeBefore) f_onViewportOffsetChangeBefore(this, offset);

            OnViewportOffsetChangeHelper(offset);

            if (f_onViewportOffsetChangeAfter) f_onViewportOffsetChangeAfter(this, offset);
        }
    }

    void ScrollView::OnViewportOffsetChangeHelper(const D2D1_POINT_2F& offset)
    {
        // TODO: add scroll-view viewport offset changed logic.
    }

    void ScrollView::OnRendererDrawD2D1LayerHelper(Renderer* rndr)
    {
        ResizablePanel::OnRendererDrawD2D1LayerHelper(rndr);

        if (m_content != nullptr && m_content->IsD2D1ObjectVisible())
        {
            m_content->OnRendererDrawD2D1Layer(rndr);
        }
        // Content on Mask. Note other children are hidden by default.
        contentMask.BeginMaskDraw(rndr->d2d1DeviceContext.Get(), D2D1::Matrix3x2F::Translation(-m_absoluteRect.left, -m_absoluteRect.top));
        {
            if (m_content != nullptr && m_content->IsD2D1ObjectVisible())
            {
                m_content->OnRendererDrawD2D1Object(rndr);
            }
        }
        contentMask.EndMaskDraw(rndr->d2d1DeviceContext.Get());
    }

    void ScrollView::OnRendererDrawD2D1ObjectHelper(Renderer* rndr)
    {
        // Background
        Resu::SOLID_COLOR_BRUSH->SetColor(background.color);
        Resu::SOLID_COLOR_BRUSH->SetOpacity(background.opacity);

        ResizablePanel::DrawBackground(rndr);

        // Content
        rndr->d2d1DeviceContext->DrawBitmap(contentMask.bitmap.Get(), m_absoluteRect);

        // Outline
        Resu::SOLID_COLOR_BRUSH->SetColor(stroke.color);
        Resu::SOLID_COLOR_BRUSH->SetOpacity(stroke.opacity);

        auto innerRect = Mathu::Stretch(m_absoluteRect, { -stroke.width * 0.5f, -stroke.width * 0.5f });

        rndr->d2d1DeviceContext->DrawRoundedRectangle(
            { innerRect, roundRadiusX, roundRadiusY }, Resu::SOLID_COLOR_BRUSH.Get(), stroke.width);

        // Scroll Bars
        if (m_content != nullptr)
        {
            auto horzState = GetHorzBarState(m_isHorzBarHover, m_isHorzBarDown);
            auto vertState = GetVertBarState(m_isVertBarHover, m_isVertBarDown);

            auto& horzSetting = scrollBarAppearances[(size_t)horzState];
            auto& vertSetting = scrollBarAppearances[(size_t)vertState];

            // Horizontal Bar
            Resu::SOLID_COLOR_BRUSH->SetColor(horzSetting.color);
            Resu::SOLID_COLOR_BRUSH->SetOpacity(horzSetting.opacity);

            rndr->d2d1DeviceContext->FillRoundedRectangle(
                {
                    SelfCoordToAbsolute(HorzBarRect(horzState)),
                    horzSetting.roundRadius,
                    horzSetting.roundRadius
                },
                Resu::SOLID_COLOR_BRUSH.Get());
            
            // Vertical Bar
            Resu::SOLID_COLOR_BRUSH->SetColor(vertSetting.color);
            Resu::SOLID_COLOR_BRUSH->SetOpacity(vertSetting.opacity);

            rndr->d2d1DeviceContext->FillRoundedRectangle(
                {
                    SelfCoordToAbsolute(VertBarRect(vertState)),
                    vertSetting.roundRadius,
                    vertSetting.roundRadius
                },
                Resu::SOLID_COLOR_BRUSH.Get());
        }
    }

    void ScrollView::OnSizeHelper(SizeEvent& e)
    {
        ResizablePanel::OnSizeHelper(e);

        // Because self size was changed just now, the viewport offset
        // needs to be updated to ensure it is within the valid range.
        SetViewportOffset(m_viewportOffset);

        contentMask.LoadMaskBitmap(Mathu::Rounding(e.size.width), Mathu::Rounding(e.size.height));
    }

    void ScrollView::OnChangeThemeHelper(WstrViewParam themeName)
    {
        ResizablePanel::OnChangeThemeHelper(themeName);

        if (themeName == L"Light")
        {
            background.color = D2D1::ColorF{ 0xf2f2f2 };
            background.opacity = 1.0f;

            stroke.width = 1.0f;

            stroke.color = D2D1::ColorF{ 0xe5e5e5 };
            stroke.opacity = 1.0f;

            scrollBarAppearances[(size_t)ScrollBarState::Idle] =
            {
                4.0f, // internal size
                2.0f, // round radius
                2.0f, // external offset
                D2D1::ColorF{ 0xc2c2c2 }, // color
                1.0f // opacity
            };
            scrollBarAppearances[(size_t)ScrollBarState::Hover] =
            {
                8.0f, // internal size
                4.0f, // round radius
                2.0f, // external offset
                D2D1::ColorF{ 0xc2c2c2 }, // color
                1.0f // opacity
            };
            scrollBarAppearances[(size_t)ScrollBarState::Down] =
            {
                8.0f, // internal size
                4.0f, // round radius
                2.0f, // external offset
                D2D1::ColorF{ 0x8f8f8f }, // color
                1.0f // opacity
            };
        }
        else if (themeName == L"Dark")
        {
            background.color = D2D1::ColorF{ 0x1f1f1f };
            background.opacity = 1.0f;

            stroke.width = 1.0f;

            stroke.color = D2D1::ColorF{ 0x1a1a1a };
            stroke.opacity = 1.0f;

            scrollBarAppearances[(size_t)ScrollBarState::Idle] =
            {
                4.0f, // internal size
                2.0f, // round radius
                2.0f, // external offset
                D2D1::ColorF{ 0x3d3d3d }, // color
                1.0f // opacity
            };
            scrollBarAppearances[(size_t)ScrollBarState::Hover] =
            {
                8.0f, // internal size
                4.0f, // round radius
                2.0f, // external offset
                D2D1::ColorF{ 0x3d3d3d }, // color
                1.0f // opacity
            };
            scrollBarAppearances[(size_t)ScrollBarState::Down] =
            {
                8.0f, // internal size
                4.0f, // round radius
                2.0f, // external offset
                D2D1::ColorF{ 0x5c5c5c }, // color
                1.0f // opacity
            };
        }
    }

    bool ScrollView::OnMouseButtonHelper(MouseButtonEvent& e)
    {
        auto p = AbsoluteToSelfCoord(e.cursorPoint);

        if (e.status.LeftDown())
        {
            m_isHorzBarDown = m_isHorzBarHover;
            m_isVertBarDown = m_isVertBarHover;

            m_horzBarHoldOffset = p.x;
            m_vertBarHoldOffset = p.y;

            m_originalViewportOffset = m_viewportOffset;
        }
        else if (e.status.LeftUp())
        {
            m_isHorzBarDown = m_isVertBarDown = false;
            m_horzBarHoldOffset = m_vertBarHoldOffset = 0.0f;
            m_originalViewportOffset = { 0.0f, 0.0f };
        }
        return ResizablePanel::OnMouseButtonHelper(e);
    }

    bool ScrollView::OnMouseMoveHelper(MouseMoveEvent& e)
    {
        auto p = AbsoluteToSelfCoord(e.cursorPoint);
        float selfWidth = Width(), selfHeight = Height();

        // Perform viewport motion.
        if (m_content != nullptr && selfWidth != 0.0f && selfHeight != 0.0f)
        {
            float contentWidth = m_content->Width(), contentHeight = m_content->Height();
            float horzRatio = contentWidth / selfWidth, vertRatio = contentHeight / selfHeight;

            if (m_isHorzBarDown)
            {
                m_skipDeliverNextMouseMoveEventToChildren = true;

                SetViewportOffset(Mathu::Offset(
                    m_originalViewportOffset, { (p.x - m_horzBarHoldOffset) * horzRatio, 0.0f}));
            }
            if (m_isVertBarDown)
            {
                m_skipDeliverNextMouseMoveEventToChildren = true;

                SetViewportOffset(Mathu::Offset(
                    m_originalViewportOffset, { 0.0f, (p.y - m_vertBarHoldOffset) * vertRatio }));
            }
        }
        // Update scroll bar state.
        {
            m_isVertBarHover = Mathu::IsInside(
                p, VertBarRect(m_isVertBarHover ? ScrollBarState::Hover : ScrollBarState::Idle));

            m_isHorzBarHover = Mathu::IsInside(
                p, HorzBarRect(m_isHorzBarHover ? ScrollBarState::Hover : ScrollBarState::Idle));
        }
        return ResizablePanel::OnMouseMoveHelper(e);
    }

    bool ScrollView::OnMouseLeaveHelper(MouseMoveEvent& e)
    {
        m_isHorzBarHover = m_isHorzBarDown = m_isVertBarHover = m_isVertBarDown = false;

        return ResizablePanel::OnMouseLeaveHelper(e);
    }

    bool ScrollView::OnMouseWheelHelper(MouseWheelEvent& e)
    {
        SetViewportOffset(Mathu::Offset(m_viewportOffset, e.keyState.CTRL ?
            D2D1_POINT_2F{ -e.deltaCount * deltaPixelsPerScroll, 0.0f } :
            D2D1_POINT_2F{ 0.0f, -e.deltaCount * deltaPixelsPerScroll }));

        return ResizablePanel::OnMouseWheelHelper(e);
    }
}