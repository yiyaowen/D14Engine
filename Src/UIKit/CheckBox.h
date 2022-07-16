#pragma once

#include "Common/Precompile.h"

#include "UIKit/ClickablePanel.h"
#include "UIKit/MaskStyle.h"
#include "UIKit/SolidStyle.h"
#include "UIKit/StatefulObject.h"
#include "UIKit/StrokeStyle.h"

namespace d14engine::uikit
{
    struct CheckBoxState
    {
        enum class ActiveFlag { Unchecked, Intermidiate, Checked, Count } activeFlag = {};
        enum class ButtonFlag { Idle, Hover, Down, Count } buttonFlag = {};

        size_t Index() { return (size_t)buttonFlag + (size_t)activeFlag * (size_t)ButtonFlag::Count; }

        enum class Flag // List again to match with different appearance schemes.
        {
            UncheckedIdle, UncheckedHover, UncheckedDown,
            IntermidiateIdle, IntermidiateHover, IntermidiateDown,
            CheckedIdle, CheckedHover, CheckedDown, Count
        };
    };

    struct CheckBoxStateChangeEvent : uikit::Event
    {
        CheckBoxState::ActiveFlag flag = {};

        bool Unchecked() { return flag == CheckBoxState::ActiveFlag::Unchecked; }
        bool Intermidiate() { return flag == CheckBoxState::ActiveFlag::Intermidiate; }
        bool Checked() { return flag == CheckBoxState::ActiveFlag::Checked; }
    };

    struct CheckBox : ClickablePanel, StatefulObject<CheckBoxState, CheckBoxStateChangeEvent>
    {
        CheckBox(
            bool isTripleState = false,
            const D2D1_RECT_F& rect = { 0.0f, 0.0f, 20.0f, 20.0f },
            float roundRadius = 4.0f);

        MaskStyle mask = { 0, 0 };

    public:
        struct Appearance
        {
            ComPtr<ID2D1Bitmap1> bitmap = {};
            float bitmapOpacity = {};

            SolidStyle foreground = {};
            SolidStyle background = {};

            StrokeStyle stroke = {};
        }
        appearances[(size_t)State::Flag::Count] = {};

        struct IconStyle
        {
            struct Intermidiate
            {
                D2D1_SIZE_F size = { 12.0f, 3.5f };
            }
            intermidiate = {};

            struct Checked
            {
                struct Line
                {
                    D2D1_POINT_2F point0 = {}, point1 = {};
                }
                tickLine0 = { { 2.5f, 9.5f }, { 8.5f, 15.5f } },
                tickLine1 = { { 6.5, 15.5f }, { 17.5f, 5.5f } };

                float width = 3.5f;
            }
            checked = {};
        }
        iconStyle = {};

    protected:
        bool m_isTripleState = {};

        using StateTransitionMap = std::unordered_map<
            State::ActiveFlag, // source state
            State::ActiveFlag, // destination state
            decltype([](State::ActiveFlag flag) { return (size_t)flag; })>;

        // This container stores <Source State, Destination State> as <key, value>.
        // When clicked, current state will be changed to related destination state.
        StateTransitionMap m_stateTransitionMap = {};

    public:
        bool IsTripleState();
        void EnableTripleState(bool value);

    protected:
        void DrawIntermidiateIcon(Renderer* rndr);
        void DrawCheckedIcon(Renderer* rndr);

    protected:
        // Override interface methods.

        // IDrawObject2D
        void OnRendererDrawD2D1LayerHelper(Renderer* rndr) override;

        void OnRendererDrawD2D1ObjectHelper(Renderer* rndr) override;

        // Panel
        void OnSizeHelper(SizeEvent& e) override;

        void OnChangeThemeHelper(WstrViewParam themeName) override;

        bool OnMouseEnterHelper(MouseMoveEvent& e) override;

        bool OnMouseLeaveHelper(MouseMoveEvent& e) override;

    public:
        void SetEnabled(bool value) override;

    protected:
        // ClickablePanel
        void OnMouseButtonPressHelper(ClickablePanel::Event& e) override;

        void OnMouseButtonReleaseHelper(ClickablePanel::Event& e) override;
    };
}