#pragma once

#include "Common/Precompile.h"

#include "UIKit/Layout.h"

namespace d14engine::uikit
{
    struct GridLayoutGeometryInfo
    {
        bool isFixedSize = false;

        struct Axis
        {
            struct Data
            {
                size_t offset = 0, count = 1;
            }
            x = {}, y = {};
        }
        axis = {};

        D2D1_RECT_F spacing = { 0.0f, 0.0f, 0.0f, 0.0f };
    };

    struct GridLayout : Layout<GridLayoutGeometryInfo>
    {
        GridLayout(
            const D2D1_RECT_F& rect,
            size_t horzCellCount = 1,
            size_t vertCellCount = 1,
            float horzSpacing = 8.0f,
            float vertSpacing = 8.0f);

    protected:
        size_t m_horzCellCount = {}, m_vertCellCount = {};

        float m_horzSpacing = {}, m_vertSpacing = {};

        void UpdateElement(ElementGeometryInfoMap::iterator& elemItor) override;

    public:
        size_t HorzCellCount();
        size_t VertCellCount();

        void SetCellCount(size_t horz, size_t vert);

        float HorzSpacing();
        float VertSpacing();

        void SetSpacing(float horz, float vert);
    };
}