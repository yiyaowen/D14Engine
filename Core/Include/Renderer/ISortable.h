#pragma once

#include "Precompile.h"

namespace d14engine::renderer
{
    template<typename T>
    struct ISortable
    {
    protected:
        int m_priority = 0;

    public:
        const ISortable<T>* ID() const { return this; }

        int Priority() const { return m_priority; }
        
        struct UniqueAscending
        {
            bool operator() (ShrdPtrParam<ISortable<T>> left, ShrdPtrParam<ISortable<T>> right) const
            {
                return operator()(left.get(), right.get());
            }
            bool operator() (ISortable<T>* left, ISortable<T>* right) const
            {
                if (left->ID() == right->ID())
                {
                    return false;
                }
                else // Strict unique according to ID.
                {
                    if (left->Priority() == right->Priority())
                    {
                        return left->ID() < right->ID();
                    }
                    else // Strict ascending according to Priority.
                    {
                        return left->Priority() < right->Priority();
                    }
                }
            }
        };
    };
}