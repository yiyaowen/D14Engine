#include "Precompile.h"

#include "UI/SolidStyle.h"

namespace d14engine::ui
{
    SolidStyle::SolidStyle(
        D2D1_COLOR_F backgroundColor,
        float backgroundOpacity)
        :
        backgroundColor(backgroundColor),
        backgroundOpacity(backgroundOpacity) { }
}