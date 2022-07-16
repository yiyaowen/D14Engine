#pragma once

#include "Common/Precompile.h"

#include "UIKit/FilledButton.h"
#include "UIKit/StatefulObject.h"

namespace d14engine::uikit
{
    struct ToggleButtonState
    {
        enum class ActiveFlag { Inactivated, Activated, Count } activeFlag = {};
        enum class ButtonFlag { Idle, Hover, Down, Count } buttonFlag = {};

        size_t Index() { return (size_t)buttonFlag + (size_t)activeFlag * (size_t)ButtonFlag::Count; }

        enum class Flag // List again to match with different appearance schemes.
        {
            InactivatedIdle, InactivatedHover, InactivatedDown,
            ActivatedIdle, ActivatedHover, ActivatedDown, Count
        };
    };

    struct ToggleButtonStateChangeEvent : uikit::Event
    {
        ToggleButtonState::ActiveFlag flag = {};

        bool Activated() { return flag == ToggleButtonState::ActiveFlag::Activated; }
        bool Inactivated() { return flag == ToggleButtonState::ActiveFlag::Inactivated; }
    };

    struct ToggleButton : FilledButton, StatefulObject<ToggleButtonState, ToggleButtonStateChangeEvent>
    {
        ToggleButton(
            WstrParam text,
            const D2D1_RECT_F& rect,
            float roundRadius = 0.0f,
            ComPtrParam<ID2D1Bitmap1> icon = nullptr);

    public:
        Appearance activatedAppearances[(size_t)Button::State::Count];

    protected:
        // Override interface methods.

        // Panel
        void OnChangeThemeHelper(WstrViewParam themeName) override;

        // ClickablePanel
        void OnMouseButtonReleaseHelper(Button::Event& e) override;

        // Button
        void UpdateAppearanceSetting(Button::State state) override;
    };
}