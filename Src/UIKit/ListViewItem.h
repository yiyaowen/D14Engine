#pragma once

#include "Common/Precompile.h"

#include "Common/CppLangUtils.h"

#include "UIKit/ClickablePanel.h"
#include "UIKit/MaskStyle.h"
#include "UIKit/SolidStyle.h"
#include "UIKit/StrokeStyle.h"

namespace d14engine::uikit
{
    struct ListViewItem : ClickablePanel
    {
        ListViewItem(const D2D1_RECT_F& rect, ShrdPtrParam<Panel> content = nullptr);

        void OnInitializeFinish() override;

        MaskStyle contentMask = { 0, 0 };

    public:
        SharedPtr<Panel> Content();
        void SetContent(ShrdPtrParam<Panel> content);

        enum class State
        {
            Idle, Hover, ActiveSelected, InactiveSelected, ActiveSelectedHover, Count
        }
        state = {};

        using StateTransitionMap = std::unordered_map<
            State, // before
            State, // after
            EnumClassHash<State>>;

        const static StateTransitionMap
            ENTER_STATE_TRANS_MAP,
            LEAVE_STATE_TRANS_MAP,
            CHECK_STATE_TRANS_MAP,
            UNCHECK_STATE_TRANS_MAP,
            GET_FOCUS_STATE_TRANS_MAP,
            LOSE_FOCUS_STATE_TRANS_MAP;

        struct Appearance
        {
            ComPtr<ID2D1Bitmap1> bitmap = {};
            float bitmapOpacity = {};

            SolidStyle background = {};

            StrokeStyle stroke = {};
        }
        appearances[(size_t)State::Count];

    protected:
        SharedPtr<Panel> m_content = {};

    protected:
        // Override interface methods.

        // IDrawObject2D
        void OnRendererDrawD2D1LayerHelper(Renderer* rndr) override;

        void OnRendererDrawD2D1ObjectHelper(Renderer* rndr) override;

        // Panel
        void OnSizeHelper(SizeEvent& e) override;

        void OnChangeThemeHelper(WstrViewParam themeName) override;
    };
}