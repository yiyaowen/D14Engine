#pragma once

#include "Common/Precompile.h"

namespace d14engine::uikit
{
    template<typename State_T, typename StateChangeEvent_T>
    struct StatefulObject
    {
        using StatefulObjectType = StatefulObject<State_T, StateChangeEvent_T>;

        using State = State_T;
        using Event = StateChangeEvent_T;

    protected:
        State_T m_state = {};

    public:
        void OnStateChange(StateChangeEvent_T& e)
        {
            if (f_onStateChangeOverride)
            {
                f_onStateChangeOverride(this, e);
            }
            else
            {
                if (f_onStateChangeBefore) f_onStateChangeBefore(this, e);

                OnStateChangeHelper(e);

                if (f_onStateChangeAfter) f_onStateChangeAfter(this, e);
            }
        }

        Function<void(StatefulObjectType*,StateChangeEvent_T&)>
            f_onStateChangeOverride = {},
            f_onStateChangeBefore = {},
            f_onStateChangeAfter = {};

    protected:
        virtual void OnStateChangeHelper(StateChangeEvent_T& e)
        {
            // TODO: add stateful-object state changing logic.
        }
    };
}