#include "Precompile.h"

#include "UI/ScrollView.h"

#include "Renderer/MathUtils.h"
using namespace d14engine::renderer;

#include "UI/UIResourceUtils.h"

namespace d14engine::ui
{
    ScrollView::ScrollView(const D2D1_RECT_F& rect, ShrdPtrParam<Panel> content)
        :
        ResizablePanel(rect, UIResu::SOLID_COLOR_BRUSH),
        MaskStyle((UINT)Width(), (UINT)Height()),
        SolidStyle({ 0.9f, 0.9f, 0.9f, 1.0f }),
        StrokeStyle(1.0f, { 0.8f, 0.8f, 0.8f, 1.0f }),
        m_content(content)
    {
        m_takeOverChildrenDrawing = true;

        SetResizable(false);
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
        RemoveUIObject(content);

        m_content = content;
        AddUIObject(content);

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

    void ScrollView::OnRendererDrawD2D1LayerHelper(Renderer* rndr)
    {
        ResizablePanel::OnRendererDrawD2D1LayerHelper(rndr);

        // Content on Mask. Note other children are hidden by default.
        BeginDrawOnMask(rndr->d2d1DeviceContext.Get(), D2D1::Matrix3x2F::Translation(-m_absoluteRect.left, -m_absoluteRect.top));
        {
            if (m_content != nullptr && m_content->IsD2D1ObjectVisible())
            {
                m_content->OnRendererDrawD2D1Object(rndr);
            }
        }
        EndDrawOnMask(rndr->d2d1DeviceContext.Get());
    }

    void ScrollView::OnRendererDrawD2D1ObjectHelper(Renderer* rndr)
    {
        // Background
        UIResu::SOLID_COLOR_BRUSH->SetColor(backgroundColor);
        UIResu::SOLID_COLOR_BRUSH->SetOpacity(backgroundOpacity);

        ResizablePanel::DrawBackground(rndr);

        // Content
        rndr->d2d1DeviceContext->DrawBitmap(maskBitmap.Get(), m_absoluteRect);

        // Outline
        UIResu::SOLID_COLOR_BRUSH->SetColor(strokeColor);
        UIResu::SOLID_COLOR_BRUSH->SetOpacity(strokeOpacity);

        auto innerRect = Mathu::Stretch(m_absoluteRect, { -strokeWidth * 0.5f, -strokeWidth * 0.5f });

        rndr->d2d1DeviceContext->DrawRoundedRectangle(
            { innerRect, roundRadiusX, roundRadiusY }, UIResu::SOLID_COLOR_BRUSH.Get(), strokeWidth);

        // Scroll Bars
        if (m_content != nullptr)
        {
            float selfWidth = Width(), selfHeight = Height();
            float contentWidth = m_content->Width(), contentHeight = m_content->Height();

            UIResu::SOLID_COLOR_BRUSH->SetOpacity(1.0f);

            auto horzState = ScrollBarState::Idle; // Idle
            if (m_isHorzBarHover) horzState = ScrollBarState::Hover;
            if (m_isHorzBarDown) horzState = ScrollBarState::Down;

            auto& horzSetting = scrollBarAppearances[(size_t)horzState];

            auto vertState = ScrollBarState::Idle; // Idle
            if (m_isVertBarHover) vertState = ScrollBarState::Hover;
            if (m_isVertBarDown) vertState = ScrollBarState::Down;

            auto& vertSetting = scrollBarAppearances[(size_t)vertState];

            // Horizontal Bar
            UIResu::SOLID_COLOR_BRUSH->SetColor(horzSetting.color);

            rndr->d2d1DeviceContext->FillRoundedRectangle(
                {
                    SelfCoordToAbsolute(HorzBarRect(horzState)),
                    horzSetting.roundRadius,
                    horzSetting.roundRadius
                },
                UIResu::SOLID_COLOR_BRUSH.Get());
            

            // Vertical Bar
            UIResu::SOLID_COLOR_BRUSH->SetColor(vertSetting.color);

            rndr->d2d1DeviceContext->FillRoundedRectangle(
                {
                    SelfCoordToAbsolute(VertBarRect(vertState)),
                    vertSetting.roundRadius,
                    vertSetting.roundRadius
                },
                UIResu::SOLID_COLOR_BRUSH.Get());
        }
    }

    void ScrollView::OnSizeHelper(SizeEvent& e)
    {
        ResizablePanel::OnSizeHelper(e);

        // Because self size was changed just now, the viewport offset
        // needs to be updated to ensure it is within the valid range.
        SetViewportOffset(m_viewportOffset);

        LoadMaskBitmap((UINT)(e.size.width + 0.5f), (UINT)(e.size.height + 0.5f));
    }

    void ScrollView::OnChangeThemeHelper(WstrViewParam themeName)
    {
        ResizablePanel::OnChangeThemeHelper(themeName);

        if (themeName == L"Light")
        {
            backgroundColor = { 0.9f, 0.9f, 0.9f, 1.0f };
            backgroundOpacity = 1.0f;

            strokeWidth = 1.0f;
            strokeColor = { 0.8f, 0.8f, 0.8f, 1.0f };
            strokeOpacity = 1.0f;

            scrollBarAppearances[(size_t)ScrollBarState::Idle] =
            {
                4.0f, // internal size
                2.0f, // round radius
                2.0f, // external offset
                { 0.8f, 0.8f, 0.8f, 1.0f }
            };
            scrollBarAppearances[(size_t)ScrollBarState::Hover] =
            {
                8.0f, // internal size
                4.0f, // round radius
                2.0f, // external offset
                { 0.8f, 0.8f, 0.8f, 1.0f }
            };
            scrollBarAppearances[(size_t)ScrollBarState::Down] =
            {
                8.0f, // internal size
                4.0f, // round radius
                2.0f, // external offset
                { 0.7f, 0.7f, 0.7f, 1.0f }
            };
        }
        else if (themeName == L"Dark")
        {
            backgroundColor = { 0.12f, 0.12f, 0.12f, 1.0f };
            backgroundOpacity = 1.0f;

            strokeWidth = 1.0f;
            strokeColor = { 0.1f, 0.1f, 0.1f, 1.0f };
            strokeOpacity = 1.0f;

            scrollBarAppearances[(size_t)ScrollBarState::Idle] =
            {
                4.0f, // internal size
                2.0f, // round radius
                2.0f, // external offset
                { 0.2f, 0.2f, 0.2f, 1.0f }
            };
            scrollBarAppearances[(size_t)ScrollBarState::Hover] =
            {
                8.0f, // internal size
                4.0f, // round radius
                2.0f, // external offset
                { 0.2f, 0.2f, 0.2f, 1.0f }
            };
            scrollBarAppearances[(size_t)ScrollBarState::Down] =
            {
                8.0f, // internal size
                4.0f, // round radius
                2.0f, // external offset
                { 0.3f, 0.3f, 0.3f, 1.0f }
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
        if (m_content != nullptr && selfWidth != 0 && selfHeight != 0)
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
            m_isVertBarHover = Mathu::IsOverlapped(
                p, VertBarRect(m_isVertBarHover ? ScrollBarState::Hover : ScrollBarState::Idle));

            m_isHorzBarHover = Mathu::IsOverlapped(
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