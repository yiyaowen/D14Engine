#include "Precompile.h"

#include "UI/GridLayout.h"

#include "Renderer/MathUtils.h"
#include "UI/UIResourceUtils.h"

namespace d14engine::ui
{
    GridLayout::GridLayout(
        const D2D1_RECT_F& rect,
        size_t horzCellCount,
        size_t vertCellCount,
        float horzSpacing,
        float vertSpacing)
        :
        Layout(rect),
        m_horzCellCount(horzCellCount),
        m_vertCellCount(vertCellCount),
        m_horzSpacing(horzSpacing),
        m_vertSpacing(vertSpacing) { }

    void GridLayout::UpdateElement(ElementGeometryInfoMap::iterator& elemItor)
    {
        if (!elemItor->first.expired())
        {
            auto pElem = elemItor->first.lock();
            auto& geoInfo = elemItor->second;

            float deltaX = Width() / m_horzCellCount;
            float deltaY = Height() / m_vertCellCount;

            if (geoInfo.isFixedSize)
            {
                float occupiedWidth = geoInfo.axis.x.count * deltaX;
                float occupiedHeight = geoInfo.axis.y.count * deltaY;

                float leftOffset = (occupiedWidth - pElem->Width()) * 0.5f;
                float topOffset = (occupiedHeight - pElem->Height()) * 0.5f;

                pElem->Move(
                    geoInfo.axis.x.offset * deltaX + leftOffset,
                    geoInfo.axis.y.offset * deltaY + topOffset);
            }
            else // Move and Resize.
            {
                float leftOffset = m_horzSpacing + geoInfo.spacing.left;
                float topOffset = m_vertSpacing + geoInfo.spacing.top;

                float horzSpacing = 2.0f * (m_horzSpacing + geoInfo.spacing.left + geoInfo.spacing.right);
                float vertSpacing = 2.0f * (m_vertSpacing + geoInfo.spacing.top + geoInfo.spacing.bottom);

                pElem->Transform(
                    geoInfo.axis.x.offset * deltaX + leftOffset,
                    geoInfo.axis.y.offset * deltaY + topOffset,
                    geoInfo.axis.x.count * deltaX - horzSpacing,
                    geoInfo.axis.y.count * deltaY - vertSpacing);
            }
        }
    }

    size_t GridLayout::HorzCellCount()
    {
        return m_horzCellCount;
    }

    size_t GridLayout::VertCellCount()
    {
        return m_vertCellCount;
    }

    void GridLayout::SetCellCount(size_t horz, size_t vert)
    {
        m_horzCellCount = horz;
        m_vertCellCount = vert;

        UpdateAllElements();
    }

    float GridLayout::HorzSpacing()
    {
        return m_horzSpacing;
    }

    float GridLayout::VertSpacing()
    {
        return m_vertSpacing;
    }

    void GridLayout::SetSpacing(float horz, float vert)
    {
        m_horzSpacing = horz;
        m_vertSpacing = vert;
        
        UpdateAllElements();
    }
}