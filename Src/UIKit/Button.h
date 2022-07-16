#pragma once

#include "Common/Precompile.h"

#include "UIKit/ClickablePanel.h"
#include "UIKit/Label.h"
#include "UIKit/MaskStyle.h"
#include "UIKit/SolidStyle.h"
#include "UIKit/StrokeStyle.h"

namespace d14engine::uikit
{
    struct Button : ClickablePanel
    {
        Button(
            WstrParam text,
            const D2D1_RECT_F& rect,
            float roundRadius = 0.0f,
            ComPtrParam<ID2D1Bitmap1> icon = nullptr);

        enum class State { Idle, Hover, Down, Count };

        SharedPtr<Label> textLabel = {};

        // This field decides the rectangle of the icon bitmap in self-coordinate.
        D2D1_RECT_F iconRect = {};

        void OnInitializeFinish() override;

        MaskStyle mask = { 0, 0 };
        SolidStyle background = {};
        StrokeStyle stroke = {};

    protected:
        virtual void UpdateAppearanceSetting(State state);

    protected:
        // Override interface methods.

        // IDrawObject2D
        void OnRendererDrawD2D1LayerHelper(Renderer* rndr) override;

        void OnRendererDrawD2D1ObjectHelper(Renderer* rndr) override;

    public:
        // Panel
        bool IsHit(uikit::Event::Point& p) override;

    protected:
        void OnSizeHelper(SizeEvent& e) override;

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