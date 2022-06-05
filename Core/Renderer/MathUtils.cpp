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

    D2D1_RECT_F MathUtils::Offset(const D2D1_RECT_F& rect, const D2D1_POINT_2F& offset)
    {
        return { rect.left + offset.x, rect.top + offset.y, rect.right + offset.x, rect.bottom + offset.y };
    }

    D2D1_POINT_2F MathUtils::Offset(const D2D1_POINT_2F& point, const D2D1_POINT_2F& offset)
    {
        return { point.x + offset.x, point.y + offset.y };
    }

    D2D1_RECT_F MathUtils::Stretch(const D2D1_RECT_F& rect, const D2D1_POINT_2F& delta)
    {
        return { rect.left - delta.x, rect.top - delta.y, rect.right + delta.x, rect.bottom + delta.y };
    }

    D2D1_RECT_F MathUtils::OverrideLeft(const D2D1_RECT_F& rect, float value)
    {
        return { value, rect.top, rect.right, rect.bottom };
    }

    D2D1_RECT_F MathUtils::OverrideTop(const D2D1_RECT_F& rect, float value)
    {
        return { rect.left, value, rect.right, rect.bottom };
    }

    D2D1_RECT_F MathUtils::OverrideRight(const D2D1_RECT_F& rect, float value)
    {
        return { rect.left, rect.top, value, rect.bottom };
    }

    D2D1_RECT_F MathUtils::OverrideBottom(const D2D1_RECT_F& rect, float value)
    {
        return { rect.left, rect.top, rect.right, value };
    }

    D2D1_RECT_F MathUtils::IncreaseLeft(const D2D1_RECT_F& rect, float value)
    {
        return { rect.left + value, rect.top, rect.right, rect.bottom };
    }

    D2D1_RECT_F MathUtils::IncreaseTop(const D2D1_RECT_F& rect, float value)
    {
        return { rect.left, rect.top + value, rect.right, rect.bottom };
    }

    D2D1_RECT_F MathUtils::IncreaseRight(const D2D1_RECT_F& rect, float value)
    {
        return { rect.left, rect.top, rect.right + value, rect.bottom };
    }

    D2D1_RECT_F MathUtils::IncreaseBottom(const D2D1_RECT_F& rect, float value)
    {
        return { rect.left, rect.top, rect.right, rect.bottom + value };
    }

    D2D1_RECT_F MathUtils::MoveVertex(const D2D1_RECT_F& rect, const D2D1_RECT_F& delta)
    {
        return { rect.left + delta.left, rect.top + delta.top, rect.right + delta.right, rect.bottom + delta.bottom };
    }
}