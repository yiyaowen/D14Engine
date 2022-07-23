#pragma once

#include "Common/Precompile.h"

#include "UIKit/ConstraintLayout.h"
#include "UIKit/ListViewItem.h"
#include "UIKit/ScrollView.h"

namespace d14engine::uikit
{
    struct ListView : ScrollView
    {
        explicit ListView(const D2D1_RECT_F& rect);

        enum class SelectMode { None, Single, Multiple, Extended };

        SelectMode selectMode = SelectMode::Extended;

        void OnInitializeFinish() override;

        // Some computation can be optimized when all items are of the same height.
        bool enableEqualHeightItemOptimization = true;

    protected:
        SharedPtr<ConstraintLayout> m_layout = {};

        using ItemArray = std::vector<SharedPtr<ListViewItem>>;

        ItemArray m_itemArray = {};

        using ItemIndexSet = std::set<size_t>;

        ItemIndexSet m_selectedItemIndexSet = {};

    public:
        WeakPtr<Panel> FindItem(size_t index);

        void InsertItem(ShrdPtrParam<ListViewItem> item, size_t index);
        void AppendItem(ShrdPtrParam<ListViewItem> item);

        void RemoveItem(size_t index);
        void RemoveItem(ShrdPtrParam<ListViewItem> item);

        size_t AvailableItemCount();

        // Recalculate each item's position and update layout.
        // Note the height of each item can be different,
        // so we need to call this method immediately to update
        // the appearance manually if any item's height changes.
        void UpdateItemConstraints();

        const ItemIndexSet& SelectedItemIndices();

    protected:
        // Which item the point with specified offset is on?
        size_t GetViewportOffsetIndex(float offset);

        size_t m_lastHoverIndex = SIZE_T_MAX;
        size_t m_lastSelectedIndex = SIZE_T_MAX;

        void TriggerNoneSelect();
        void TriggerSingleSelect(size_t index);
        void TriggerMultipleSelect(size_t index);
        void TriggerExtendedSelect(size_t index);

        size_t m_extendedSelectStartIndex = SIZE_T_MAX;

        void SetItemIndexRangeVisible(bool value);

        struct VisibleItemIndexRange
        {
            size_t first = 0, last = 0;
        }
        m_visibleItemIndexRange = {};

    public:
        void OnSelectChange(const ItemIndexSet& selected);

        Function<void(ListView*,const ItemIndexSet&)>
            f_onSelectChangeOverride = {},
            f_onSelectChangeBefore = {},
            f_onSelectChangeAfter = {};

    protected:
        virtual void OnSelectChangeHelper(const ItemIndexSet& selected);

    protected:
        // Override interface methods.

        // Panel
        bool OnGetFocusHelper() override;

        bool OnLoseFocusHelper() override;

        bool OnMouseButtonHelper(MouseButtonEvent& e) override;

        bool OnMouseMoveHelper(MouseMoveEvent& e) override;

        bool OnMouseLeaveHelper(MouseMoveEvent& e) override;

        // ScrollView
        void OnViewportOffsetChangeHelper(const D2D1_POINT_2F& offset) override;
    };
}