#include "Common/Precompile.h"

#include "UIKit/ConstraintLayout.h"

namespace d14engine::uikit
{
    ConstraintLayout::ConstraintLayout(const D2D1_RECT_F& rect)
        :
        Panel(rect, Resu::SOLID_COLOR_BRUSH),
        Layout(rect) { }

    void ConstraintLayout::UpdateElement(ElementGeometryInfoMap::iterator& elemItor)
    {
        if (!elemItor->first.expired())
        {
            auto pElem = elemItor->first.lock();
            auto& geoInfo = elemItor->second;

            D2D1_RECT_F rect = pElem->RelativeRect();
            float width = pElem->Width();
            float height = pElem->Height();

            // Left <-------- Left --------> Right
            if (geoInfo.Left.ToLeft.has_value())
            {
                rect.left = geoInfo.Left.ToLeft.value();

                if (geoInfo.keepWidth)
                {
                    rect.right = rect.left + width;
                }
            }
            else if (geoInfo.Left.ToRight.has_value())
            {
                rect.left = Width() - geoInfo.Left.ToRight.value();

                if (geoInfo.keepWidth)
                {
                    rect.right = rect.left + width;
                }
            }
            // Left <-------- Right --------> Right
            if (geoInfo.Right.ToLeft.has_value())
            {
                rect.right = geoInfo.Right.ToLeft.value();

                if (geoInfo.keepWidth)
                {
                    rect.left = rect.right - width;
                }
            }
            else if (geoInfo.Right.ToRight.has_value())
            {
                rect.right = Width() - geoInfo.Right.ToRight.value();

                if (geoInfo.keepWidth)
                {
                    rect.left = rect.right - width;
                }
            }
            // Top <-------- Top --------> Bottom
            if (geoInfo.Top.ToTop.has_value())
            {
                rect.top = geoInfo.Top.ToTop.value();

                if (geoInfo.keepHeight)
                {
                    rect.bottom = rect.top + height;
                }
            }
            else if (geoInfo.Top.ToBottom.has_value())
            {
                rect.top = Height() - geoInfo.Top.ToBottom.value();

                if (geoInfo.keepHeight)
                {
                    rect.bottom = rect.top + height;
                }
            }
            // Top <-------- Bottom --------> Bottom
            if (geoInfo.Bottom.ToTop.has_value())
            {
                rect.bottom = geoInfo.Bottom.ToTop.value();

                if (geoInfo.keepHeight)
                {
                    rect.top = rect.bottom - height;
                }
            }
            else if (geoInfo.Bottom.ToBottom.has_value())
            {
                rect.bottom = Height() - geoInfo.Bottom.ToBottom.value();

                if (geoInfo.keepHeight)
                {
                    rect.top = rect.bottom - height;
                }
            }
            pElem->Transform(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
        }
    }
}