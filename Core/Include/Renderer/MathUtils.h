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

        static D2D1_RECT_F Offset(const D2D1_RECT_F& rect, float x, float y);

        static D2D1_RECT_F Offset(const D2D1_RECT_F& rect, const D2D1_POINT_2F& offset);

        static D2D1_POINT_2F Offset(const D2D1_POINT_2F& point, float x, float y);

        static D2D1_POINT_2F Offset(const D2D1_POINT_2F& point, const D2D1_POINT_2F& offset);
    };
    // Introduce this alias to avoid too long prefix.
    using Mathu = MathUtils;
}