#pragma once

#include "Common/Precompile.h"

#include "UIKit/Layout.h"

namespace d14engine::uikit
{
    struct ConstraintLayoutGeometryInfo
    {
        struct HorzDistance
        {
            Optional<float> ToLeft = std::nullopt, ToRight = std::nullopt;
        };
        struct VertDistance
        {
            Optional<float> ToTop = std::nullopt, ToBottom = std::nullopt;
        };
        // Start with captial to match XxxxToXxxx style.
        HorzDistance Left = {}, Right = {};
        VertDistance Top = {}, Bottom = {};

        bool keepWidth = true, keepHeight = true;
    };

    struct ConstraintLayout : Layout<ConstraintLayoutGeometryInfo>
    {
        explicit ConstraintLayout(const D2D1_RECT_F& rect);

        using Layout::UpdateElement;

    protected:
        void UpdateElement(ElementGeometryInfoMap::iterator& elemItor) override;
    };
}