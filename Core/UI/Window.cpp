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
        // Title panel, linear gradient, bottom -> top, 60% B -> 90% B.
        {
            ComPtr<ID2D1GradientStopCollection> coll;
            D2D1_GRADIENT_STOP stop[] =
            {
                { 0.0f, { 0.6f, 0.6f, 0.6f, 1.0f } },
                { 1.0f, { 0.9f, 0.9f, 0.9f, 1.0f } }
            };
            THROW_IF_FAILED(Application::RNDR->d2d1DeviceContext->
                CreateGradientStopCollection(stop, _countof(stop), &coll));

            THROW_IF_FAILED(Application::RNDR->d2d1DeviceContext->
                CreateLinearGradientBrush({}, coll.Get(), &g_titleBarPanelBrush));
        }
        // Decorative bar, linear gradient, left -> right, Firebrick -> DarkRed.
        {
            ComPtr<ID2D1GradientStopCollection> coll;
            D2D1_GRADIENT_STOP stop[] =
            {
                { 0.0f, (D2D1::ColorF)D2D1::ColorF::Firebrick },
                { 1.0f, (D2D1::ColorF)D2D1::ColorF::DarkRed }
            };
            THROW_IF_FAILED(Application::RNDR->d2d1DeviceContext->
                CreateGradientStopCollection(stop, _countof(stop), &coll));

            THROW_IF_FAILED(Application::RNDR->d2d1DeviceContext->
                CreateLinearGradientBrush({}, coll.Get(), &g_decorativeBarBrush));
        }
    }

    Window::Window(WstrParam text, const D2D1_RECT_F& rect)
        :
        Panel(rect, UIResu::SOLID_COLOR_BRUSH),
        MaskStyle((UINT)Width(), (UINT)Height()),
        ShadowStyle((UINT)Width(), (UINT)Height()),
        SolidColorStyle((D2D1::ColorF)D2D1::ColorF::Gainsboro)
    {
        // We want the window can respond to sizing event when the cursor moves on shadow frame.
        shadowExtensionOffset = 6.0f;

        // Create title bar text label.
        {
            m_title = std::make_shared<Label>(text, RelativeTitlePanelRect());

            // Note p is m_title. Also see Panel's OnParentSize for details.
            m_title->f_onParentSizeBefore = [this](Panel* p, SizeEvent& e)
            {
                auto l = (Label*)p;
                // Hide if there's no enough space.
                // Use 248.0f since the minimize button is 124.0f from title bar right,
                // and the title text is going to be placed in the horizontal center.
                if (e.size.width - 248.0f > l->GetTextLayoutMetrics().width)
                {
                    l->SetD2D1ObjectVisible(true);
                    l->Resize(e.size.width, 32.0f);
                }
                else l->SetD2D1ObjectVisible(false);
            };
        }
        // Create close button text icon.
        {
            m_closeX = std::make_shared<Label>(
                // Add a little transparency to the text to make it looks like an icon.
                L"x", RelativeCloseButtonRect(), (D2D1::ColorF)D2D1::ColorF::Black, 0.8f);

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
        float opaque = 0.1f; // Idle
        if (isHover) opaque = 0.6f;
        if (isDown) opaque = 1.0f;

        UIResu::SOLID_COLOR_BRUSH->SetColor((D2D1::ColorF)D2D1::ColorF::Gray);
        UIResu::SOLID_COLOR_BRUSH->SetOpacity(opaque);
    }

    void Window::Set3BrothersIconBrushState()
    {
        UIResu::SOLID_COLOR_BRUSH->SetColor((D2D1::ColorF)D2D1::ColorF::Black);
        UIResu::SOLID_COLOR_BRUSH->SetOpacity(0.6f);
    }

    void Window::OnRendererDrawD2D1Layer(Renderer* rndr)
    {
        Panel::DrawChildrenLayers(rndr);

        BeginDrawOnMask(rndr->d2d1DeviceContext.Get(), D2D1::Matrix3x2F::Translation(-m_absoluteRect.left, -m_absoluteRect.top));
        {
            Panel::DrawChildrenObjects(rndr);
        }
        EndDrawOnMask(rndr->d2d1DeviceContext.Get());
    }

    void Window::OnRendererDrawD2D1Object(Renderer* rndr)
    {
        // Shadow
        {
            UIResu::SHADOW_EFFECT->SetInput(0, m_shadowBitmap.Get());

            UIResu::SHADOW_EFFECT->SetValue(D2D1_SHADOW_PROP_BLUR_STANDARD_DEVIATION, shadowStandardDeviation);
            UIResu::SHADOW_EFFECT->SetValue(D2D1_SHADOW_PROP_COLOR, shadowColor);
            UIResu::SHADOW_EFFECT->SetValue(D2D1_SHADOW_PROP_OPTIMIZATION, shadowOptimization);

            rndr->d2d1DeviceContext->DrawImage(UIResu::SHADOW_EFFECT.Get(), AbsolutePosition());
        }
        // Background
        {
            UIResu::SOLID_COLOR_BRUSH->SetColor(solidColor);
            UIResu::SOLID_COLOR_BRUSH->SetOpacity(solidColorOpaque);

            Panel::DrawBackground(rndr);
        }
        // Title Bar
        {
            // Panel
            auto tpr = TitlePanelRect();

            g_titleBarPanelBrush->SetStartPoint({ tpr.left, tpr.bottom });
            g_titleBarPanelBrush->SetEndPoint({ tpr.left, tpr.top });

            rndr->d2d1DeviceContext->FillRectangle(tpr, g_titleBarPanelBrush.Get());

            // Title text had been drawn as child.

            // Decorative Bar
            auto dbr = DecorativeBarRect();

            g_decorativeBarBrush->SetStartPoint({ dbr.left, dbr.top });
            g_decorativeBarBrush->SetEndPoint({ dbr.right, dbr.top });

            rndr->d2d1DeviceContext->FillRectangle(dbr, g_decorativeBarBrush.Get());
        }
        // 3 Brothers
        {
            // Close Button
            {
                Set3BrothersButtonBrushState(m_isCloseHover, m_isCloseDown);
                rndr->d2d1DeviceContext->FillRectangle(CloseButtonRect(), UIResu::SOLID_COLOR_BRUSH.Get());

                // Close icon had been drawn as child.
            }
            // Maximize Button
            {
                auto mbr = MaximizeButtonRect();

                Set3BrothersButtonBrushState(m_isMaximizeHover, m_isMaximizeDown);
                rndr->d2d1DeviceContext->FillRectangle(mbr, UIResu::SOLID_COLOR_BRUSH.Get());

                Set3BrothersIconBrushState();
                rndr->d2d1DeviceContext->DrawRectangle(MaximizeIconRect(), UIResu::SOLID_COLOR_BRUSH.Get(), 2.0f);
            }
            // Minimize Button
            {
                Set3BrothersButtonBrushState(m_isMinimizeHover, m_isMinimizeDown);
                rndr->d2d1DeviceContext->FillRectangle(MinimizeButtonRect(), UIResu::SOLID_COLOR_BRUSH.Get());

                Set3BrothersIconBrushState();
                rndr->d2d1DeviceContext->FillRectangle(MinimizeIconRect(), UIResu::SOLID_COLOR_BRUSH.Get());
            }
        }
        PostMaskToScene(rndr->d2d1DeviceContext.Get(), m_absoluteRect);
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
            // Update button states of 3 brothers.
            m_isCloseDown = Mathu::IsInside(p, CloseButtonRect());
            m_isMaximizeDown = Mathu::IsInside(p, MaximizeButtonRect());
            m_isMinimizeDown = Mathu::IsInside(p, MinimizeButtonRect());

            // Check whether is dragging title bar.
            if (!m_isCloseHover && !m_isMaximizeHover && !m_isMinimizeHover)
            {
                if (m_isDragTitleBar = Mathu::IsInside(p, TitlePanelRect()))
                {
                    m_dragPoint = AbsoluteToSelfCoord(p);
                    // In case the mouse dashes out of the boundary.
                    Application::APP->PinUIObject(shared_from_this());

                    Application::CURSOR->SetIcon(Cursor::IconIndex::Move);
                }
            }

            m_isLeftSizing = m_isLeftHover;
            m_isTopSizing = m_isTopHover;
            m_isRightSizing = m_isRightHover;
            m_isBottomSizing = m_isBottomHover;

            // Check whether is holding window frame.
            if (m_isLeftSizing || m_isTopSizing || m_isRightSizing || m_isBottomSizing)
            {
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
                m_isDragTitleBar = false;
                Application::APP->UnpinUIObject(shared_from_this());
            }

            // Always cancel sizing if left button up.
            if (m_isLeftSizing || m_isTopSizing || m_isRightSizing || m_isBottomSizing)
            {
                m_isLeftSizing = m_isTopSizing = m_isRightSizing = m_isBottomSizing = false;
                Application::APP->UnpinUIObject(shared_from_this());
            }
        }
        return Panel::OnMouseButtonHelper(e);
    }

    bool Window::OnMouseMoveHelper(MouseMoveEvent& e)
    {
        auto p = e.cursorPoint;
        
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

        // Trigger movement if drag title bar.
        if (m_isDragTitleBar)
        {
            auto relative = AbsoluteToRelative(p);
            Move(relative.x - m_dragPoint.x, relative.y - m_dragPoint.y);

            Application::CURSOR->SetIcon(Cursor::IconIndex::Move);
        }

        // Trigger resizing if hold window frame.
        {
            auto relative = AbsoluteToRelative(p);

            // The sizing point is not aligned to window boundary strictly since the sizing event
            // will be triggered when mouse cursor moves on shadow frame. This causes the point
            // actually offsets the boundary by a few pixels, which is stored in m_sizingOffset.
            relative.x -= m_sizingOffset.x;
            relative.y -= m_sizingOffset.y;

            // Left

            if (m_isLeftSizing && !m_isTopSizing && !m_isRightSizing && !m_isBottomSizing)
            {
                float afterWidth = m_rect.right - relative.x;

                if (afterWidth >= MinimalWidth())
                {
                    Transform(relative.x, m_rect.top, afterWidth, Height());
                }
                Application::CURSOR->SetIcon(Cursor::IconIndex::HorzSize);
            }

            // Right

            else if (m_isRightSizing && !m_isLeftSizing && !m_isTopSizing && !m_isBottomSizing)
            {
                float afterWidth = relative.x - m_rect.left;

                if (afterWidth >= MinimalWidth())
                {
                    Transform(m_rect.left, m_rect.top, afterWidth, Height());
                }
                Application::CURSOR->SetIcon(Cursor::IconIndex::HorzSize);
            }

            // Top

            else if (m_isTopSizing && !m_isLeftSizing && !m_isRightSizing && !m_isBottomSizing)
            {
                float afterHeight = m_rect.bottom - relative.y;

                if (afterHeight >= MinimalHeight())
                {
                    Transform(m_rect.left, relative.y, Width(), afterHeight);
                }
                Application::CURSOR->SetIcon(Cursor::IconIndex::VertSize);
            }

            // Bottom

            else if (m_isBottomSizing && !m_isLeftSizing && !m_isTopSizing && !m_isRightSizing)
            {
                float afterHeight = relative.y - m_rect.top;

                if (afterHeight >= MinimalHeight())
                {
                    Transform(m_rect.left, m_rect.top, Width(), afterHeight);
                }
                Application::CURSOR->SetIcon(Cursor::IconIndex::VertSize);
            }

            // Left Top

            else if (m_isLeftSizing && m_isTopSizing && !m_isRightSizing && !m_isBottomSizing)
            {
                float afterWidth = m_rect.right - relative.x;
                float afterHeight = m_rect.bottom - relative.y;

                float minWidth = MinimalWidth();
                float minHeight = MinimalHeight();

                minWidth = max(minWidth, afterWidth);
                minHeight = max(minHeight, afterHeight);

                if (afterWidth >= MinimalWidth() && afterHeight >= MinimalHeight())
                {
                    Transform(relative.x, relative.y, minWidth, minHeight);
                }
                else if (afterWidth >= MinimalWidth())
                {
                    Transform(relative.x, m_rect.top, minWidth, Height());
                }
                else if (afterHeight >= MinimalHeight())
                {
                    Transform(m_rect.left, relative.y, Width(), minHeight);
                }
                Application::CURSOR->SetIcon(Cursor::IconIndex::MainDiagSize);
            }

            // Left Bottom

            else if (m_isLeftSizing && m_isBottomSizing && !m_isRightSizing && !m_isTopSizing)
            {
                float afterWidth = m_rect.right - relative.x;
                float afterHeight = relative.y - m_rect.top;

                float minWidth = MinimalWidth();
                float minHeight = MinimalHeight();

                minWidth = max(minWidth, afterWidth);
                minHeight = max(minHeight, afterHeight);

                if (afterWidth >= MinimalWidth() && afterHeight >= MinimalHeight())
                {
                    Transform(relative.x, m_rect.top, minWidth, minHeight);
                }
                else if (afterWidth >= MinimalWidth())
                {
                    Transform(relative.x, m_rect.top, minWidth, Height());
                }
                else if (afterHeight >= MinimalHeight())
                {
                    Transform(m_rect.left, m_rect.top, Width(), minHeight);
                }
                Application::CURSOR->SetIcon(Cursor::IconIndex::BackDiagSize);
            }

            // Right Top

            else if (m_isRightSizing && m_isTopSizing && !m_isLeftSizing && !m_isBottomSizing)
            {
                float afterWidth = relative.x - m_rect.left;
                float afterHeight = m_rect.bottom - relative.y;

                float minWidth = MinimalWidth();
                float minHeight = MinimalHeight();

                minWidth = max(minWidth, afterWidth);
                minHeight = max(minHeight, afterHeight);

                if (afterWidth >= MinimalWidth() && afterHeight >= MinimalHeight())
                {
                    Transform(m_rect.left, relative.y, minWidth, minHeight);
                }
                else if (afterWidth >= MinimalWidth())
                {
                    Transform(m_rect.left, m_rect.top, minWidth, Height());
                }
                else if (afterHeight >= MinimalHeight())
                {
                    Transform(m_rect.left, relative.y, Width(), minHeight);
                }
                Application::CURSOR->SetIcon(Cursor::IconIndex::BackDiagSize);
            }

            // Right Bottom

            else if (m_isRightSizing && m_isBottomSizing && !m_isLeftSizing && !m_isTopSizing)
            {
                float afterWidth = relative.x - m_rect.left;
                float afterHeight = relative.y - m_rect.top;

                float minWidth = MinimalWidth();
                float minHeight = MinimalHeight();

                minWidth = max(minWidth, afterWidth);
                minHeight = max(minHeight, afterHeight);

                if (afterWidth >= MinimalWidth() && afterHeight >= MinimalHeight())
                {
                    Transform(m_rect.left, m_rect.top, minWidth, minHeight);
                }
                else if (afterWidth >= MinimalWidth())
                {
                    Transform(m_rect.left, m_rect.top, minWidth, Height());
                }
                else if (afterHeight >= MinimalHeight())
                {
                    Transform(m_rect.left, m_rect.top, Width(), minHeight);
                }
                Application::CURSOR->SetIcon(Cursor::IconIndex::MainDiagSize);
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

                    Application::CURSOR->SetIcon(Cursor::IconIndex::MainDiagSize);
                }
                else if ((p.x <= m_absoluteRect.left && p.y >= m_absoluteRect.bottom) ||
                         (p.x >= m_absoluteRect.right && p.y <= m_absoluteRect.top))
                {
                    if (p.x <= m_absoluteRect.left)
                    {
                        m_isLeftHover = m_isBottomHover = true;
                    }
                    else m_isRightHover = m_isTopHover = true;

                    Application::CURSOR->SetIcon(Cursor::IconIndex::BackDiagSize);
                }
                else if (p.x <= m_absoluteRect.left || p.x >= m_absoluteRect.right)
                {
                    if (p.x <= m_absoluteRect.left)
                    {
                        m_isLeftHover = true;
                    }
                    else m_isRightHover = true;

                    Application::CURSOR->SetIcon(Cursor::IconIndex::HorzSize);
                }
                else if (p.y <= m_absoluteRect.top || p.y >= m_absoluteRect.bottom)
                {
                    if (p.y <= m_absoluteRect.top)
                    {
                        m_isTopHover = true;
                    }
                    else m_isBottomHover = true;

                    Application::CURSOR->SetIcon(Cursor::IconIndex::VertSize);
                }
                else if (!m_isDragTitleBar) Application::CURSOR->SetIcon(Cursor::IconIndex::Arrow);
            }
        }
        return Panel::OnMouseMoveHelper(e);
    }
}