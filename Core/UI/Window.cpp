#include "Precompile.h"

#include "UI/Window.h"

#include "Renderer/MathUtils.h"
#include "UI/Application.h"

namespace d14engine::ui
{
    ComPtr<ID2D1LinearGradientBrush>
        Window::g_titleBarPanelBrush,
        Window::g_decorativeBarBrush;

    void Window::LoadCommonResources()
    {
        // Title panel, linear gradient, bottom -> top, 80% B -> 95% B.
        {
            ComPtr<ID2D1GradientStopCollection> coll;
            D2D1_GRADIENT_STOP stop[] =
            {
                { 0.0f, { 0.8f, 0.8f, 0.8f, 1.0f } },
                { 1.0f, { 0.95f, 0.95f, 0.95f, 1.0f } }
            };
            THROW_IF_FAILED(Application::RENDERER->d2d1DeviceContext->
                CreateGradientStopCollection(stop, _countof(stop), &coll));

            THROW_IF_FAILED(Application::RENDERER->d2d1DeviceContext->
                CreateLinearGradientBrush({}, coll.Get(), &g_titleBarPanelBrush));
        }
        // Decorative bar, linear gradient, left -> right, Crimson -> Firebrick.
        {
            ComPtr<ID2D1GradientStopCollection> coll;
            D2D1_GRADIENT_STOP stop[] =
            {
                { 0.0f, (D2D1::ColorF)D2D1::ColorF::Crimson },
                { 1.0f, (D2D1::ColorF)D2D1::ColorF::Firebrick }
            };
            THROW_IF_FAILED(Application::RENDERER->d2d1DeviceContext->
                CreateGradientStopCollection(stop, _countof(stop), &coll));

            THROW_IF_FAILED(Application::RENDERER->d2d1DeviceContext->
                CreateLinearGradientBrush({}, coll.Get(), &g_decorativeBarBrush));
        }
    }

    Window::Window(WstrParam text, const D2D1_RECT_F& rect)
        :
        Panel(rect, UIResu::SOLID_COLOR_BRUSH),
        MaskStyle((UINT)Width(), (UINT)Height()),
        ShadowStyle((UINT)Width(), (UINT)Height()),
        SolidStyle({ 0.95f, 0.95f, 0.95f, 1.0f })
    {
        // We want the window can respond to sizing event when the cursor moves on shadow frame.
        shadowExtensionOffset = 6.0f;

        // Create title bar text label.
        {
            m_title = MakeUIObject<Label>(text, RelativeTitlePanelRect());

            // Note p is m_title. Also see Panel's OnParentSize for details.
            m_title->f_onParentSizeBefore = [this](Panel* p, SizeEvent& e)
            {
                auto l = (Label*)p;
                // Hide if there's no enough space.
                // Use 248.0f since the minimize button is 124.0f from title bar right,
                // and the title text is going to be placed in the horizontal center.
                if (e.size.width - 248.0f > l->TextLayoutMetrics().width)
                {
                    l->Resize(e.size.width, 32.0f);

                    // Change the extra visibility flag instead of use SetVisible(true)
                    // to avoid the title showing incorrectly when the window is small.
                    m_isTitleVisible = true;
                }
                else m_isTitleVisible = false;
            };
        }
        // Create close button text icon.
        {
            m_closeX = MakeUIObject<Label>(L"x", RelativeCloseButtonRect());

            // Note p is m_closeX. Also see Panel's OnParentSize for details.
            m_closeX->f_onParentSizeBefore = [this](Panel* p, SizeEvent& e)
            {
                p->Move(e.size.width - 60.0f, 0.0f);
            };

            // FIXME: why not CENTER? The 'x' needs to be placed in the center of the the button;
            // however, it only looks correct when we set the alignment to FAR instead of CENTER.
            m_closeX->alignment.vertical = DWRITE_PARAGRAPH_ALIGNMENT_FAR;
        }
    }

    void Window::OnInitializeFinish()
    {
        m_title->SetParent(shared_from_this());
        m_closeX->SetParent(shared_from_this());
    }

    void Window::OnMinimize()
    {
        if (f_onMinimizeOverride)
        {
            f_onMinimizeOverride(this);
        }
        else
        {
            if (f_onMinimizeBefore) f_onMinimizeBefore(this);

            OnMinimizeHelper();

            if (f_onMinimizeAfter) f_onMinimizeAfter(this);
        }
    }

    void Window::OnMinimizeHelper()
    {
        // TODO: add window minimize logic.
    }

    void Window::OnMaximize()
    {
        if (f_onMaximizeOverride)
        {
            f_onMaximizeOverride(this);
        }
        else
        {
            if (f_onMaximizeBefore) f_onMaximizeBefore(this);

            OnMaximizeHelper();

            if (f_onMaximizeAfter) f_onMaximizeAfter(this);
        }
    }

    void Window::OnMaximizeHelper()
    {
        // TODO: add window maximize logic.
    }

    void Window::OnClose()
    {
        if (f_onCloseOverride)
        {
            f_onCloseOverride(this);
        }
        else
        {
            if (f_onCloseBefore) f_onCloseBefore(this);

            OnCloseHelper();

            if (f_onCloseAfter) f_onCloseAfter(this);
        }
    }

    void Window::OnCloseHelper()
    {
        UnregisterDrawObjects();
        UnregisterApplicationEvents();

        for (auto& child : m_children)
        {
            child->UnregisterDrawObjects();
            child->UnregisterApplicationEvents();
        }
    }

    D2D1_RECT_F Window::TitlePanelRect()
    {
        return
        {
            m_absoluteRect.left,
            m_absoluteRect.top,
            m_absoluteRect.right,
            m_absoluteRect.top + 32.0f
        };
    }

    D2D1_RECT_F Window::RelativeTitlePanelRect()
    {
        return { 0.0f, 0.0f, Width(), 32.0f };
    }

    D2D1_RECT_F Window::DecorativeBarRect()
    {
        return
        {
            m_absoluteRect.left,
            m_absoluteRect.top + 32.0f,
            m_absoluteRect.right,
            m_absoluteRect.top + 40.0f
        };
    }

    D2D1_RECT_F Window::CloseButtonRect()
    {
        return
        {
            m_absoluteRect.right - 60.0f,
            m_absoluteRect.top,
            m_absoluteRect.right - 20.0f,
            m_absoluteRect.top + 24.0f
        };
    }

    D2D1_RECT_F Window::RelativeCloseButtonRect()
    {
        float width = Width();
        return { width - 60.0f, 0.0f, width - 20.0f, 24.0f };
    }

    D2D1_RECT_F Window::MaximizeButtonRect()
    {
        return
        {
            m_absoluteRect.right - 92.0f,
            m_absoluteRect.top,
            m_absoluteRect.right - 60.0f,
            m_absoluteRect.top + 24.0f
        };
    }

    D2D1_RECT_F Window::MaximizeIconRect()
    {
        auto mbr = MaximizeButtonRect();
        // Since the whole stroke should be drawn inside the boundary,
        // we simply shrink the rect with 1px to simulate this effects.
        return { mbr.left + 11.0f, mbr.top + 7.0f, mbr.left + 21.0f, mbr.top + 17.0f };
    }

    D2D1_RECT_F Window::MinimizeButtonRect()
    {
        return
        {
            m_absoluteRect.right - 124.0f,
            m_absoluteRect.top,
            m_absoluteRect.right - 92.0f,
            m_absoluteRect.top + 24.0f
        };
    }

    D2D1_RECT_F Window::MinimizeIconRect()
    {
        auto mbr = MinimizeButtonRect();
        return { mbr.left + 10.0f, mbr.top + 14.0f, mbr.left + 22.0f, mbr.top + 16.0f };
    }

    void Window::Set3BrothersButtonBrushState(bool isHover, bool isDown)
    {
        float opaque = 0.0f; // Idle
        if (isHover) opaque = 0.8f;
        if (isDown) opaque = 0.6f;

        UIResu::SOLID_COLOR_BRUSH->SetColor((D2D1::ColorF)D2D1::ColorF::Black);
        UIResu::SOLID_COLOR_BRUSH->SetOpacity(opaque);
    }

    void Window::Set3BrothersIconBrushState(bool isHover, bool isDown)
    {
        auto color = (D2D1::ColorF)D2D1::ColorF::Black; // Idle
        if (isHover) color = (D2D1::ColorF)D2D1::ColorF::White;
        if (isDown) color = (D2D1::ColorF)D2D1::ColorF::White;

        float opaque = 0.8f; // Idle
        if (isHover) opaque = 0.8f;
        if (isDown) opaque = 0.6f;

        UIResu::SOLID_COLOR_BRUSH->SetColor(color);
        UIResu::SOLID_COLOR_BRUSH->SetOpacity(opaque);
    }

    void Window::OnRendererDrawD2D1Layer(Renderer* RENDERER)
    {
        Panel::DrawChildrenLayers(RENDERER);

        // Shape of Shadow
        BeginDrawOnShadow(RENDERER->d2d1DeviceContext.Get());
        {
            // The color of shadow canvas can be designated casually,
            // where alpha channel will decide the shape of shadow.
            RENDERER->d2d1DeviceContext->Clear((D2D1::ColorF)D2D1::ColorF::Black);
        }
        EndDrawOnShadow(RENDERER->d2d1DeviceContext.Get());

        // Children on Mask
        BeginDrawOnMask(RENDERER->d2d1DeviceContext.Get(), D2D1::Matrix3x2F::Translation(-m_absoluteRect.left, -m_absoluteRect.top));
        {
            // In case the title displays incorrectly when the window is small.
            // For example, SetVisible(true) is called by the user unexpectedly.
            bool tmpFlag = m_title->IsVisible();
            m_title->SetVisible(tmpFlag && m_isTitleVisible);

            // Adjust the appearance of the close button icon dynamically.
            if (m_isCloseDown)
            {
                m_closeX->textColor = (D2D1::ColorF)D2D1::ColorF::White;
                m_closeX->textColorOpaque = 0.8f;
            }
            else if (m_isCloseHover)
            {
                m_closeX->textColor = (D2D1::ColorF)D2D1::ColorF::White;
                m_closeX->textColorOpaque = 1.0f;
            }
            else // Close button is idle.
            {
                m_closeX->textColor = (D2D1::ColorF)D2D1::ColorF::Black;
                m_closeX->textColorOpaque = 1.0f;
            }

            Panel::DrawChildrenObjects(RENDERER);

            m_title->SetVisible(tmpFlag);
        }
        EndDrawOnMask(RENDERER->d2d1DeviceContext.Get());
    }

    void Window::OnRendererDrawD2D1Object(Renderer* RENDERER)
    {
        // Shadow
        {
            UIResu::SHADOW_EFFECT->SetInput(0, m_shadowBitmap.Get());

            UIResu::SHADOW_EFFECT->SetValue(D2D1_SHADOW_PROP_BLUR_STANDARD_DEVIATION, shadowStandardDeviation);
            UIResu::SHADOW_EFFECT->SetValue(D2D1_SHADOW_PROP_COLOR, shadowColor);
            UIResu::SHADOW_EFFECT->SetValue(D2D1_SHADOW_PROP_OPTIMIZATION, shadowOptimization);

            RENDERER->d2d1DeviceContext->DrawImage(UIResu::SHADOW_EFFECT.Get(), AbsolutePosition());
        }
        // Background
        {
            UIResu::SOLID_COLOR_BRUSH->SetColor(solidColor);
            UIResu::SOLID_COLOR_BRUSH->SetOpacity(solidColorOpaque);

            Panel::DrawBackground(RENDERER);
        }
        // Title Bar
        {
            // Panel
            auto tpr = TitlePanelRect();

            g_titleBarPanelBrush->SetStartPoint({ tpr.left, tpr.bottom });
            g_titleBarPanelBrush->SetEndPoint({ tpr.left, tpr.top });

            RENDERER->d2d1DeviceContext->FillRectangle(tpr, g_titleBarPanelBrush.Get());

            // Title text had been drawn as child.

            // Decorative Bar
            auto dbr = DecorativeBarRect();

            g_decorativeBarBrush->SetStartPoint({ dbr.left, dbr.top });
            g_decorativeBarBrush->SetEndPoint({ dbr.right, dbr.top });

            RENDERER->d2d1DeviceContext->FillRectangle(dbr, g_decorativeBarBrush.Get());
        }
        // 3 Brothers
        {
            // Close Button
            {
                float opaque = 0.0f; // Idle
                if (m_isCloseHover) opaque = 1.0f;
                if (m_isCloseDown) opaque = 0.8f;

                UIResu::SOLID_COLOR_BRUSH->SetColor({ 0.78f, 0.12f, 0.2f, 1.0f });
                UIResu::SOLID_COLOR_BRUSH->SetOpacity(opaque);

                RENDERER->d2d1DeviceContext->FillRectangle(CloseButtonRect(), UIResu::SOLID_COLOR_BRUSH.Get());

                // Close icon had been drawn as child.
            }
            // Maximize Button
            {
                auto mbr = MaximizeButtonRect();

                Set3BrothersButtonBrushState(m_isMaximizeHover, m_isMaximizeDown);
                RENDERER->d2d1DeviceContext->FillRectangle(mbr, UIResu::SOLID_COLOR_BRUSH.Get());

                Set3BrothersIconBrushState(m_isMaximizeHover, m_isMaximizeDown);
                RENDERER->d2d1DeviceContext->DrawRectangle(MaximizeIconRect(), UIResu::SOLID_COLOR_BRUSH.Get(), 2.0f);
            }
            // Minimize Button
            {
                Set3BrothersButtonBrushState(m_isMinimizeHover, m_isMinimizeDown);
                RENDERER->d2d1DeviceContext->FillRectangle(MinimizeButtonRect(), UIResu::SOLID_COLOR_BRUSH.Get());

                Set3BrothersIconBrushState(m_isMinimizeHover, m_isMinimizeDown);
                RENDERER->d2d1DeviceContext->FillRectangle(MinimizeIconRect(), UIResu::SOLID_COLOR_BRUSH.Get());
            }
        }
        PostMaskToScene(RENDERER->d2d1DeviceContext.Get(), m_absoluteRect);
    }

    bool Window::IsHit(Event::Point& p)
    {
        return Mathu::IsOverlapped(p, ShadedRect(m_absoluteRect));
    }

    float Window::MinimalWidth()
    {
        return 144.0f;
    }

    float Window::MinimalHeight()
    {
        return 40.0f;
    }

    void Window::OnSizeHelper(SizeEvent& e)
    {
        auto width = (UINT)Width();
        auto height = (UINT)Height();

        LoadMaskBitmap(width, height);
        LoadShadowBitmap(width, height);

        Panel::OnSizeHelper(e);
    }

    bool Window::OnMouseButtonHelper(MouseButtonEvent& e)
    {
        auto p = e.cursorPoint;

        if (e.status.LeftDown())
        {
            if (m_is3BrothersEnabled)
            {
                // Update button states of 3 brothers.
                m_isCloseDown = Mathu::IsInside(p, CloseButtonRect());
                m_isMaximizeDown = Mathu::IsInside(p, MaximizeButtonRect());
                m_isMinimizeDown = Mathu::IsInside(p, MinimizeButtonRect());
            }
            else m_isCloseDown = m_isMaximizeDown = m_isMinimizeDown = false;

            // Check whether is dragging title bar.
            if (!m_isCloseHover && !m_isMaximizeHover && !m_isMinimizeHover)
            {
                if (m_isDragTitleBar = Mathu::IsInside(p, TitlePanelRect()))
                {
                    m_is3BrothersEnabled = false;

                    m_dragPoint = AbsoluteToSelfCoord(p);
                    // In case the mouse dashes out of the boundary.
                    Application::APP->PinUIObject(shared_from_this());

                    Application::APP->MainCursor()->SetIcon(Cursor::IconIndex::Move);
                }
            }

            m_isLeftSizing = m_isLeftHover;
            m_isTopSizing = m_isTopHover;
            m_isRightSizing = m_isRightHover;
            m_isBottomSizing = m_isBottomHover;

            // Check whether is holding window frame.
            if (m_isLeftSizing || m_isTopSizing || m_isRightSizing || m_isBottomSizing)
            {
                m_is3BrothersEnabled = false;

                m_sizingOffset = AbsoluteToSelfCoord(p);

                if (m_sizingOffset.x > 0) m_sizingOffset.x -= Width();
                if (m_sizingOffset.y > 0) m_sizingOffset.y -= Height();

                // In case the mouse dashes out of the boundary.
                Application::APP->PinUIObject(shared_from_this());
            }
        }
        else if (e.status.LeftUp())
        {
            // Trigger button events of 3 brothers.
            if (m_isCloseDown)
            {
                OnClose();
            }
            else if (m_isMaximizeDown)
            {
                OnMaximize();
            }
            else if (m_isMinimizeDown)
            {
                OnMinimize();
            }
            m_isCloseDown = m_isMaximizeDown = m_isMinimizeDown = false;

            // Always cancel dragging if left button up.
            if (m_isDragTitleBar)
            {
                m_is3BrothersEnabled = true;
                m_isDragTitleBar = false;
                Application::APP->UnpinUIObject(shared_from_this());
            }

            // Always cancel sizing if left button up.
            if (m_isLeftSizing || m_isTopSizing || m_isRightSizing || m_isBottomSizing)
            {
                m_is3BrothersEnabled = true;
                m_isLeftSizing = m_isTopSizing = m_isRightSizing = m_isBottomSizing = false;
                Application::APP->UnpinUIObject(shared_from_this());
            }
        }
        return Panel::OnMouseButtonHelper(e);
    }

    bool Window::OnMouseMoveHelper(MouseMoveEvent& e)
    {
        auto p = e.cursorPoint;
        
        if (m_is3BrothersEnabled)
        {
            // Update button states of 3 brothers.
            if (!Mathu::IsInside(p, CloseButtonRect()))
            {
                m_isCloseHover = false;
                m_isCloseDown = false;
            }
            else m_isCloseHover = true;

            if (!Mathu::IsInside(p, MaximizeButtonRect()))
            {
                m_isMaximizeHover = false;
                m_isMaximizeDown = false;
            }
            else m_isMaximizeHover = true;

            if (!Mathu::IsInside(p, MinimizeButtonRect()))
            {
                m_isMinimizeHover = false;
                m_isMinimizeDown = false;
            }
            else m_isMinimizeHover = true;
        }
        else m_isCloseHover = m_isMaximizeHover = m_isMinimizeHover = false;

        // Trigger movement if drag title bar.
        if (m_isDragTitleBar)
        {
            auto relative = AbsoluteToRelative(p);
            Move(relative.x - m_dragPoint.x, relative.y - m_dragPoint.y);

            Application::APP->MainCursor()->SetIcon(Cursor::IconIndex::Move);
        }
        else // Trigger resizing if hold window frame.
        {
            auto relative = AbsoluteToRelative(p);

            // The sizing point is not aligned to window boundary strictly since the sizing event
            // will be triggered when mouse cursor moves on shadow frame. This causes the point
            // actually offsets the boundary by a few pixels, which is stored in m_sizingOffset.
            relative.x -= m_sizingOffset.x;
            relative.y -= m_sizingOffset.y;

            float minWidth = MinimalWidth();
            float minHeight = MinimalHeight();

            // Left

            if (m_isLeftSizing && !m_isTopSizing && !m_isRightSizing && !m_isBottomSizing)
            {
                float afterWidth = m_rect.right - relative.x;

                if (afterWidth < minWidth)
                {
                    afterWidth = minWidth;
                    relative.x = m_rect.right - minWidth;
                }
                Transform(relative.x, m_rect.top, afterWidth, Height());

                Application::APP->MainCursor()->SetIcon(Cursor::IconIndex::HorzSize);
            }

            // Right

            else if (m_isRightSizing && !m_isLeftSizing && !m_isTopSizing && !m_isBottomSizing)
            {
                float afterWidth = relative.x - m_rect.left;

                if (afterWidth < minWidth)
                {
                    afterWidth = minWidth;
                }
                Transform(m_rect.left, m_rect.top, afterWidth, Height());

                Application::APP->MainCursor()->SetIcon(Cursor::IconIndex::HorzSize);
            }

            // Top

            else if (m_isTopSizing && !m_isLeftSizing && !m_isRightSizing && !m_isBottomSizing)
            {
                float afterHeight = m_rect.bottom - relative.y;

                if (afterHeight < minHeight)
                {
                    afterHeight = minHeight;
                    relative.y = m_rect.bottom - minHeight;
                }
                Transform(m_rect.left, relative.y, Width(), afterHeight);

                Application::APP->MainCursor()->SetIcon(Cursor::IconIndex::VertSize);
            }

            // Bottom

            else if (m_isBottomSizing && !m_isLeftSizing && !m_isTopSizing && !m_isRightSizing)
            {
                float afterHeight = relative.y - m_rect.top;

                if (afterHeight < minHeight)
                {
                    afterHeight = minHeight;
                }
                Transform(m_rect.left, m_rect.top, Width(), afterHeight);

                Application::APP->MainCursor()->SetIcon(Cursor::IconIndex::VertSize);
            }

            // Left Top

            else if (m_isLeftSizing && m_isTopSizing && !m_isRightSizing && !m_isBottomSizing)
            {
                float afterWidth = m_rect.right - relative.x;
                float afterHeight = m_rect.bottom - relative.y;

                if (afterWidth < minWidth)
                {
                    afterWidth = minWidth;
                    relative.x = m_rect.right - minWidth;
                }
                if (afterHeight < minHeight)
                {
                    afterHeight = minHeight;
                    relative.y = m_rect.bottom - minHeight;
                }
                Transform(relative.x, relative.y, afterWidth, afterHeight);

                Application::APP->MainCursor()->SetIcon(Cursor::IconIndex::MainDiagSize);
            }

            // Left Bottom

            else if (m_isLeftSizing && m_isBottomSizing && !m_isRightSizing && !m_isTopSizing)
            {
                float afterWidth = m_rect.right - relative.x;
                float afterHeight = relative.y - m_rect.top;

                if (afterWidth < minWidth)
                {
                    afterWidth = minWidth;
                    relative.x = m_rect.right - minWidth;
                }
                if (afterHeight < minHeight)
                {
                    afterHeight = minHeight;
                }
                Transform(relative.x, m_rect.top, afterWidth, afterHeight);

                Application::APP->MainCursor()->SetIcon(Cursor::IconIndex::BackDiagSize);
            }

            // Right Top

            else if (m_isRightSizing && m_isTopSizing && !m_isLeftSizing && !m_isBottomSizing)
            {
                float afterWidth = relative.x - m_rect.left;
                float afterHeight = m_rect.bottom - relative.y;

                if (afterWidth < minWidth)
                {
                    afterWidth = minWidth;
                }
                if (afterHeight < minHeight)
                {
                    afterHeight = minHeight;
                    relative.y = m_rect.bottom - minHeight;
                }
                Transform(m_rect.left, relative.y, afterWidth, afterHeight);

                Application::APP->MainCursor()->SetIcon(Cursor::IconIndex::BackDiagSize);
            }

            // Right Bottom

            else if (m_isRightSizing && m_isBottomSizing && !m_isLeftSizing && !m_isTopSizing)
            {
                float afterWidth = relative.x - m_rect.left;
                float afterHeight = relative.y - m_rect.top;

                if (afterWidth < minWidth)
                {
                    afterWidth = minWidth;
                }
                if (afterHeight < minHeight)
                {
                    afterHeight = minHeight;
                }
                Transform(m_rect.left, m_rect.top, afterWidth, afterHeight);

                Application::APP->MainCursor()->SetIcon(Cursor::IconIndex::MainDiagSize);
            }

            // No Sizing

            else // Update sizing boundary states & icons.
            {
                m_isLeftHover = m_isTopHover = m_isRightHover = m_isBottomHover = false;

                if ((p.x <= m_absoluteRect.left && p.y <= m_absoluteRect.top) ||
                    (p.x >= m_absoluteRect.right && p.y >= m_absoluteRect.bottom))
                {
                    if (p.x <= m_absoluteRect.left)
                    {
                        m_isLeftHover = m_isTopHover = true;
                    }
                    else m_isRightHover = m_isBottomHover = true;

                    Application::APP->MainCursor()->SetIcon(Cursor::IconIndex::MainDiagSize);
                }
                else if ((p.x <= m_absoluteRect.left && p.y >= m_absoluteRect.bottom) ||
                         (p.x >= m_absoluteRect.right && p.y <= m_absoluteRect.top))
                {
                    if (p.x <= m_absoluteRect.left)
                    {
                        m_isLeftHover = m_isBottomHover = true;
                    }
                    else m_isRightHover = m_isTopHover = true;

                    Application::APP->MainCursor()->SetIcon(Cursor::IconIndex::BackDiagSize);
                }
                else if (p.x <= m_absoluteRect.left || p.x >= m_absoluteRect.right)
                {
                    if (p.x <= m_absoluteRect.left)
                    {
                        m_isLeftHover = true;
                    }
                    else m_isRightHover = true;

                    Application::APP->MainCursor()->SetIcon(Cursor::IconIndex::HorzSize);
                }
                else if (p.y <= m_absoluteRect.top || p.y >= m_absoluteRect.bottom)
                {
                    if (p.y <= m_absoluteRect.top)
                    {
                        m_isTopHover = true;
                    }
                    else m_isBottomHover = true;

                    Application::APP->MainCursor()->SetIcon(Cursor::IconIndex::VertSize);
                }
                else Application::APP->MainCursor()->SetIcon(Cursor::IconIndex::Arrow);
            }
        }
        if (m_isDragTitleBar || m_isLeftSizing || m_isTopSizing || m_isRightSizing || m_isBottomSizing)
        {
            return false; // Prevent children from responding to UI event during special operations.
        }
        else return Panel::OnMouseMoveHelper(e);
    }
}