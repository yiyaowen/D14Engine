#pragma once

#include "Precompile.h"

namespace d14engine::renderer
{
    // Define this interface as a template to enable distinguishing ISortable objects
    // according to specific types to maintain different priorities for different attributes.
    // For example, suppose class A inherits from both ISortable<B> and ISortable<C>:
    // A should hold B::priority and C::priority respectively and could be sorted by any of them.
    template<typename T>
    struct ISortable
    {
    protected:
        int m_priority = 0;

    public:
        const ISortable<T>* ID() const { return this; }

        int Priority() const { return m_priority; }
        
        struct RawAscending
        {
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
        struct ShrdAscending
        {
            bool operator() (ShrdPtrParam<ISortable<T>> left, ShrdPtrParam<ISortable<T>> right) const
            {
                return RawAscending()(left.get(), right.get());
            }
        };
        struct WeakAscending
        {
            bool operator() (WeakPtrParam<ISortable<T>> left, WeakPtrParam<ISortable<T>> right) const
            {
                // The order of comparison is significant here.
                // 1. Return false in advance of true to ensure total order relation.
                // 2. Compare right firstly to place expired elements in front of set.
                if (right.expired()) return false;
                if (left.expired()) return true;

                return ShrdAscending()(left.lock(), right.lock());
            }
        };

        using ShrdPrioritySet = std::set<SharedPtr<T>, ShrdAscending>;
        template<typename ValueType>
        using ShrdPriorityMap = std::map<SharedPtr<T>, ValueType, ShrdAscending>;

        using WeakPrioritySet = std::set<WeakPtr<T>, WeakAscending>;
        template<typename ValueType>
        using WeakPriorityMap = std::map<WeakPtr<T>, ValueType, WeakAscending>;

        // func's return boolean means whether to handle the remaining valid objects.
        static void Foreach(WeakPrioritySet& cont, const Function<bool(ShrdPtrParam<T>)>& func)
        {
            bool continueDeliver = true;

            for ( auto itor = cont.begin() ;; )
            {
                // Remove expired objects by the way. It doesn't waste much performance.
                // After all, the iteration will always be completed when calling this.
                while (itor != cont.end() && itor->expired())
                {
                    itor = cont.erase(itor);
                }
                if (itor != cont.end())
                {
                    if (continueDeliver)
                    {
                        continueDeliver = func(itor->lock());
                    }
                    ++itor; // Move the increment operation here.
                }
                else break; // Move the termination condition here.
            }
        }
    };
}