#include "Precompile.h"

#include "UI/SolidColorStyle.h"

namespace d14engine::ui
{
    SolidColorStyle::SolidColorStyle(
        D2D1_COLOR_F solidColor,
        float solidColorOpaque)
        :
        solidColor(solidColor),
        solidColorOpaque(solidColorOpaque) { }
}