#pragma once

#include "Common/Precompile.h"

namespace d14engine
{
    // Simulate "finally" mechanism with RAII.
    template<typename T>
    struct FinalAction
    {
        T clean;
        explicit FinalAction(T functor) : clean{ functor } { }
        virtual ~FinalAction() { clean(); }
    };
    template<typename T>
    FinalAction<T> Finally(T functor) { return FinalAction<T>(functor); }

    // Provide hash operation for enum class.
    template<typename T>
    struct EnumClassHash
    {
        size_t operator()(T value) const { return (size_t)value; }
    };
}