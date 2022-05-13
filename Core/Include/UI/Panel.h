#pragma once

#include "Precompile.h"

#include "Renderer/Renderer.h"
using namespace d14engine::renderer;

#include "Event.h"

namespace d14engine::ui
{
    template<typename T, typename... Types>
    SharedPtr<T> MakeUIObject(Types&& ...args)
    {
        auto uiobj = std::make_shared<T>(args...);
        uiobj->OnInitializeFinish();
        return uiobj; // In case you forget to initialize.
    }

    template<typename T, typename... Types>
    SharedPtr<T> MakeRootUIObject(Types&& ...args)
    {
        auto uiobj = MakeUIObject<T>(args...);
        uiobj->RegisterDrawObjects();
        uiobj->RegisterApplicationEvents();
        return uiobj; // In case you forget to register root item.
    }

    struct Panel;

    template<typename T, typename... Types>
    SharedPtr<T> MakeManagedUIObject(ShrdPtrParam<Panel> parent, Types&& ...args)
    {
        auto uiobj = MakeUIObject<T>(args...);
        uiobj->SetParent(parent);
        return uiobj; // Help to reduce duplicated SetParent codes.
    }

    struct Panel : ISortable<Panel>, IDrawObject2D, std::enable_shared_from_this<Panel>
    {
        // Implement interface methods.

        // IDrawObject2D
        bool IsD2D1ObjectVisible() override;

        void SetD2D1ObjectVisible(bool value) override;

        void OnRendererUpdateObject2D(Renderer* rndr) override;

        void OnRendererDrawD2D1Layer(Renderer* rndr) override;

        void OnRendererDrawD2D1Object(Renderer* rndr) override;

        // Implement struct methods.

        Panel(
            const D2D1_RECT_F& rect,
            ComPtrParam<ID2D1Brush> brush = nullptr,
            ComPtrParam<ID2D1Bitmap1> bitmap = nullptr);

        virtual void OnInitializeFinish() { }

        virtual bool IsHit(Event::Point& p);

        // We don't make this virtual since it always returns <minimal-width, minimal-height>.
        D2D1_SIZE_F MinimalSize();

        virtual float MinimalWidth();
        virtual float MinimalHeight();

        virtual void RegisterDrawObjects();
        virtual void UnregisterDrawObjects();

        virtual void RegisterApplicationEvents();
        virtual void UnregisterApplicationEvents();

        /*
        * Introduce OnxxxHelper to solve the inheritance conflicts of
        * the 'override', 'before' and 'after' event callback lambdas.
        * 
        * We expect these lambdas to be called in the correct place in the entire inheritance tree.
        * For example, suppose Window is inherited from Panel, and overrides OnSize event callback,
        * but we want 'before' lambda to be called before Window's OnSize instead of Panel's OnSize.
        * To achieve this mechanism, we can place the actual works in OnSizeHelper method
        * and then rewrite Window's OnSize method to call Window's OnSizeHelper internally,
        * and Window's OnSizeHelper calls Panel's OnSizeHelper and finishes its own works.
        * 
        * |----------|----------------------------------------|---------------------------------------------|
        * | Class    | OnSize                                 | OnSizeHelper                                |
        * |----------|----------------------------------------|---------------------------------------------|
        * | Panel    | 'before' ; Call Panel's OnSizeHelper   | Panel's works                               |
        * |----------|----------------------------------------|---------------------------------------------|
        * | Window   | 'before' ; Call Window's OnSizeHelper  | Call Panel's OnSizeHelper ; Window's works  |
        * |----------|----------------------------------------|---------------------------------------------|
        * 
        * To sum up, do the actual works in OnxxxHelper method and wrap it into Onxxx method.
        */

        void OnSize(SizeEvent& e);
        virtual void OnSizeHelper(SizeEvent& e);

        Function<void(Panel*,SizeEvent&)>
            f_onSizeOverride = {},
            f_onSizeBefore = {},
            f_onSizeAfter = {};

        void OnParentSize(SizeEvent& e);
        virtual void OnParentSizeHelper(SizeEvent& e);

        Function<void(Panel*,SizeEvent&)>
            f_onParentSizeOverride = {},
            f_onParentSizeBefore = {},
            f_onParentSizeAfter = {};

        void OnMove(MoveEvent& e);
        virtual void OnMoveHelper(MoveEvent& e);

        Function<void(Panel*,MoveEvent&)>
            f_onMoveOverride = {},
            f_onMoveBefore = {},
            f_onMoveAfter = {};

        void OnParentMove(MoveEvent& e);
        virtual void OnParentMoveHelper(MoveEvent& e);

        Function<void(Panel*,MoveEvent&)>
            f_onParentMoveOverride = {},
            f_onParentMoveBefore = {},
            f_onParentMoveAfter = {};

        // Set this field to true if expect to receive get/lose focus event.
        bool isFocusable = false;

        // These event callbacks return a boolean value,
        // which means whether the UI object should be set/lose focus actually.

        bool OnGetFocus();
        virtual bool OnGetFocusHelper();

        Function<bool(void)>
            f_onGetFocusOverride = {},
            f_onGetFocusBefore = {},
            f_onGetFocusAfter = {};

        bool OnLoseFocus();
        virtual bool OnLoseFocusHelper();

        Function<bool()>
            f_onLoseFocusOverride = {},
            f_onLoseFocusBefore = {},
            f_onLoseFocusAfter = {};

        // These event callbacks return a boolean value,
        // which means whether to continue delivering the event to other UI objects.

        bool OnMouseButton(MouseButtonEvent& e);
        virtual bool OnMouseButtonHelper(MouseButtonEvent& e);

        Function<bool(Panel*,MouseButtonEvent&)>
            f_onMouseButtonOverride = {},
            f_onMouseButtonBefore = {},
            f_onMouseButtonAfter = {};

        bool OnMouseEnter(MouseEnterEvent& e);
        virtual bool OnMouseEnterHelper(MouseEnterEvent& e);

        Function<bool(Panel*,MouseEnterEvent&)>
            f_onMouseEnterOverride = {},
            f_onMouseEnterBefore = {},
            f_onMouseEnterAfter = {};

        bool OnMouseMove(MouseMoveEvent& e);
        virtual bool OnMouseMoveHelper(MouseMoveEvent& e);

        Function<bool(Panel*,MouseMoveEvent&)>
            f_onMouseMoveOverride = {},
            f_onMouseMoveBefore = {},
            f_onMouseMoveAfter = {};

        bool OnMouseLeave(MouseLeaveEvent& e);
        virtual bool OnMouseLeaveHelper(MouseLeaveEvent& e);

        Function<bool(Panel*,MouseLeaveEvent&)>
            f_onMouseLeaveOverride = {},
            f_onMouseLeaveBefore = {},
            f_onMouseLeaveAfter = {};

        bool OnMouseWheel(MouseWheelEvent& e);
        virtual bool OnMouseWheelHelper(MouseWheelEvent& e);

        Function<bool(Panel*,MouseWheelEvent&)>
            f_onMouseWheelOverride = {},
            f_onMouseWheelBefore = {},
            f_onMouseWheelAfter = {};

        bool OnKeyboard(KeyboardEvent& e);
        virtual bool OnKeyboardHelper(KeyboardEvent& e);

        Function<bool(Panel*,KeyboardEvent&)>
            f_onKeyboardOverride = {},
            f_onKeyboardBefore = {},
            f_onKeyboardAfter = {};

        // Define a series of detailed response control flags that work together with the "enable" flag.
        // We can use these flags to decide the UI object's application event sensitivity more flexibly;
        // for example, set "enable" as false and "mouse-button" as true to only receive mouse-button event.

        bool IsMouseButtonSensitive();
        void SetMouseButtonSensitive(bool value);

        bool IsMouseEnterSensitive();
        void SetMouseEnterSensitive(bool value);

        bool IsMouseMoveSensitive();
        void SetMouseMoveSensitive(bool value);

        bool IsMouseLeaveSensitive();
        void SetMouseLeaveSensitive(bool value);

        bool IsMouseWheelSensitive();
        void SetMouseWheelSensitive(bool value);

        bool IsKeyboardSensitive();
        void SetKeyboardSensitive(bool value);

    protected:
        bool m_isVisible = true;

        bool m_isMouseButtonSensitive = true;
        bool m_isMouseEnterSensitive = true;
        bool m_isMouseMoveSensitive = true;
        bool m_isMouseLeaveSensitive = true;
        bool m_isMouseWheelSensitive = true;
        bool m_isKeyboardSensitive = true;

        D2D1_RECT_F m_rect;
        
        // Cache this to reduce the computational overhead during rendering.
        D2D1_RECT_F m_absoluteRect;

    public:
        // This is the same as IsD2D1ObjectVisible for Panel since there're only D2D1 objects.
        // A derived class can control the visibility of D3D12 objects by overriding this method.
        virtual bool IsVisible();
        virtual void SetVisible(bool value);

        bool IsEnabled();
        void SetEnabled(bool value);

        D2D1_SIZE_F Size();
        float Width();
        float Height();

        D2D1_POINT_2F Position();
        D2D1_POINT_2F AbsolutePosition();

        SharedPtr<Panel> Parent();
        void SetParent(ShrdPtrParam<Panel> uiobj);

        void AddUIObject(ShrdPtrParam<Panel> uiobj);
        void RemoveUIObject(ShrdPtrParam<Panel> uiobj);

        void PinUIObject(ShrdPtrParam<Panel> uiobj);
        void UnpinUIObject(ShrdPtrParam<Panel> uiobj);

        // Don't call this method directly in any callback, use xxxLater instead.
        // See UpdateDiffPinnedUIObjectsLater in Application.h for explanations.
        void UpdateDiffPinnedUIObjects();
        void UpdateDiffPinnedUIObjectsLater();

        void PinApplicationEvents();
        void UnpinApplicationEvents();

        // Pass through RawPtr directly instead of SharedPtr since the iteration
        // is performed temporarily and no child should be referenced externally.
        void ForeachChild(const Function<void(Panel*)>& func);

        // Absolute position is relative to the root window (HWND).

        D2D1_POINT_2F AbsoluteToRelative(const D2D1_POINT_2F& p);
        D2D1_RECT_F AbsoluteToRelative(const D2D1_RECT_F& rect);
        D2D1_POINT_2F AbsoluteToSelfCoord(const D2D1_POINT_2F& p);
        D2D1_RECT_F AbsoluteToSelfCoord(const D2D1_RECT_F& rect);

        // Relative position is relative to parent object.

        D2D1_POINT_2F RelativeToAbsolute(const D2D1_POINT_2F& p);
        D2D1_RECT_F RelativeToAbsolute(const D2D1_RECT_F& rect);
        D2D1_POINT_2F RelativeToSelfCoord(const D2D1_POINT_2F& p);
        D2D1_RECT_F RelativeToSelfCoord(const D2D1_RECT_F& rect);

        // Selfcoord position is relative to this object.

        D2D1_POINT_2F SelfCoordToAbsolute(const D2D1_POINT_2F& p);
        D2D1_RECT_F SelfCoordToAbsolute(const D2D1_RECT_F& rect);
        D2D1_POINT_2F SelfCoordToRelative(const D2D1_POINT_2F& p);
        D2D1_RECT_F SelfCoordToRelative(const D2D1_RECT_F& rect);

        // All transform operations are based on relative position.

        void Transform(float left, float top, float width, float height);

        void Move(float left, float top);

        void Resize(float width, float height);

        // Miscellaneous methods.
        void SetUIObjectPriority(int value);
        void SetD2D1ObjectPriority(int value);

        void UpdateAbsoluteRect();

        const D2D1_RECT_F& AbsoluteRect();
        const D2D1_RECT_F& RelativeRect();

        D2D1_RECT_F SelfCoordRect();

    public:
        // Use the first valid object found to draw the background;
        // For example, to fill in the background with the brush,
        // you need to initialize the brush and set bitmap to nullptr.

        ComPtr<ID2D1Brush> brush = nullptr;

        float bitmapOpaque = 1.0f;
        ComPtr<ID2D1Bitmap1> bitmap = nullptr;

        float roundRadiusX = 0.0f, roundRadiusY = 0.0f;

    protected:
        void UpdateChildrenObjects(Renderer* rndr);

        void DrawChildrenLayers(Renderer* rndr);

        void DrawBackground(Renderer* rndr);

        void DrawChildrenObjects(Renderer* rndr);

    protected:
        Renderer::DrawObject2DSet m_drawObjects2D;

        // Don't use SharedPtr here since it will cause circular reference.
        WeakPtr<Panel> m_parent;

        using ChildObjectSet = std::unordered_set<SharedPtr<Panel>>;

        ChildObjectSet m_children;

        using ChildObjectPrioritySet = ISortable<Panel>::WeakPrioritySet;

        ChildObjectPrioritySet m_hitChildren;

        // Pinned children would still receive UI events even though they are not hit.
        ChildObjectPrioritySet m_pinnedChildren, m_diffPinnedChildren;
    };
}