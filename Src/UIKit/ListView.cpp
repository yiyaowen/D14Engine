#include "Common/Precompile.h"

#include "UIKit/ListView.h"

namespace d14engine::uikit
{
    ListView::ListView(const D2D1_RECT_F& rect)
        :
        Panel(rect, Resu::SOLID_COLOR_BRUSH),
        ScrollView(rect)
    {
        // We must place SetContent in OnInitializeFinish instead in ctor
        // since its implementation calls shared_from_this internally,
        // which can cause undefined result if object is initializing.
    }

    void ListView::OnInitializeFinish()
    {
        ScrollView::OnInitializeFinish();

        SetContent(m_layout = MakeUIObject<ConstraintLayout>(SelfCoordRect()));

        m_layout->background.opacity = 0.0f;
        m_layout->stroke.opacity = 0.0f;

        // Each item should have the same width with view.
        m_layout->f_onParentSizeAfter = [&](Panel* p, SizeEvent& e)
        {
            p->Resize(e.size.width, p->Height());
        };
    }

    WeakPtr<Panel> ListView::FindItem(size_t index)
    {
        return (index >= 0 && index < m_itemArray.size()) ? m_itemArray[index] : WeakPtr<Panel>{};
    }

    void ListView::InsertItem(ShrdPtrParam<Panel> item, size_t index)
    {
        // Note "index == m_itemArray.size()" should be taken into account.
        if (index >= 0 && index <= m_itemArray.size())
        {
            m_layout->Resize(Width(), m_layout->Height() + item->Height());

            float offset = 0.0f;
            // Lower Item
            for (size_t i = 0; i < index; ++i)
            {
                offset += m_itemArray[i]->Height();
            }
            // New Item
            {
                ConstraintLayout::GeometryInfo info = {};

                info.Left.ToLeft = 0.0f;
                info.Right.ToRight = 0.0f;
                info.Top.ToTop = offset;
                info.keepWidth = false;

                m_layout->AddElement(item, info);

                offset += item->Height();
            }
            // Higher Item
            for (size_t i = index; i < m_itemArray.size(); ++i)
            {
                auto pInfo = m_layout->PeekElemGeoInfo(m_itemArray[i]);
                if (pInfo != nullptr)
                {
                    pInfo->Top.ToTop = offset;
                }
                m_layout->UpdateElement(m_itemArray[i]);

                offset += m_itemArray[i]->Height();
            }
            m_itemArray.insert(m_itemArray.begin() + index, item);
        }
    }

    void ListView::AppendItem(ShrdPtrParam<Panel> item)
    {
        InsertItem(item, m_itemArray.size());
    }

    void ListView::RemoveItem(size_t index)
    {
        if (index >= 0 && index < m_itemArray.size())
        {
            auto itemItor = m_itemArray.begin() + index;
            m_layout->Resize(Width(), m_layout->Height() - (*itemItor)->Height());

            float offset = 0.0f;
            // Lower Item
            for (size_t i = 0; i < index; ++i)
            {
                offset += m_itemArray[i]->Height();
            }
            // Higher Item
            for (size_t i = index + 1; i < m_itemArray.size(); ++i)
            {
                auto pInfo = m_layout->PeekElemGeoInfo(m_itemArray[i]);
                if (pInfo != nullptr)
                {
                    pInfo->Top.ToTop = offset;
                }
                m_layout->UpdateElement(m_itemArray[i]);

                offset += m_itemArray[i]->Height();
            }
            m_itemArray.erase(itemItor);
        }
    }

    void ListView::RemoveItem(ShrdPtrParam<Panel> item)
    {
        RemoveItem(std::find(m_itemArray.begin(), m_itemArray.end(), item) - m_itemArray.begin());
    }

    size_t ListView::AvailableItemCount()
    {
        return m_itemArray.size();
    }

    void ListView::UpdateItemConstraints()
    {
        float offset = 0.0f;

        for (size_t i = 0; i < m_itemArray.size(); ++i)
        {
            auto& item = m_itemArray[i];

            auto pInfo = m_layout->PeekElemGeoInfo(item);
            if (pInfo != nullptr)
            {
                pInfo->Top.ToTop = offset;
            }
            offset += item->Height();
        }
        m_layout->Resize(Width(), offset);
        m_layout->UpdateAllElements();
    }
}