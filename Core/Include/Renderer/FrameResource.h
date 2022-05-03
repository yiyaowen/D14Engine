#pragma once

#include "Precompile.h"

#include "GraphUtils.h"
#include "RuntimeError.h"

namespace d14engine::renderer
{
    struct FrameResource
    {
        // Match with swap chain back buffer count!
        constexpr static UINT CIRCLE_COUNT = 3;

        enum class CmdLayer { Update, PreD3D12, D2D1, PostD3D12, ClearBkgn, PostScene, Count };
    
        using CmdAllocArray = std::array<ComPtr<ID3D12CommandAllocator>, (size_t)CmdLayer::Count>;

        CmdAllocArray cmdAllocs;

        UINT64 fenceValue = 0;

        explicit FrameResource(ID3D12Device* device)
        {
            for (int i = 0; i < cmdAllocs.size(); ++i)
            {
                auto& frameResourceCmdAlloc = cmdAllocs[i];

                THROW_IF_FAILED(device->CreateCommandAllocator(
                    D3D12_COMMAND_LIST_TYPE_DIRECT,
                    IID_PPV_ARGS(&frameResourceCmdAlloc)));

                NAME_D3D12_OBJECT_INDEXED(frameResourceCmdAlloc, i);
            }
        }

        void ResetCmdList(ID3D12GraphicsCommandList* cmdList, CmdLayer layer)
        {
            auto& alloc = cmdAllocs[(size_t)layer];
            alloc->Reset();
            cmdList->Reset(alloc.Get(), nullptr);
        }
    };
}