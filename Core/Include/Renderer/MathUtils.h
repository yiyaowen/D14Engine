#pragma once

#include "Precompile.h"

namespace d14engine::renderer
{
    struct MathUtils
    {
        // 3D

        static XMFLOAT4X4 IdentityFloat4x4();

        // 2D

        static bool IsInside(const D2D1_POINT_2F& p, const D2D1_RECT_F& r);
        static bool IsOverlapped(const D2D1_POINT_2F& p, const D2D1_RECT_F& r);

        static D2D1_RECT_F Offset(const D2D1_RECT_F& rect, const D2D1_POINT_2F& offset);
        static D2D1_POINT_2F Offset(const D2D1_POINT_2F& point, const D2D1_POINT_2F& offset);

        static D2D1_POINT_2F OverrideX(const D2D1_POINT_2F& point, float value);
        static D2D1_POINT_2F OverrideY(const D2D1_POINT_2F& point, float value);

        static D2D1_POINT_2F IncreaseX(const D2D1_POINT_2F& point, float value);
        static D2D1_POINT_2F IncreaseY(const D2D1_POINT_2F& point, float value);

        // Pass through positive delta to expand and negative delta to shrink the rectangle.
        static D2D1_RECT_F Stretch(const D2D1_RECT_F& rect, const D2D1_POINT_2F& delta);

        static D2D1_RECT_F OverrideLeft(const D2D1_RECT_F& rect, float value);
        static D2D1_RECT_F OverrideTop(const D2D1_RECT_F& rect, float value);
        static D2D1_RECT_F OverrideRight(const D2D1_RECT_F& rect, float value);
        static D2D1_RECT_F OverrideBottom(const D2D1_RECT_F& rect, float value);

        static D2D1_RECT_F IncreaseLeft(const D2D1_RECT_F& rect, float value);
        static D2D1_RECT_F IncreaseTop(const D2D1_RECT_F& rect, float value);
        static D2D1_RECT_F IncreaseRight(const D2D1_RECT_F& rect, float value);
        static D2D1_RECT_F IncreaseBottom(const D2D1_RECT_F& rect, float value);

        static D2D1_RECT_F MoveVertex(const D2D1_RECT_F& rect, const D2D1_RECT_F& delta);
    };
    // Introduce this alias to avoid too long prefix.
    using Mathu = MathUtils;
}