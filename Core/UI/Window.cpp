#include "Precompile.h"

#include "UI/Window.h"

#include "Renderer/MathUtils.h"
using namespace d14engine::renderer;

#include "UI/Application.h"
#include "UI/TabGroup.h"

namespace d14engine::ui
{
    ComPtr<ID2D1LinearGradientBrush>
        Window::g_titleBarPanelBrush = {},
        Window::g_decorativeBarBrush = {};

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
            THROW_IF_FAILED(Application::APP->MainRenderer()->d2d1DeviceContext->
                CreateGradientStopCollection(stop, _countof(stop), &coll));

            THROW_IF_FAILED(Application::APP->MainRenderer()->d2d1DeviceContext->
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
            THROW_IF_FAILED(Application::APP->MainRenderer()->d2d1DeviceContext->
                CreateGradientStopCollection(stop, _countof(stop), &coll));

            THROW_IF_FAILED(Application::APP->MainRenderer()->d2d1DeviceContext->
                CreateLinearGradientBrush({}, coll.Get(), &g_decorativeBarBrush));
        }
    }

    Window::Window(WstrParam title, const D2D1_RECT_F& rect)
        :
        ResizablePanel(rect, UIResu::SOLID_COLOR_BRUSH),
        MaskStyle((UINT)Width(), (UINT)Height()),
        SolidStyle({ 0.95f, 0.95f, 0.95f, 1.0f }),
        ShadowStyle((UINT)Width(), (UINT)Height(), 3.0f, { 0.7f, 0.7f, 0.7f, 1.0f })
    {
        // All children will be drawn on the mask instead of in Panel.
        m_takeOverChildrenDrawing = true;

        // Create title bar text label.
        m_title = MakeUIObject<Label>(
            title, CenterTitleRect(), UIResu::TEXT_FORMATS.at(L"微软雅黑/Light/16"));

        m_title->alignment.vertical = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;

        // Note p is m_title. Also see Panel's OnParentSize for details.
        m_title->f_onParentSizeBefore = [this](Panel* p, SizeEvent& e)
        {
            auto centerRect = CenterTitleRect();
            p->Resize( // Keep in horizontal center.
                      centerRect.right - centerRect.left,
                      centerRect.bottom - centerRect.top);
        };
    }

    void Window::OnInitializeFinish()
    {
        ResizablePanel::OnInitializeFinish();

        m_title->SetParent(shared_from_this());
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

    const Wstring& Window::Title()
    {
        return m_title->Text();
    }

    void Window::SetTitle(WstrViewParam title)
    {
        m_title->SetText(title);
    }

    float Window::ClientAreaHeight()
    {
        return Height() - 40.0f;
    }

    D2D1_RECT_F Window::ClientAreaRect()
    {
        // Don't use ClientAreaHeight() since the 4th field is "bottom",
        // which should be Height() in the window's relative coordinate.
        return { 0.0f, 40.0f, Width(), Height() };
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

    D2D1_RECT_F Window::CenterTitleRect()
    {
        // Magic Number - the minimize button is 124.0f from title bar right,
        // and the title text is going to be placed in the horizontal center.
        return { 124.0f, 0.0f, std::max(Width() - 124.0f, 124.0f), 32.0f };
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

    D2D1_RECT_F Window::CloseIconRect()
    {
        auto cbr = CloseButtonRect();
        return { cbr.left + 15.0f, cbr.top + 7.0f, cbr.right - 15.0f, cbr.bottom - 7.0f };
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
        return { mbr.left + 11.0f, mbr.top + 7.0f, mbr.right - 11.0f, mbr.bottom - 7.0f };
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
        return { mbr.left + 10.0f, mbr.top + 14.0f, mbr.right - 10.0f, mbr.bottom - 8.0f };
    }

    Window::ThreeBrosState Window::GetMinMaxBroState(bool isHover, bool isDown)
    {
        if (isDown) return ThreeBrosState::Down;
        else if (isHover) return ThreeBrosState::Hover;
        else return ThreeBrosState::Idle;
    }

    Window::ThreeBrosState Window::GetCloseXBroState(bool isHover, bool isDown)
    {
        if (isDown) return ThreeBrosState::CloseDown;
        else if (isHover) return ThreeBrosState::CloseHover;
        else return ThreeBrosState::CloseIdle;
    }

    void Window::Set3BrothersButtonBrushState(ThreeBrosState state)
    {
        auto& setting = threeBrosAppearances[(size_t)state];

        UIResu::SOLID_COLOR_BRUSH->SetColor(setting.buttonColor);
        UIResu::SOLID_COLOR_BRUSH->SetOpacity(setting.buttonOpacity);
    }

    void Window::Set3BrothersIconBrushState(ThreeBrosState state)
    {
        auto& setting = threeBrosAppearances[(size_t)state];

        UIResu::SOLID_COLOR_BRUSH->SetColor(setting.iconColor);
        UIResu::SOLID_COLOR_BRUSH->SetOpacity(setting.iconOpacity);
    }

    SharedPtr<Panel> Window::CenterUIObject()
    {
        return m_centerUIObject.lock();
    }

    void Window::SetCenterUIObject(ShrdPtrParam<Panel> uiobj)
    {
        if (uiobj != nullptr)
        {
            AddUIObject(uiobj);
            m_centerUIObject = uiobj;

            uiobj->f_onParentSizeAfter = [&](Panel* p, SizeEvent& e)
            {
                auto parent = p->Parent();
                if (!parent.expired())
                {
                    auto wptr = std::static_pointer_cast<Window>(parent.lock());
                    p->Resize(wptr->Width(), wptr->ClientAreaHeight());
                }
            };
            uiobj->Transform(0.0f, Window::NonClientAreaHeight(), Width(), ClientAreaHeight());
        }
        else if (!m_centerUIObject.expired())
        {
            RemoveUIObject(m_centerUIObject.lock());
            m_centerUIObject.reset();
        }
    }

    void Window::RegisterTabGroup(WeakPtrParam<TabGroup> tg)
    {
        m_registeredTabGroups.insert(tg);
    }

    void Window::UnregisterTabGroup(WeakPtrParam<TabGroup> tg)
    {
        m_registeredTabGroups.erase(tg);
    }

    void Window::HandleMouseButtonForRegisteredTabGroups(MouseButtonEvent& e)
    {
        for (auto tgItor = m_registeredTabGroups.begin(); tgItor != m_registeredTabGroups.end(); )
        {
            if (!tgItor->expired())
            {
                auto tg = tgItor->lock();

                // Update tab-group's associated-window field.
                if (m_isDragTitleBar)
                {
                    tg->temporaryAssociatedWindow = std::static_pointer_cast<Window>(shared_from_this());
                }
                else tg->temporaryAssociatedWindow.reset();

                tgItor++; // Remove expired elements by the way.
            }
            else tgItor = m_registeredTabGroups.erase(tgItor);
        }
        if (e.status.LeftUp())
        {
            // Demote this window to tab-group's new page.
            if (!temporaryAssociatedTabGroup.expired())
            {
                auto tg = temporaryAssociatedTabGroup.lock();

                UnregisterDrawObjects();
                UnregisterApplicationEvents();

                tg->AppendPage({ MakeUIObject<Label>(Title(), D2D1_RECT_F{}), CenterUIObject() });
                tg->SelectPage(tg->AvailablePageCount() - 1); // Show demoted page immediately.
            }
        }
    }

    void Window::HandleMouseMoveForRegisteredTabGroups(MouseMoveEvent& e)
    {
        auto& p = e.cursorPoint;

        temporaryAssociatedTabGroup.reset();
        if (!m_isDragTitleBar) return;

        for (auto tgItor = m_registeredTabGroups.begin(); tgItor != m_registeredTabGroups.end(); )
        {
            if (!tgItor->expired())
            {
                auto tg = tgItor->lock();

                // Check whether is dragging above any tab-group.
                auto tgPos = tg->AbsolutePosition();
                D2D1_RECT_F tgRect =
                {
                    tgPos.x,
                    tgPos.y - tg->tabBarExtendedHeight,
                    tgPos.x + tg->Width(),
                    tgPos.y
                };
                if (Mathu::IsInside(p, tgRect))
                {
                    temporaryAssociatedTabGroup = tg;
                }
                tgItor++; // Remove expired elements by the way.
            }
            else tgItor = m_registeredTabGroups.erase(tgItor);
        }
    }

    void Window::OnRendererDrawD2D1LayerHelper(Renderer* rndr)
    {
        ResizablePanel::OnRendererDrawD2D1LayerHelper(rndr);

        // The shadow will be hiden temporarily when drag above any registered tab-group.
        if (temporaryAssociatedTabGroup.expired())
        {
            // Shape of Shadow
            BeginDrawOnShadow(rndr->d2d1DeviceContext.Get());
            {
                // The color of shadow canvas can be designated casually,
                // where alpha channel will decide the shape of shadow.
                rndr->d2d1DeviceContext->Clear((D2D1::ColorF)D2D1::ColorF::Black);
            }
            EndDrawOnShadow(rndr->d2d1DeviceContext.Get());
        }
        // Content on Mask
        BeginDrawOnMask(rndr->d2d1DeviceContext.Get(), D2D1::Matrix3x2F::Translation(-m_absoluteRect.left, -m_absoluteRect.top));
        {
            // Background
            {
                UIResu::SOLID_COLOR_BRUSH->SetColor(backgroundColor);
                UIResu::SOLID_COLOR_BRUSH->SetOpacity(backgroundOpacity);

                ResizablePanel::DrawBackground(rndr);
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
                    auto state = GetCloseXBroState(m_isCloseHover, m_isCloseDown);

                    Set3BrothersButtonBrushState(state);
                    rndr->d2d1DeviceContext->FillRectangle(CloseButtonRect(), UIResu::SOLID_COLOR_BRUSH.Get());

                    Set3BrothersIconBrushState(state);

                    auto iconRect = CloseIconRect();

                    // Main Diagonal
                    rndr->d2d1DeviceContext->DrawLine(
                        { iconRect.left, iconRect.top },
                        { iconRect.right, iconRect.bottom },
                        UIResu::SOLID_COLOR_BRUSH.Get(),
                        CloseXStrokeWidth());

                    // Back Diagonal
                    rndr->d2d1DeviceContext->DrawLine(
                        { iconRect.right, iconRect.top },
                        { iconRect.left, iconRect.bottom },
                        UIResu::SOLID_COLOR_BRUSH.Get(),
                        CloseXStrokeWidth());
                }
                // Maximize Button
                {
                    auto state = GetMinMaxBroState(m_isMaximizeHover, m_isMaximizeDown);

                    Set3BrothersButtonBrushState(state);
                    rndr->d2d1DeviceContext->FillRectangle(MaximizeButtonRect(), UIResu::SOLID_COLOR_BRUSH.Get());

                    Set3BrothersIconBrushState(state);
                    rndr->d2d1DeviceContext->DrawRectangle(MaximizeIconRect(), UIResu::SOLID_COLOR_BRUSH.Get(), 2.0f);
                }
                // Minimize Button
                {
                    auto state = GetMinMaxBroState(m_isMinimizeHover, m_isMinimizeDown);

                    Set3BrothersButtonBrushState(state);
                    rndr->d2d1DeviceContext->FillRectangle(MinimizeButtonRect(), UIResu::SOLID_COLOR_BRUSH.Get());

                    Set3BrothersIconBrushState(state);
                    rndr->d2d1DeviceContext->FillRectangle(MinimizeIconRect(), UIResu::SOLID_COLOR_BRUSH.Get());
                }
            }
            // Children
            Panel::DrawChildrenObjects(rndr);
        }
        EndDrawOnMask(rndr->d2d1DeviceContext.Get());
    }

    void Window::OnRendererDrawD2D1ObjectHelper(Renderer* rndr)
    {
        // Shadow
        if (temporaryAssociatedTabGroup.expired())
        {
            ConfigShadowEffectInput(UIResu::SHADOW_EFFECT.Get());

            rndr->d2d1DeviceContext->DrawImage(UIResu::SHADOW_EFFECT.Get(), AbsolutePosition());
        }
        // Content
        {
            rndr->d2d1DeviceContext->DrawBitmap(maskBitmap.Get(), m_absoluteRect,
                temporaryAssociatedTabGroup.expired() ? maskOpacity : maskOpacityWhenDragAboveTabGroup);
        }
    }

    float Window::MinimalWidth()
    {
        return NonClientAreaMinimalWidth();
    }

    float Window::MinimalHeight()
    {
        return NonClientAreaHeight();
    }

    void Window::OnSizeHelper(SizeEvent& e)
    {
        ResizablePanel::OnSizeHelper(e);

        auto bitmapWidth = (UINT)(e.size.width + 0.5f);
        auto bitmapHeight = (UINT)(e.size.height + 0.5f);

        LoadMaskBitmap(bitmapWidth, bitmapHeight);
        LoadShadowBitmap(bitmapWidth, bitmapHeight);
    }

    void Window::OnChangeThemeHelper(WstrViewParam themeName)
    {
        ResizablePanel::OnChangeThemeHelper(themeName);

        if (themeName == L"Light")
        {
            backgroundColor = { 0.95f, 0.95f, 0.95f, 1.0f };
            backgroundOpacity = 1.0f;

            shadowColor = { 0.7f, 0.7f, 0.7f, 1.0f };
            shadowOpacity = 1.0f;

            // Title panel, linear gradient, bottom -> top, 80% B -> 95% B.
            {
                ComPtr<ID2D1GradientStopCollection> coll;
                D2D1_GRADIENT_STOP stop[] =
                {
                    { 0.0f, { 0.8f, 0.8f, 0.8f, 1.0f } },
                    { 1.0f, { 0.95f, 0.95f, 0.95f, 1.0f } }
                };
                THROW_IF_FAILED(Application::APP->MainRenderer()->d2d1DeviceContext->
                                CreateGradientStopCollection(stop, _countof(stop), &coll));

                THROW_IF_FAILED(Application::APP->MainRenderer()->d2d1DeviceContext->
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
                THROW_IF_FAILED(Application::APP->MainRenderer()->d2d1DeviceContext->
                                CreateGradientStopCollection(stop, _countof(stop), &coll));

                THROW_IF_FAILED(Application::APP->MainRenderer()->d2d1DeviceContext->
                                CreateLinearGradientBrush({}, coll.Get(), &g_decorativeBarBrush));
            }

            threeBrosAppearances[(size_t)ThreeBrosState::Idle] =
            {
                (D2D1::ColorF)D2D1::ColorF::Black, // button color
                0.0f, // button opacity
                (D2D1::ColorF)D2D1::ColorF::Black, // icon color
                0.8f, // icon opacity
            };
            threeBrosAppearances[(size_t)ThreeBrosState::Hover] =
            {
                (D2D1::ColorF)D2D1::ColorF::Black, // button color
                1.0f, // button opacity
                (D2D1::ColorF)D2D1::ColorF::White, // icon color
                0.8f, // icon opacity
            };
            threeBrosAppearances[(size_t)ThreeBrosState::Down] =
            {
                (D2D1::ColorF)D2D1::ColorF::Black, // button color
                0.6f, // button opacity
                (D2D1::ColorF)D2D1::ColorF::White, // icon color
                0.6f, // icon opacity
            };
            threeBrosAppearances[(size_t)ThreeBrosState::CloseIdle] =
            {
                { 0.78f, 0.12f, 0.2f, 1.0f }, // button color
                0.0f, // button opacity
                (D2D1::ColorF)D2D1::ColorF::Black, // icon color
                1.0f, // icon opacity
            };
            threeBrosAppearances[(size_t)ThreeBrosState::CloseHover] =
            {
                { 0.78f, 0.12f, 0.2f, 1.0f }, // button color
                1.0f, // button opacity
                (D2D1::ColorF)D2D1::ColorF::White, // icon color
                1.0f, // icon opacity
            };
            threeBrosAppearances[(size_t)ThreeBrosState::CloseDown] =
            {
                { 0.78f, 0.12f, 0.2f, 1.0f }, // button color
                0.8f, // button opacity
                (D2D1::ColorF)D2D1::ColorF::White, // icon color
                0.8f, // icon opacity
            };
        }
        else if (themeName == L"Dark")
        {
            backgroundColor = { 0.15f, 0.15f, 0.15f, 1.0f };
            backgroundOpacity = 1.0f;

            shadowColor = { 0.05f, 0.05f, 0.05f, 1.0f };
            shadowOpacity = 1.0f;

            // Title panel, linear gradient, bottom -> top, 15% B -> 25% B.
            {
                ComPtr<ID2D1GradientStopCollection> coll;
                D2D1_GRADIENT_STOP stop[] =
                {
                    { 0.0f, { 0.15f, 0.15f, 0.15f, 1.0f } },
                    { 1.0f, { 0.25f, 0.25f, 0.25f, 1.0f } }
                };
                THROW_IF_FAILED(Application::APP->MainRenderer()->d2d1DeviceContext->
                                CreateGradientStopCollection(stop, _countof(stop), &coll));

                THROW_IF_FAILED(Application::APP->MainRenderer()->d2d1DeviceContext->
                                CreateLinearGradientBrush({}, coll.Get(), &g_titleBarPanelBrush));
            }
            // Decorative bar, linear gradient, left -> right, MediumSeaGreen -> SeaGreen.
            {
                ComPtr<ID2D1GradientStopCollection> coll;
                D2D1_GRADIENT_STOP stop[] =
                {
                    { 0.0f, (D2D1::ColorF)D2D1::ColorF::MediumSeaGreen },
                    { 1.0f, (D2D1::ColorF)D2D1::ColorF::SeaGreen }
                };
                THROW_IF_FAILED(Application::APP->MainRenderer()->d2d1DeviceContext->
                                CreateGradientStopCollection(stop, _countof(stop), &coll));

                THROW_IF_FAILED(Application::APP->MainRenderer()->d2d1DeviceContext->
                                CreateLinearGradientBrush({}, coll.Get(), &g_decorativeBarBrush));
            }

            threeBrosAppearances[(size_t)ThreeBrosState::Idle] =
            {
                (D2D1::ColorF)D2D1::ColorF::Black, // button color
                0.0f, // button opacity
                (D2D1::ColorF)D2D1::ColorF::White, // icon color
                0.8f, // icon opacity
            };
            threeBrosAppearances[(size_t)ThreeBrosState::Hover] =
            {
                { 0.1f, 0.1f, 0.1f, 1.0f }, // button color
                1.0f, // button opacity
                (D2D1::ColorF)D2D1::ColorF::White, // icon color
                0.8f, // icon opacity
            };
            threeBrosAppearances[(size_t)ThreeBrosState::Down] =
            {
                { 0.4f, 0.4f, 0.4f, 0.4f }, // button color
                1.0f, // button opacity
                (D2D1::ColorF)D2D1::ColorF::White, // icon color
                0.6f, // icon opacity
            };
            threeBrosAppearances[(size_t)ThreeBrosState::CloseIdle] =
            {
                { 0.78f, 0.12f, 0.2f, 1.0f }, // button color
                0.0f, // button opacity
                { 0.9f, 0.9f, 0.9f, 1.0f }, // icon color
                1.0f, // icon opacity
            };
            threeBrosAppearances[(size_t)ThreeBrosState::CloseHover] =
            {
                { 0.78f, 0.12f, 0.2f, 1.0f }, // button color
                1.0f, // button opacity
                { 0.9f, 0.9f, 0.9f, 1.0f }, // icon color
                1.0f, // icon opacity
            };
            threeBrosAppearances[(size_t)ThreeBrosState::CloseDown] =
            {
                { 0.78f, 0.32f, 0.32f, 1.0f }, // button color
                1.0f, // button opacity
                { 0.9f, 0.9f, 0.9f, 1.0f }, // icon color
                0.8f, // icon opacity
            };
        }
    }

    bool Window::OnMouseButtonHelper(MouseButtonEvent& e)
    {
        if (e.status.LeftDown() || e.status.MiddleDown() || e.status.RightDown())
        {
            // We decide Window should be a special top-level UI object,
            // which means it will show and respond at top when clicked.
            MoveTopmost();
        }
        auto& p = e.cursorPoint;

        if (e.status.LeftDown())
        {
            if (m_is3BrothersEnabled)
            {
                // Update button states of 3 brothers.
                m_isCloseDown = m_isCloseHover;
                m_isMaximizeDown = m_isMaximizeHover;
                m_isMinimizeDown = m_isMinimizeHover;
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
                    PinApplicationEvents();

                    Application::APP->MainCursor()->SetIcon(Cursor::IconIndex::AllSize);
                }
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

                UnpinApplicationEvents();

                Application::APP->MainCursor()->SetIcon(Cursor::IconIndex::Arrow);
            }
        }
        HandleMouseButtonForRegisteredTabGroups(e);

        return ResizablePanel::OnMouseButtonHelper(e);
    }

    bool Window::OnMouseMoveHelper(MouseMoveEvent& e)
    {
        auto& p = e.cursorPoint;

        m_is3BrothersEnabled = !m_isDragTitleBar &&
            !m_isLeftSizing && !m_isTopSizing && !m_isRightSizing && !m_isBottomSizing;
        
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

        if (m_isDragTitleBar)
        {
            // Trigger movement if drag title bar.
            m_skipDeliverNextMouseMoveEventToChildren = true;

            auto relative = AbsoluteToRelative(p);
            Move(relative.x - m_dragPoint.x, relative.y - m_dragPoint.y);

            Application::APP->MainCursor()->SetIcon(Cursor::IconIndex::AllSize);
        }
        HandleMouseMoveForRegisteredTabGroups(e);

        return ResizablePanel::OnMouseMoveHelper(e);
    }
}