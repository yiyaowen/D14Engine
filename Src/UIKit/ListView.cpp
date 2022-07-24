#include "Common/Precompile.h"

#include "UIKit/ListView.h"

namespace d14engine::uikit
{
    ListView::ListView(const D2D1_RECT_F& rect)
        :
        Panel(rect, Resu::SOLID_COLOR_BRUSH),
        ScrollView(rect, MakeUIObject<ConstraintLayout>(SelfCoordRect()))
    {
        // We must place SetContent in OnInitializeFinish instead in ctor
        // since its implementation calls shared_from_this internally,
        // which can cause undefined result if object is initializing.

        isFocusable = true;
    }

    void ListView::OnInitializeFinish()
    {
        ScrollView::OnInitializeFinish();

        // No need to check as it was created in the ctor.
        m_layout = std::dynamic_pointer_cast<ConstraintLayout>(m_content);

        m_layout->background.opacity = 0.0f;
        m_layout->stroke.opacity = 0.0f;

        // In case trigger by mistake when control scroll bars.
        m_layout->f_isHit = [this](Panel* p, Event::Point& pt)
        {
            return Mathu::IsOverlapped(pt, m_layout->AbsoluteRect()) &&
                !m_isHorzBarHover && !m_isHorzBarDown && !m_isVertBarHover && !m_isVertBarDown;
        };
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

    void ListView::InsertItem(ShrdPtrParam<ListViewItem> item, size_t index)
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

                // Only display in-range items to improve performance.
                if (offset <= m_viewportOffset.y + Height() &&
                   (offset += item->Height()) >= m_viewportOffset.y)
                {
                    item->SetD2D1ObjectVisible(true);
                }
                else item->SetD2D1ObjectVisible(false);
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

            // Update selected item index set.
            ItemIndexSet updatedItemIndexSet = {};
            for (auto& idx : m_selectedItemIndexSet)
            {
                if (idx < index) updatedItemIndexSet.insert(idx);
                else if (idx > index) updatedItemIndexSet.insert(idx + 1);
            }
            m_selectedItemIndexSet = std::move(updatedItemIndexSet);
        }
    }

    void ListView::AppendItem(ShrdPtrParam<ListViewItem> item)
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

            // Update selected item index set.
            ItemIndexSet updatedItemIndexSet = {};
            for (auto& idx : m_selectedItemIndexSet)
            {
                if (idx < index) updatedItemIndexSet.insert(idx);
                else if (idx > index) updatedItemIndexSet.insert(idx - 1);
            }
            m_selectedItemIndexSet = std::move(updatedItemIndexSet);
        }
    }

    void ListView::RemoveItem(ShrdPtrParam<ListViewItem> item)
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
        for (auto& item : m_itemArray)
        {
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

    const ListView::ItemIndexSet& ListView::SelectedItemIndices()
    {
        return m_selectedItemIndexSet;
    }

    size_t ListView::GetViewportOffsetIndex(float offset)
    {
        if (enableEqualHeightItemOptimization)
        {
            if (m_itemArray.size() > 0)
            {
                auto index = (size_t)(offset / m_itemArray[0]->Height());
                return std::clamp(index, 0ull, m_itemArray.size() - 1);
            }
            else return SIZE_T_MAX;
        }
        else // We have to compute recursively if heights are different.
        {
            float dist = 0.0f;
            for (size_t i = 0; i < m_itemArray.size(); ++i)
            {
                auto& item = m_itemArray[i];
                // Don't use "dist < offset" here since it might cause unexpected
                // capture failure when the point is right on the edge of an item.
                if (dist <= offset && (dist += item->Height()) > offset)
                {
                    return i;
                }
            }
            return SIZE_T_MAX;
        }
    }

    void ListView::TriggerNoneSelect()
    {
        for (auto& idx : m_selectedItemIndexSet)
        {
            auto& item = m_itemArray[idx];
            item->state = ListViewItem::UNCHECK_STATE_TRANS_MAP.at(item->state);
        }
        m_selectedItemIndexSet.clear();

        m_lastSelectedIndex = m_extendedSelectStartIndex = SIZE_T_MAX;
    }

    void ListView::TriggerSingleSelect(size_t index)
    {
        for (auto& idx : m_selectedItemIndexSet)
        {
            auto& item = m_itemArray[idx];
            item->state = ListViewItem::UNCHECK_STATE_TRANS_MAP.at(item->state);
        }
        auto& item = m_itemArray[index];
        item->state = ListViewItem::CHECK_STATE_TRANS_MAP.at(item->state);

        m_selectedItemIndexSet = { index };

        m_lastSelectedIndex = m_extendedSelectStartIndex = index;
    }

    void ListView::TriggerMultipleSelect(size_t index)
    {
        if (m_selectedItemIndexSet.find(index) != m_selectedItemIndexSet.end())
        {
            auto& item = m_itemArray[index];
            item->state = ListViewItem::UNCHECK_STATE_TRANS_MAP.at(item->state);

            m_selectedItemIndexSet.erase(index);
        }
        else // New selected item.
        {
            auto& item = m_itemArray[index];
            item->state = ListViewItem::CHECK_STATE_TRANS_MAP.at(item->state);

            m_selectedItemIndexSet.insert(index);
        }
        if (m_lastSelectedIndex != SIZE_T_MAX && m_lastSelectedIndex != index)
        {
            auto& item = m_itemArray[m_lastSelectedIndex];
            item->state = ListViewItem::LEAVE_STATE_TRANS_MAP.at(item->state);
        }
        m_lastSelectedIndex = m_extendedSelectStartIndex = index;
    }

    void ListView::TriggerExtendedSelect(size_t index)
    {
        if (KeyboardEvent::SHIFT())
        {
            if (!m_selectedItemIndexSet.empty())
            {
                for (auto& idx : m_selectedItemIndexSet)
                {
                    auto& item = m_itemArray[idx];
                    item->state = ListViewItem::UNCHECK_STATE_TRANS_MAP.at(item->state);
                }
                m_selectedItemIndexSet.clear();

                auto range = std::minmax(index, m_extendedSelectStartIndex);
                for (size_t i = range.first; i <= range.second; ++i)
                {
                    auto& item = m_itemArray[i];
                    item->state = ListViewItem::CHECK_STATE_TRANS_MAP.at(item->state);
                    if (i != index) // Only the last selected item needs to highlight.
                    {
                        item->state = ListViewItem::LEAVE_STATE_TRANS_MAP.at(item->state);
                    }
                    m_selectedItemIndexSet.insert(i);
                }
                m_lastSelectedIndex = index;
            }
            else TriggerSingleSelect(index);
        }
        else if (KeyboardEvent::CTRL())
        {
            TriggerMultipleSelect(index);
        }
        else TriggerSingleSelect(index);
    }

    void ListView::SetItemIndexRangeVisible(bool value)
    {
        if (m_visibleItemIndexRange.first >= 0 && m_visibleItemIndexRange.first < m_itemArray.size())
        {
            size_t last = std::clamp(m_visibleItemIndexRange.last,
                m_visibleItemIndexRange.first, m_itemArray.size() - 1);

            // Change visibility of specified item index range.
            for (size_t i = m_visibleItemIndexRange.first; i <= last; ++i)
            {
                m_itemArray[i]->SetD2D1ObjectVisible(value);
            }
        }
    }

    void ListView::OnSelectChange(const ItemIndexSet& selected)
    {
        if (f_onSelectChangeOverride)
        {
            f_onSelectChangeOverride(this, selected);
        }
        else
        {
            if (f_onSelectChangeBefore) f_onSelectChangeBefore(this, selected);

            OnSelectChangeHelper(selected);

            if (f_onSelectChangeAfter) f_onSelectChangeAfter(this, selected);
        }
    }

    void ListView::OnSelectChangeHelper(const ItemIndexSet& selected)
    {
        // TODO: add list view select changed logic.
    }

    bool ListView::OnGetFocusHelper()
    {
        for (size_t index : m_selectedItemIndexSet)
        {
            auto& item = m_itemArray[index];
            item->state = ListViewItem::GET_FOCUS_STATE_TRANS_MAP.at(item->state);
        }
        return ScrollView::OnGetFocusHelper();
    }

    bool ListView::OnLoseFocusHelper()
    {
        for (size_t index : m_selectedItemIndexSet)
        {
            auto& item = m_itemArray[index];
            item->state = ListViewItem::LOSE_FOCUS_STATE_TRANS_MAP.at(item->state);
        }
        return ScrollView::OnLoseFocusHelper();
    }

    bool ListView::OnMouseButtonHelper(MouseButtonEvent& e)
    {
        auto& p = e.cursorPoint;
        auto relative = AbsoluteToSelfCoord(p);

        if (e.status.LeftDown())
        {
            // In case trigger by mistake when control scroll bars.
            size_t index = (IsHit(p) && !IsControllingScrollBars()) ?
                GetViewportOffsetIndex(m_viewportOffset.y + relative.y) : SIZE_T_MAX;

            if (index != SIZE_T_MAX)
            {
                switch (selectMode)
                {
                case SelectMode::None: TriggerNoneSelect(); break;
                case SelectMode::Single: TriggerSingleSelect(index); break;
                case SelectMode::Multiple: TriggerMultipleSelect(index); break;
                case SelectMode::Extended: TriggerExtendedSelect(index); break;
                default: break;
                }
                OnSelectChange(m_selectedItemIndexSet);
            }
        }
        return ScrollView::OnMouseButtonHelper(e);
    }

    bool ListView::OnMouseMoveHelper(MouseMoveEvent& e)
    {
        auto& p = e.cursorPoint;
        auto relative = AbsoluteToSelfCoord(p);

        // In case trigger by mistake when control scroll bars.
        size_t index = (IsHit(p) && !IsControllingScrollBars()) ?
            GetViewportOffsetIndex(m_viewportOffset.y + relative.y) : SIZE_T_MAX;

        if (index != SIZE_T_MAX)
        {
            auto& currItem = m_itemArray[index];
            currItem->state = ListViewItem::ENTER_STATE_TRANS_MAP.at(currItem->state);
        }
        if (m_lastHoverIndex != SIZE_T_MAX && m_lastHoverIndex != index)
        {
            auto& lastItem = m_itemArray[m_lastHoverIndex];
            lastItem->state = ListViewItem::LEAVE_STATE_TRANS_MAP.at(lastItem->state);
        }
        m_lastHoverIndex = index;

        return ScrollView::OnMouseMoveHelper(e);
    }

    bool ListView::OnMouseLeaveHelper(MouseMoveEvent& e)
    {
        if (m_lastHoverIndex != SIZE_T_MAX)
        {
            auto& lastItem = m_itemArray[m_lastHoverIndex];
            lastItem->state = ListViewItem::LEAVE_STATE_TRANS_MAP.at(lastItem->state);
        }
        m_lastHoverIndex = SIZE_T_MAX;

        return ScrollView::OnMouseLeaveHelper(e);
    }

    void ListView::OnViewportOffsetChangeHelper(const D2D1_POINT_2F& offset)
    {
        ScrollView::OnViewportOffsetChangeHelper(offset);

        SetItemIndexRangeVisible(false); // Hide old items.

        m_visibleItemIndexRange.first = GetViewportOffsetIndex(offset.y);
        m_visibleItemIndexRange.last = GetViewportOffsetIndex(offset.y + Height());

        SetItemIndexRangeVisible(true); // Show new items.
    }
}