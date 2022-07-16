#pragma once

#include "Common/Precompile.h"

#include "UIKit/ConstraintLayout.h"
#include "UIKit/ScrollView.h"

namespace d14engine::uikit
{
    struct ListView : ScrollView
    {
        explicit ListView(const D2D1_RECT_F& rect);

        void OnInitializeFinish() override;

    protected:
        SharedPtr<ConstraintLayout> m_layout = {};

        using ItemArray = std::vector<SharedPtr<Panel>>;

        ItemArray m_itemArray = {};

    public:
        WeakPtr<Panel> FindItem(size_t index);

        void InsertItem(ShrdPtrParam<Panel> item, size_t index);
        void AppendItem(ShrdPtrParam<Panel> item);

        void RemoveItem(size_t index);
        void RemoveItem(ShrdPtrParam<Panel> item);

        size_t AvailableItemCount();

        // Recalculate each item's position and update layout.
        void UpdateItemConstraints();
    };
}