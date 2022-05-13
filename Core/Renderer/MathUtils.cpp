#include "Precompile.h"

#include "Renderer/MathUtils.h"

namespace d14engine::renderer
{
    XMFLOAT4X4 MathUtils::IdentityFloat4x4()
    {
        return
        {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
    }

    bool MathUtils::IsInside(const D2D1_POINT_2F& p, const D2D1_RECT_F& r)
    {
        return p.x > r.left && p.x < r.right&& p.y > r.top && p.y < r.bottom;
    }

    bool MathUtils::IsOverlapped(const D2D1_POINT_2F& p, const D2D1_RECT_F& r)
    {
        return p.x >= r.left && p.x <= r.right && p.y >= r.top && p.y <= r.bottom;
    }

    D2D1_RECT_F MathUtils::Offset(const D2D1_RECT_F& rect, float x, float y)
    {
        return { rect.left + x, rect.top + y, rect.right + x, rect.bottom + y };
    }

    D2D1_RECT_F MathUtils::Offset(const D2D1_RECT_F& rect, const D2D1_POINT_2F& offset)
    {
        return { rect.left + offset.x, rect.top + offset.y, rect.right + offset.x, rect.bottom + offset.y };
    }

    D2D1_POINT_2F MathUtils::Offset(const D2D1_POINT_2F& point, float x, float y)
    {
        return { point.x + x, point.y + y };
    }

    D2D1_POINT_2F MathUtils::Offset(const D2D1_POINT_2F& point, const D2D1_POINT_2F& offset)
    {
        return { point.x + offset.x, point.y + offset.y };
    }
}