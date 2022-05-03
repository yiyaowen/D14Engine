#include "Precompile.h"

#include "UI/Application.h"

#include "UI/UIResourceUtils.h"
#include "UI/Window.h"

namespace d14engine::ui
{
    Application* Application::APP = nullptr;
    Renderer* Application::RNDR = nullptr;
    Cursor* Application::CURSOR = nullptr;

    Application::Application(int argc, wchar_t* argv[], const CreateInfo& info)
    {
        APP = this;

        HINSTANCE hInstance;
        WNDCLASS wndclass;

        hInstance = GetModuleHandle(nullptr);

        wndclass = {};
        wndclass.lpfnWndProc = WndProc;
        // We will populate the window user data
        // with the application instance pointer later.
        wndclass.cbWndExtra = sizeof(this);
        wndclass.hInstance = hInstance;
        wndclass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
        wndclass.hCursor = LoadCursor(nullptr, IDC_ARROW);
        // Take over background clearing from GDI.
        wndclass.hbrBackground = nullptr;
        wndclass.lpszClassName = info.name.c_str();

        RegisterClass(&wndclass);

        RECT rc = {};
        // We have to decide the window's initial position and size manually
        // since CW_USEDEFAULT doesn't work when set style to WS_POPUP.
        SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
        int workAreaWidth = rc.right - rc.left;
        int workAreaHeight = rc.bottom - rc.top;

        int x, y, width, height;
        if (info.showMaximized)
        {
            x = y = 0;
            width = workAreaWidth;
            height = workAreaHeight;
        }
        else if (info.rootWindowRect == CreateInfo::DEFAULT_INITIAL_RECT)
        {
            x = workAreaWidth / 10;
            y = workAreaHeight / 12;
            width = workAreaWidth * 4 / 5;
            height = workAreaHeight * 5 / 6;
        }
        else // Use the user specified rectangle.
        {
            auto& userRect = info.rootWindowRect;
            x = userRect.left;
            y = userRect.top;
            width = userRect.right - userRect.left;
            height = userRect.bottom - userRect.top;
        }

        THROW_IF_NULL(m_window = CreateWindowEx(
            // Prevent DWM from drawing again.
            WS_EX_NOREDIRECTIONBITMAP,
            info.name.c_str(),
            info.name.c_str(),
            WS_POPUP,
            x,
            y,
            width,
            height,
            nullptr,
            nullptr,
            hInstance,
            nullptr));

        // Pass application instance pointer through the window user data,
        // so that we can use the static method to handle all callbacks.
        SetWindowLongPtr(m_window, GWLP_USERDATA, (LONG_PTR)this);

        RNDR = (m_renderer = std::make_shared<Renderer>(m_window)).get();

        // Initialize miscellaneous components.
        UIResu::Initialize();

        Bitmapu::Initialize();

        Window::LoadCommonResources();

        // We don't want the cursor to receive any UI event, so only register draw parts.
        (CURSOR = (m_cursor = std::make_shared<Cursor>(
            Cursor::LoadBasicIconSeries())).get())->RegisterDrawObjects();
    }

    int Application::Run(const std::function<void(Application* app)>& onLaunch)
    {
        onLaunch(this);

        ShowWindow(m_window, SW_SHOW);
        UpdateWindow(m_window);

        MSG msg;
        while (true)
        {
            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                if (msg.message == WM_QUIT)
                {
                    break;
                }
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        return (int)msg.wParam;
    }

    void Application::AddUIObject(ShrdPtrParam<Panel> uiobj)
    {
        if (uiobj == nullptr) return;
        m_uiobjects.insert(uiobj);
    }

    void Application::RemoveUIObject(ShrdPtrParam<Panel> uiobj)
    {
        m_uiobjects.erase(uiobj);
    }

    void Application::PinUIObject(ShrdPtrParam<Panel> uiobj)
    {
        if (uiobj == nullptr) return;
        m_pinnedUIObject.insert(uiobj);
    }

    void Application::UnpinUIObject(ShrdPtrParam<Panel> uiobj)
    {
        m_pinnedUIObject.erase(uiobj);
    }

    LRESULT CALLBACK Application::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        auto app = (Application*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

        switch (message)
        {
        case WM_ERASEBKGND:
        {
            // Take over background clearing from GDI.
            return TRUE;
        }
        case WM_PAINT:
        {
            if (app != nullptr && app->m_renderer != nullptr)
            {
                app->m_renderer->DrawNextFrame();
            }
            return 0;
        }
        case WM_WINDOWPOSCHANGING:
        {
            DefWindowProc(hwnd, message, wParam, lParam);
            // Help reduce the possibility of window flicker/jitter.
            ((WINDOWPOS*)lParam)->flags |= SWP_NOCOPYBITS;

            if (app != nullptr && app->m_renderer != nullptr)
            {
                app->m_renderer->OnWindowResize();
                // Notify the renderer to draw immediately after resized,
                // which can reduce the possibility of content tearing.
                app->m_renderer->DrawNextFrame();
            }
            return 0;
        }
        case WM_SETCURSOR:
        {
            // Take over cursor drawing from GDI.
            SetCursor(nullptr);
            return 0;
        }
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        {
            if (message == WM_LBUTTONDOWN ||
                message == WM_RBUTTONDOWN ||
                message == WM_MBUTTONDOWN)
            {
                SetCapture(hwnd);
            }
            else ReleaseCapture();

            MouseButtonEvent e = {};
            e.cursorPoint = { (float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam) };
            {
                auto& flag = e.status.flag;
                using Flag = MouseButtonEvent::Status::Flag;
                switch (message)
                {
                case WM_LBUTTONDOWN: flag = Flag::LeftDown; break;
                case WM_LBUTTONUP: flag = Flag::LeftUp; break;
                case WM_RBUTTONDOWN: flag = Flag::RightDown; break;
                case WM_RBUTTONUP: flag = Flag::RightUp; break;
                case WM_MBUTTONDOWN: flag = Flag::MiddleDown; break;
                case WM_MBUTTONUP: flag = Flag::MiddleUp; break;
                default: flag = Flag::Unknown; break;
                }
            }

            // Only broadcast the event to those hit widgets.
            for (auto& uiobj : app->m_hitUIObjects)
            {
                if (uiobj->IsMouseButtonSensitive())
                {
                    // Note the return boolean means whether to continue delivering the event.
                    if (!uiobj->OnMouseButton(e)) break;
                }
            }
            for (auto& uiobj : app->m_pinnedUIObject)
            {
                if (uiobj->IsMouseButtonSensitive())
                {
                    if (!uiobj->OnMouseButton(e)) break;
                }
            }
            return 0;
        }
        case WM_MOUSEMOVE:
        {
            static D2D1_POINT_2F LAST_CURSOR_POINT = {};

            D2D1_POINT_2F cursorPoint =
            {
                (float)GET_X_LPARAM(lParam),
                (float)GET_Y_LPARAM(lParam)
            };
            // Update application cursor position & icon.
            app->m_cursor->Move(cursorPoint.x, cursorPoint.y);
            app->m_cursor->SetIcon(Cursor::IconIndex::Arrow);

            UIObjectPrioritySet currHitUIObjects;
            // m_hitUIObject now stores last hit UI objects;
            // compare them to broadcast mouse enter & leave events.
            for (auto& uiobj : app->m_uiobjects)
            {
                if (uiobj->IsHit(cursorPoint))
                {
                    currHitUIObjects.insert(uiobj);
                }
            }
            // OnMouseEnter
            MouseEnterEvent mee = {};
            mee.cursorPoint = cursorPoint;

            for (auto& uiobj : currHitUIObjects)
            {
                // Moved in just now, trigger OnMouseEnter event.
                if (app->m_hitUIObjects.find(uiobj) == app->m_hitUIObjects.end())
                {
                    if (uiobj->IsMouseEnterSensitive())
                    {
                        if (!uiobj->OnMouseEnter(mee)) break;
                    }
                }
            }
            // OnMouseLeave
            MouseLeaveEvent mle = {};
            mle.cursorPoint = LAST_CURSOR_POINT;

            for (auto& uiobj : app->m_hitUIObjects)
            {
                // Moved out just now, trigger OnMouseLeave event.
                if (currHitUIObjects.find(uiobj) == currHitUIObjects.end())
                {
                    if (uiobj->IsMouseLeaveSensitive())
                    {
                        if (!uiobj->OnMouseLeave(mle)) break;
                    }
                }
            }
            // Update application's hit UI object set.
            app->m_hitUIObjects = std::move(currHitUIObjects);

            MouseMoveEvent e = {};
            e.cursorPoint = cursorPoint;

            e.buttonState.leftPressed = wParam & MK_LBUTTON;
            e.buttonState.middlePressed = wParam & MK_MBUTTON;
            e.buttonState.rightPressed = wParam & MK_RBUTTON;

            e.keyState.ALT = wParam & MK_ALT;
            e.keyState.CTRL = wParam & MK_CONTROL;
            e.keyState.SHIFT = wParam & MK_SHIFT;

            e.lastCursorPoint = LAST_CURSOR_POINT;
            LAST_CURSOR_POINT = cursorPoint;

            // Only broadcast the event to those hit widgets.
            for (auto& uiobj : app->m_hitUIObjects)
            {
                if (uiobj->IsMouseMoveSensitive())
                {
                    // Note the return boolean means whether to continue delivering the event.
                    if (!uiobj->OnMouseMove(e)) break;
                }
            }
            for (auto& uiobj : app->m_pinnedUIObject)
            {
                if (uiobj->IsMouseMoveSensitive())
                {
                    if (!uiobj->OnMouseMove(e)) break;
                }
            }
            return 0;
        }
        case WM_MOUSEWHEEL:
        {
            MouseWheelEvent e = {};
            POINT screenCursorPoint =
            {
                GET_X_LPARAM(lParam),
                GET_Y_LPARAM(lParam)
            };
            // The origin cursor point in mouse-wheel event is relative to the screen.
            ScreenToClient(hwnd, &screenCursorPoint);
            e.cursorPoint =
            {
                (float)screenCursorPoint.x,
                (float)screenCursorPoint.y
            };

            auto lowParam = LOWORD(wParam);

            e.buttonState.leftPressed = lowParam & MK_LBUTTON;
            e.buttonState.middlePressed = lowParam & MK_MBUTTON;
            e.buttonState.rightPressed = lowParam & MK_RBUTTON;

            e.keyState.CTRL = lowParam & MK_CONTROL;
            e.keyState.SHIFT = lowParam & MK_SHIFT;

            // Don't use HIWORD to extract the wheel distance since it can be negative.
            e.deltaCount = GET_Y_LPARAM(wParam) / WHEEL_DELTA;

            // Only broadcast the event to those hit widgets.
            for (auto& uiobj : app->m_hitUIObjects)
            {
                if (uiobj->IsMouseWheelSensitive())
                {
                    // Note the return boolean means whether to continue delivering the event.
                    if (!uiobj->OnMouseWheel(e)) break;
                }
            }
            for (auto& uiobj : app->m_pinnedUIObject)
            {
                if (uiobj->IsMouseWheelSensitive())
                {
                    if (!uiobj->OnMouseWheel(e)) break;
                }
            }
            return 0;
        }
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            KeyboardEvent e = {};
            e.vkey = wParam;
            {
                auto& flag = e.status.flag;
                using Flag = KeyboardEvent::Status::Flag;
                if (message == WM_KEYDOWN || message == WM_SYSKEYDOWN)
                {
                    flag = Flag::Pressed;
                }
                else flag = Flag::Released;
            }

            // Only broadcast the event to those hit widgets.
            for (auto& uiobj : app->m_hitUIObjects)
            {
                if (uiobj->IsKeyboardSensitive())
                {
                    // Note the return boolean means whether to continue delivering the event.
                    if (!uiobj->OnKeyboard(e)) break;
                }
            }
            for (auto& uiobj : app->m_pinnedUIObject)
            {
                if (uiobj->IsKeyboardSensitive())
                {
                    if (!uiobj->OnKeyboard(e)) break;
                }
            }
            return 0;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
}