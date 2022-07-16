#pragma once

#include "Common/Precompile.h"

#include "UIKit/ClickablePanel.h"
#include "UIKit/MaskStyle.h"
#include "UIKit/SolidStyle.h"
#include "UIKit/StatefulObject.h"
#include "UIKit/StrokeStyle.h"

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

    struct OnOffSwitch : ClickablePanel, StatefulObject<OnOffSwitchState, OnOffSwitchStateChangeEvent>
    {
        OnOffSwitch(
            const D2D1_RECT_F& rect = { 0.0f, 0.0f, 50.0f, 26.0f },
            float roundRadius = 13.0f);

        MaskStyle mask = { 0, 0 };

        float handleVariableSpeedSecs = 0.05f;
        float handleUniformSpeedSecs = 0.05f;

    public:
        struct Appearance
        {
            SolidStyle background = {};

            StrokeStyle stroke = {};

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

    protected:
        // Override interface methods.

        // IDrawObject2D
        void OnRendererUpdateObject2DHelper(Renderer* rndr) override;

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