#pragma once

#include "Precompile.h"

#include "Panel.h"

namespace d14engine::ui
{
    struct OnOffSwitch : Panel
    {
        OnOffSwitch(
            const D2D1_RECT_F& rect = { 0.0f, 0.0f, 50.0f, 26.0f },
            float roundRadius = 13.0f);

        float handleVariableSpeedSecs = 0.05f;
        float handleUniformSpeedSecs = 0.05f;

    public:
        struct State
        {
            // Note ActiveFlag::Finished is not included in total number
            // since only target-state will use it temporarily in Update.
            enum class ActiveFlag { On, Off, Count, Finished } activeFlag;
            enum class ButtonFlag { Idle, Hover, Down, Count } buttonFlag;

            size_t Index() { return (size_t)buttonFlag + (size_t)activeFlag * (size_t)ButtonFlag::Count; }

            enum class Flag { OnIdle, OnHover, OnDown, OffIdle, OffHover, OffDown, Count };
        };

        struct Appearance
        {
            D2D1_COLOR_F backgroundColor = (D2D1::ColorF)D2D1::ColorF::White;
            float backgroundOpacity = 1.0f;

            D2D1_COLOR_F strokeColor = (D2D1::ColorF)D2D1::ColorF::Black;
            float strokeOpacity = 1.0f;
            float strokeWidth = 0.0f;

            struct Handle
            {
                D2D1_COLOR_F color = (D2D1::ColorF)D2D1::ColorF::Black;
                float opacity = 1.0f;

                D2D1_SIZE_F size = { 0.0f, 0.0f };
                float roundRadius = 0.0f;

                Optional<float> leftOffset = std::nullopt;
                Optional<float> rightOffset = std::nullopt;

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
        appearances[(size_t)State::Flag::Count];

    protected:
        State m_state = { State::ActiveFlag::Off, State::ButtonFlag::Idle };

        State::ActiveFlag m_targetActiveState = State::ActiveFlag::Finished;

        float m_currHandleDisplacement = 0.0f;
        float m_currHandleOffsetToLeft = 0.0f;

        bool m_hasLeftPressed = false; // See Button.h for detailed explanation.

    public:
        struct Event : ui::Event
        {
            State::ActiveFlag flag = State::ActiveFlag::Finished;

            bool SwitchOn() { return flag == State::ActiveFlag::On; }
            bool SwitchOff() { return flag == State::ActiveFlag::Off; }
        };

        void OnStateChange(Event& e);

        Function<void(OnOffSwitch*,Event&)>
            f_onStateChangeOverride = {},
            f_onStateChangeBefore = {},
            f_onStateChangeAfter = {};

    protected:
        virtual void OnStateChangeHelper(Event& e);

    public:
        // Override interface methods.

        // IDrawObject2D
        void OnRendererUpdateObject2DHelper(Renderer* rndr) override;

        void OnRendererDrawD2D1ObjectHelper(Renderer* rndr) override;

        // Panel
        void OnChangeThemeHelper(WstrViewParam themeName) override;

        bool OnMouseButtonHelper(MouseButtonEvent& e) override;

        bool OnMouseEnterHelper(MouseEnterEvent& e) override;

        bool OnMouseLeaveHelper(MouseLeaveEvent& e) override;
    };
}