#include "Common/Precompile.h"

#include "UIKit/StrokeStyle.h"

namespace d14engine::uikit
{
    StrokeStyle::StrokeStyle(
        float width,
        D2D1_COLOR_F color,
        float opacity)
        :
        width(width),
        color(color),
        opacity(opacity) { }
}