#pragma once

#include "Precompile.h"
#include "RuntimeError.h"

#include "Renderer/Renderer.h"
using namespace d14engine::renderer;

#include "BitmapUtils.h"
#include "Cursor.h"
#include "Panel.h"

namespace d14engine::ui
{
    struct Application
    {
        struct CreateInfo
        {
            // This field will be used to register window class firstly,
            // and the root window's caption will be set to this later.
            Wstring name = L"D14Engine";

            bool showMaximized = false;
            RECT rootWindowRect = DEFAULT_INITIAL_RECT;

            constexpr static RECT DEFAULT_INITIAL_RECT = { -1, -1, -1, -1 };
        };

        static Application* APP;
        static Renderer* RNDR;
        static Cursor* CURSOR;

        Application(int argc, wchar_t* argv[], const CreateInfo& info = {});

        int Run(const std::function<void(Application* app)>& onLaunch);

        // Don't use WM_QUIT here!
        // SendMessage will transmit the message to the window process directly,
        // instead of pushing it into the queue and waiting the window to receive that.
        // In this case, we will never get a WM_QUIT from PeekMessage in the main loop.
        void Exit() { SendMessage(m_window, WM_DESTROY, 0, 0); }

        void AddUIObject(ShrdPtrParam<Panel> uiobj);
        void RemoveUIObject(ShrdPtrParam<Panel> uiobj);

        void PinUIObject(ShrdPtrParam<Panel> uiobj);
        void UnpinUIObject(ShrdPtrParam<Panel> uiobj);

    private:
        static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

        Wstring m_name;

        HWND m_window;

        // The application only maintains one Win32 window during its lifecycle,
        // and the renderer will take over the display work of all UI objects.
        // Thus all child windows are rendered within the main window,
        // where the main window actually becomes the "desktop" of the application.
        SharedPtr<Renderer> m_renderer;

        using UIObjectSet = std::unordered_set<SharedPtr<Panel>>;

        UIObjectSet m_uiobjects;

        using UIObjectPrioritySet = std::set<SharedPtr<Panel>, ISortable<Panel>::UniqueAscending>;

        UIObjectPrioritySet m_hitUIObjects;
        
        // Pinned objects would still receive UI events even though they are not hit.
        UIObjectPrioritySet m_pinnedUIObject;

        SharedPtr<Cursor> m_cursor;
    };
}