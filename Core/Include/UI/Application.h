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
            Optional<RECT> rootWindowRect = std::nullopt;
        };

        static Application* APP;

        Application(int argc, wchar_t* argv[], const CreateInfo& info = {});

        int Run(const std::function<void(Application* app)>& onLaunch);

        // Don't use WM_QUIT here!
        // SendMessage will transmit the message to the window process directly,
        // instead of pushing it into the queue and waiting the window to receive that.
        // In this case, we will never get a WM_QUIT from PeekMessage in the main loop.
        void Exit() { SendMessage(m_window, WM_DESTROY, 0, 0); }

        HWND RootWindow() { return m_window; }
        Renderer* MainRenderer() { return m_renderer.get(); }

        void IncreaseAnimateCount();
        void DecreaseAnimateCount();

        void AddUIObject(ShrdPtrParam<Panel> uiobj);
        void RemoveUIObject(ShrdPtrParam<Panel> uiobj);

        void PinUIObject(WeakPtrParam<Panel> uiobj);
        void UnpinUIObject(WeakPtrParam<Panel> uiobj);

        // Return whether the target UI object exists in specified container.
        bool FindUIObject(ShrdPtrParam<Panel> uiobj);
        bool FindHitUIObject(WeakPtrParam<Panel> uiobj);
        bool FindPinnedUIObject(WeakPtrParam<Panel> uiobj);
        bool FindDiffPinnedUIObject(WeakPtrParam<Panel> uiobj);

        // Return RawPtr directly since the main cursor won't be destroyed until the application exits.
        Cursor* MainCursor();

        WeakPtr<Panel> GetFocusedUIObject();
        // A simple tool method that forwards GetFocusedUIObject.
        bool IsUIObjectFocused(WeakPtrParam<Panel> uiobj);

        // This method will change the focused immediately, which is dangerous
        // when the application events are delivering. See FocusUIObjectLater.
        void FocusUIObject(WeakPtrParam<Panel> uiobj);

        // When the focused is changed some UI object containers will be updated,
        // which will cause undefined result if the application events are delivering.
        // Use FocusUIObjectLater instead to change the focused in the callbacks,
        // and the actual focus transition will be performed in the next event pass.
        void FocusUIObjectLater(WeakPtrParam<Panel> uiobj);

        void ChangeTheme(WstrViewParam themeName);

    private:
        // This method will update the diff-pinned set immediately, which is dangerous
        // when the container is being iterated. See UpdateDiffPinnedUIObjectsLater.
        void UpdateDiffPinnedUIObjects();

        // It's dangerous to call UpdateDiffPinnedUIObjects directly in any callback,
        // since the diff-pinned set is probably being iterated and thus the updating
        // may cause unexpected result. Similar to the method above (focus-uiobj),
        // it's recommended to post a msg instead and update in the next event pass.
        void UpdateDiffPinnedUIObjectsLater();

    private:
        static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

        HWND m_window;

        // The application only maintains one Win32 window during its lifecycle,
        // and the renderer will take over the display work of all UI objects.
        // Thus all children (UI objects) are rendered within the main window,
        // where the main window actually becomes the "desktop" of the application.
        UniquePtr<Renderer> m_renderer;

        // This field indicates whether the renderer should be updated in the main while-loop.
        // When the application keeps idle, there's no need to draw frames continuously;
        // however, when there're objects that perform animation, i.e. animate-count > 0,
        // we have to draw frames in the main while-loop instead of waiting for paint-events.
        int m_animateCount = 0;

        using UIObjectSet = std::unordered_set<SharedPtr<Panel>>;

        UIObjectSet m_uiobjects;

        using UIObjectPrioritySet = ISortable<Panel>::WeakPrioritySet;

        UIObjectPrioritySet m_hitUIObjects;
        
        // Pinned objects keep receiving UI events even though they are not hit.
        UIObjectPrioritySet m_pinnedUIObjects, m_diffPinnedUIObjects;

        SharedPtr<Cursor> m_cursor;

        WeakPtr<Panel> m_focusedUIObject;

    public:
        enum class CustomWinMessage
        {
            ChangeFocusedUIObject = WM_USER,
            UpdateRootDiffPinnedUIObjects = WM_USER + 1,
            UpdateMiscDiffPinnedUIObjects = WM_USER + 2
        };

        void PostCustomWinMessage(CustomWinMessage message);

    private:
        WeakPtr<Panel> m_nextFocusedCandidate;

        using NextDiffPinnedUpdatingCandidates = std::set<WeakPtr<Panel>, std::owner_less<WeakPtr<Panel>>>;

        NextDiffPinnedUpdatingCandidates m_nextDiffPinnedUpdatingCandidates;

    public:
        void MarkDiffPinnedUpdatingCandidate(WeakPtrParam<Panel> uiobj);
    };
}