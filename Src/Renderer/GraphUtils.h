#pragma once

#include "Common/Precompile.h"
#include "Common/RuntimeError.h"

namespace d14engine::renderer
{
    struct GraphUtils
    {

#ifdef _DEBUG

#define NAME_D3D12_OBJECT(Object) \
    Object->SetName(L#Object)

#define NAME_D3D12_OBJECT_INDEXED(Object, Index) \
    Object->SetName((L#Object L"[" + std::to_wstring(Index) + L"]").c_str())

#else

#define NAME_D3D12_OBJECT(Object)

#define NAME_D3D12_OBJECT_INDEXED(Object, Index)

#endif

#define THROW_IF_ERROR(Expression) \
do { \
    ComPtr<ID3DBlob> error; \
    HRESULT hr = (Expression); \
    if (error != nullptr) \
    { \
        OutputDebugStringA((char*)error->GetBufferPointer()); \
    } \
    THROW_IF_FAILED(hr); \
} while (0)

#define REVERT_BARRIER_STATE(Barrier) \
do { \
    std::swap((Barrier).Transition.StateBefore, \
              (Barrier).Transition.StateAfter); \
} while (0)

#define REVERT_BARRIER_ARRAY_STATES(Barrier_Array) \
do { \
    for (auto& d14_br : (Barrier_Array)) \
    { \
        REVERT_BARRIER_STATE(d14_br); \
    } \
} while (0)

        static D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineStateDescTemplate();
    };
    // Introduce this alias to avoid too long prefix.
    using Graphu = GraphUtils;
}