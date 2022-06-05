#include "Precompile.h"

#include "UI/StrokeStyle.h"

namespace d14engine::ui
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