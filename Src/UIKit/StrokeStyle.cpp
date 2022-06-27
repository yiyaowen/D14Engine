#include "Common/Precompile.h"

#include "UIKit/StrokeStyle.h"

namespace d14engine::uikit
{
    StrokeStyle::StrokeStyle(
        float strokeWidth,
        D2D1_COLOR_F strokeColor,
        float strokeOpacity)
        :
        strokeWidth(strokeWidth),
        strokeColor(strokeColor),
        strokeOpacity(strokeOpacity) { }
}