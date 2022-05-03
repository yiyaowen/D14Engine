#pragma once

#include "Precompile.h"

namespace d14engine::renderer
{
    struct MathUtils
    {
        // 3D

        static XMFLOAT4X4 IdentityFloat4x4()
        {
            return
            {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            };
        }

        // 2D

        static bool IsInside(const D2D1_POINT_2F& p, const D2D1_RECT_F& r)
        {
            return p.x > r.left && p.x < r.right && p.y > r.top && p.y < r.bottom;
        }

        static bool IsOverlapped(const D2D1_POINT_2F& p, const D2D1_RECT_F& r)
        {
            return p.x >= r.left && p.x <= r.right&& p.y >= r.top && p.y <= r.bottom;
        }

        static D2D1_RECT_F Offset(const D2D1_RECT_F& rect, float x, float y)
        {
            return { rect.left + x, rect.top + y, rect.right + x, rect.bottom + y };
        }

        static D2D1_RECT_F Offset(const D2D1_RECT_F& rect, const D2D1_POINT_2F& offset)
        {
            return { rect.left + offset.x, rect.top + offset.y, rect.right + offset.x, rect.bottom + offset.y };
        }

        static D2D1_POINT_2F Offset(const D2D1_POINT_2F& point, float x, float y)
        {
            return { point.x + x, point.y + y };
        }

        static D2D1_POINT_2F Offset(const D2D1_POINT_2F& point, const D2D1_POINT_2F& offset)
        {
            return { point.x + offset.x, point.y + offset.y };
        }
    };
    // Introduce this alias to avoid too long prefix.
    using Mathu = MathUtils;
}