#pragma once

#include "Common/Precompile.h"

#include "Renderer/Renderer.h"
using namespace d14engine::renderer;

#include "UIKit/Event.h"

namespace d14engine::uikit
{
    template<typename T, typename... Types>
    SharedPtr<T> MakeUIObject(Types&& ...args)
    {
        auto uiobj = std::make_shared<T>(args...);
        uiobj->OnInitializeFinish();
        return uiobj; // In case users forgets to post-initialize.
    }

    /*
    * A UI object is created (if use MakeUIObject) as standby state by default,
    * which means it is unable to be displayed or receive any application event.
    * To activate the UI object, it must be configured as one of the following:
    * 
    *   1. registered as a root object of the application.
    * 
    *   2. set as a child of another already activated UI object.
    * 
    * MakeRootUIObject corresponds to the 1st condition and MakeManagedUIObject the 2nd.
    */

    struct Window;

    template<typename T, typename... Types>
    SharedPtr<T> MakeRootUIObject(Types&& ...args)
    {
        auto uiobj = MakeUIObject<T>(args...);
        uiobj->RegisterDrawObjects();
        uiobj->RegisterApplicationEvents();
        return uiobj; // Help to create and register a root object.
    }

    struct Panel;

    template<typename T, typename... Types>
    SharedPtr<T> MakeManagedUIObject(ShrdPtrParam<Panel> parent, Types&& ...args)
    {
        auto uiobj = MakeUIObject<T>(args...);
        uiobj->SetParent(parent);
        return uiobj; // Help to reduce duplicated SetParent codes.
    }

    struct Panel : std::enable_shared_from_this<Panel>, IDrawObject2D, ISortable<Panel>
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

        virtual void OnInitializeFinish();

        bool IsHit(Event::Point& p);

        Function<bool(Panel*,Event::Point&)> f_isHit = {};

        // Always returns { MinimalWidth(), MinimalHeight() }, no virtual.
        D2D1_SIZE_F MinimalSize();
        D2D1_SIZE_F MaximalSize();

        Optional<float> minimalWidth = {};
        Optional<float> minimalHeight = {};
        Optional<float> maximalWidth = {};
        Optional<float> maximalHeight = {};

        // Return optional minimal-width and minimal-height (zero if empty) by default.
        // Use virtual so that derived class can force ignoring these config fields.
        virtual float MinimalWidth();
        virtual float MinimalHeight();
        virtual float MaximalWidth();
        virtual float MaximalHeight();

        virtual void RegisterDrawObjects();
        virtual void UnregisterDrawObjects();

        void RegisterApplicationEvents();
        void UnregisterApplicationEvents();

        // Application Event Callbacks

        void OnSize(SizeEvent& e);

        Function<void(Panel*,SizeEvent&)>
            f_onSizeOverride = {},
            f_onSizeBefore = {},
            f_onSizeAfter = {};

        void OnParentSize(SizeEvent& e);

        Function<void(Panel*,SizeEvent&)>
            f_onParentSizeOverride = {},
            f_onParentSizeBefore = {},
            f_onParentSizeAfter = {};

        void OnMove(MoveEvent& e);

        Function<void(Panel*,MoveEvent&)>
            f_onMoveOverride = {},
            f_onMoveBefore = {},
            f_onMoveAfter = {};

        void OnParentMove(MoveEvent& e);

        Function<void(Panel*,MoveEvent&)>
            f_onParentMoveOverride = {},
            f_onParentMoveBefore = {},
            f_onParentMoveAfter = {};

        void OnChangeTheme(WstrViewParam themeName);

        Function<void(Panel*,WstrViewParam)>
            f_onChangeThemeOverride = {},
            f_onChangeThemeBefore = {},
            f_onChangeThemeAfter = {};

        // Set this field to true if expect to receive get/lose focus event.
        bool isFocusable = false;

        // These event callbacks return a boolean value,
        // which means whether the UI object should be set/lose focus actually.

        bool OnGetFocus();

        Function<bool(Panel*)>
            f_onGetFocusOverride = {},
            f_onGetFocusBefore = {},
            f_onGetFocusAfter = {};

        bool OnLoseFocus();

        Function<bool()>
            f_onLoseFocusOverride = {},
            f_onLoseFocusBefore = {},
            f_onLoseFocusAfter = {};

        // These event callbacks return a boolean value,
        // which means whether to continue delivering the event to other UI objects.

        bool OnMouseButton(MouseButtonEvent& e);

        Function<bool(Panel*,MouseButtonEvent&)>
            f_onMouseButtonOverride = {},
            f_onMouseButtonBefore = {},
            f_onMouseButtonAfter = {};

        bool OnMouseEnter(MouseMoveEvent& e);

        Function<bool(Panel*,MouseMoveEvent&)>
            f_onMouseEnterOverride = {},
            f_onMouseEnterBefore = {},
            f_onMouseEnterAfter = {};

        bool OnMouseMove(MouseMoveEvent& e);

        Function<bool(Panel*,MouseMoveEvent&)>
            f_onMouseMoveOverride = {},
            f_onMouseMoveBefore = {},
            f_onMouseMoveAfter = {};

        bool OnMouseLeave(MouseMoveEvent& e);

        Function<bool(Panel*,MouseMoveEvent&)>
            f_onMouseLeaveOverride = {},
            f_onMouseLeaveBefore = {},
            f_onMouseLeaveAfter = {};

        bool OnMouseWheel(MouseWheelEvent& e);

        Function<bool(Panel*,MouseWheelEvent&)>
            f_onMouseWheelOverride = {},
            f_onMouseWheelBefore = {},
            f_onMouseWheelAfter = {};

        bool OnKeyboard(KeyboardEvent& e);

        Function<bool(Panel*,KeyboardEvent&)>
            f_onKeyboardOverride = {},
            f_onKeyboardBefore = {},
            f_onKeyboardAfter = {};

    protected:
        /*
        * Introduce OnXxxHelper to solve the inheritance conflicts of
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

        virtual bool IsHitHelper(Event::Point& p);

        virtual void OnSizeHelper(SizeEvent& e);
        virtual void OnParentSizeHelper(SizeEvent& e);
        virtual void OnMoveHelper(MoveEvent& e);
        virtual void OnParentMoveHelper(MoveEvent& e);
        virtual void OnChangeThemeHelper(WstrViewParam themeName);
        virtual bool OnGetFocusHelper();
        virtual bool OnLoseFocusHelper();
        virtual bool OnMouseButtonHelper(MouseButtonEvent& e);
        virtual bool OnMouseEnterHelper(MouseMoveEvent& e);
        virtual bool OnMouseMoveHelper(MouseMoveEvent& e);
        virtual bool OnMouseLeaveHelper(MouseMoveEvent& e);
        virtual bool OnMouseWheelHelper(MouseWheelEvent& e);
        virtual bool OnKeyboardHelper(KeyboardEvent& e);

        // Miscellaneous control flags for OnxxxHelper callbacks.

        // Set this field to true to prevent changing theme for children when OnChangeThemeHelper called.
        bool m_skipChangeChildrenTheme = false;

    public:
        // Define a series of detailed response control flags that work together with the "enable" flag.
        // We can use these flags to decide the UI object's application event sensitivity more flexibly;
        // for example, set "enable" as false and "mouse-button" as true to only receive mouse-button event.

        struct ApplicationEventSensitivity
        {
            bool hitTest = true;

            struct Mouse
            {
                bool button = true;
                bool enter = true;
                bool move = true;
                bool leave = true;
                bool wheel = true;
            }
            mouse = {};

            bool keyboard = true;
        }
        appEventFlags = {}; // Enable by default.

    protected:
        bool m_isVisible = true;

        // Note we should distinguish protected m_isEnabled from public appEventFlags:
        // the former is just a direct flag that switches each time SetEnabled called;
        // the latter controls whether the panel can actually receive application events.
        bool m_isEnabled = true;

        D2D1_RECT_F m_rect = {};
        
        // Cache this to reduce the computational overhead during rendering.
        D2D1_RECT_F m_absoluteRect = {};

    public:
        // This is the same as IsD2D1ObjectVisible for Panel since there're only D2D1 objects.
        // A derived class can control the visibility of D3D12 objects by overriding this method.
        virtual bool IsVisible();
        virtual void SetVisible(bool value);

        virtual bool IsEnabled();
        virtual void SetEnabled(bool value);

        D2D1_SIZE_F Size();
        float Width();
        float Height();

        D2D1_POINT_2F Position();
        D2D1_POINT_2F AbsolutePosition();

        const D2D1_RECT_F& AbsoluteRect();
        const D2D1_RECT_F& RelativeRect();
        D2D1_RECT_F SelfCoordRect();

        WeakPtr<Panel> Parent();
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

        void ForeachChild(const Function<void(ShrdPtrParam<Panel>)>& func);

        // Absolute --> Root Window, Relative --> Parent UI Object.
        // For self coordinate, left-top point is always (0,0).

        D2D1_POINT_2F AbsoluteToRelative(const D2D1_POINT_2F& p);
        D2D1_RECT_F AbsoluteToRelative(const D2D1_RECT_F& rect);
        D2D1_POINT_2F AbsoluteToSelfCoord(const D2D1_POINT_2F& p);
        D2D1_RECT_F AbsoluteToSelfCoord(const D2D1_RECT_F& rect);

        D2D1_POINT_2F RelativeToAbsolute(const D2D1_POINT_2F& p);
        D2D1_RECT_F RelativeToAbsolute(const D2D1_RECT_F& rect);
        D2D1_POINT_2F RelativeToSelfCoord(const D2D1_POINT_2F& p);
        D2D1_RECT_F RelativeToSelfCoord(const D2D1_RECT_F& rect);

        D2D1_POINT_2F SelfCoordToAbsolute(const D2D1_POINT_2F& p);
        D2D1_RECT_F SelfCoordToAbsolute(const D2D1_RECT_F& rect);
        D2D1_POINT_2F SelfCoordToRelative(const D2D1_POINT_2F& p);
        D2D1_RECT_F SelfCoordToRelative(const D2D1_RECT_F& rect);

        // All transform operations are based on relative coordinate.
        void Resize(const D2D1_SIZE_F& size);
        void Resize(float width, float height);
        void Move(const D2D1_POINT_2F& point);
        void Move(float left, float top);
        void Transform(const D2D1_RECT_F& rect);
        void Transform(float left, float top, float width, float height);

        void SetUIObjectPriority(int value);
        void SetD2D1ObjectPriority(int value);

        void UpdateAbsoluteRect();

    protected:
        struct TopmostWindowPriority
        {
            int d2d1Object = INT_MIN;
            int uiObject = INT_MAX;
        }
        m_topmostWindowPriority = {};

    public:
        void MoveChildWindowTopmost(Panel* w);

        void MoveTopmost();

    public:
        ComPtr<ID2D1Brush> brush = nullptr;

        float bitmapOpacity = 1.0f;
        ComPtr<ID2D1Bitmap1> bitmap = nullptr;

        float roundRadiusX = 0.0f, roundRadiusY = 0.0f;

    protected:
        void UpdateChildrenObjects(Renderer* rndr);

        void DrawChildrenLayers(Renderer* rndr);

        void DrawBackground(Renderer* rndr);

        // Set this field to true to prevent drawing children in Panel.
        bool m_takeOverChildrenDrawing = false;
        void DrawChildrenObjects(Renderer* rndr);

        virtual void OnRendererUpdateObject2DHelper(Renderer* rndr);
        virtual void OnRendererDrawD2D1LayerHelper(Renderer* rndr);
        virtual void OnRendererDrawD2D1ObjectHelper(Renderer* rndr);

    public:
        Function<void(Renderer*)>
            f_onRendererUpdateObject2DOverride = {},
            f_onRendererUpdateObject2DBefore = {},
            f_onRendererUpdateObject2DAfter = {};

        Function<void(Renderer*)>
            f_onRendererDrawD2D1LayerOverride = {},
            f_onRendererDrawD2D1LayerBefore = {},
            f_onRendererDrawD2D1LayerAfter = {};

        Function<void(Renderer*)>
            f_onRendererDrawD2D1ObjectOverride = {},
            f_onRendererDrawD2D1ObjectBefore = {},
            f_onRendererDrawD2D1ObjectAfter = {};

    protected:
        Renderer::DrawObject2DSet m_drawObjects2D = {};

        // Don't use SharedPtr here since it will cause circular reference.
        WeakPtr<Panel> m_parent = {};

        using ChildObjectSet = std::unordered_set<SharedPtr<Panel>>;

        ChildObjectSet m_children = {};

        using ChildObjectPrioritySet = ISortable<Panel>::WeakPrioritySet;

        ChildObjectPrioritySet m_hitChildren = {};

        // Pinned children keep receiving UI events even though they are not hit.
        ChildObjectPrioritySet m_pinnedChildren = {}, m_diffPinnedChildren = {};
    };
}