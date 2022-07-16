#include "Common/Precompile.h"

#include "UIKit/SolidStyle.h"

namespace d14engine::uikit
{
    SolidStyle::SolidStyle(
        D2D1_COLOR_F color,
        float opacity)
        :
        color(color),
        opacity(opacity) { }
}