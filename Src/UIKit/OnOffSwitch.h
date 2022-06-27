#pragma once

#include "Common/Precompile.h"

#include "UIKit/Panel.h"
#include "UIKit/StatefulObject.h"

namespace d14engine::uikit
{
    struct OnOffSwitchState
    {
        // Note ActiveFlag::Finished is not included in total number
        // since only target-state will use it temporarily in Update.
        enum class ActiveFlag { On, Off, Count, Finished } activeFlag = {};
        enum class ButtonFlag { Idle, Hover, Down, Count } buttonFlag = {};

        size_t Index() { return (size_t)buttonFlag + (size_t)activeFlag * (size_t)ButtonFlag::Count; }

        enum class Flag { OnIdle, OnHover, OnDown, OffIdle, OffHover, OffDown, Count };
    };

    struct OnOffSwitchStateChangeEvent : uikit::Event
    {
        OnOffSwitchState::ActiveFlag flag = {};

        bool SwitchOn() { return flag == OnOffSwitchState::ActiveFlag::On; }
        bool SwitchOff() { return flag == OnOffSwitchState::ActiveFlag::Off; }
    };

    struct OnOffSwitch : Panel, StatefulObject<OnOffSwitchState, OnOffSwitchStateChangeEvent>
    {
        OnOffSwitch(
            const D2D1_RECT_F& rect = { 0.0f, 0.0f, 50.0f, 26.0f },
            float roundRadius = 13.0f);

        float handleVariableSpeedSecs = 0.05f;
        float handleUniformSpeedSecs = 0.05f;

    public:
        struct Appearance
        {
            D2D1_COLOR_F backgroundColor = {};
            float backgroundOpacity = {};

            D2D1_COLOR_F strokeColor = {};
            float strokeOpacity = {};
            float strokeWidth = {};

            struct Handle
            {
                D2D1_COLOR_F color = {};
                float opacity = {};

                D2D1_SIZE_F size = {};
                float roundRadius = {};

                Optional<float> leftOffset = {};
                Optional<float> rightOffset = {};

                float DisplacementToLeft(float parentWidth)
                {
                    if (leftOffset.has_value())
                    {
                        return leftOffset.value();
                    }
                    else if (rightOffset.has_value())
                    {
                        return parentWidth - rightOffset.value();
                    }
                    else return 0.0f; // At left-most by default.
                }
            }
            handle = {};
        }
        appearances[(size_t)State::Flag::Count] = {};

    protected:
        State::ActiveFlag m_targetActiveState = State::ActiveFlag::Finished;

        float m_currHandleDisplacement = 0.0f;
        float m_currHandleOffsetToLeft = 0.0f;

        bool m_hasLeftPressed = false; // See Button.h for detailed explanation.

    public:
        // Override interface methods.

        // IDrawObject2D
        void OnRendererUpdateObject2DHelper(Renderer* rndr) override;

        void OnRendererDrawD2D1ObjectHelper(Renderer* rndr) override;

        // Panel
        void OnChangeThemeHelper(WstrViewParam themeName) override;

        bool OnMouseButtonHelper(MouseButtonEvent& e) override;

        bool OnMouseEnterHelper(MouseMoveEvent& e) override;

        bool OnMouseLeaveHelper(MouseMoveEvent& e) override;
    };
}