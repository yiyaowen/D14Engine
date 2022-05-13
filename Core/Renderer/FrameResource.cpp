#include "Precompile.h"

#include "Renderer/FrameResource.h"

namespace d14engine::renderer
{
    FrameResource::FrameResource(ID3D12Device* device)
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

    void FrameResource::ResetCmdList(ID3D12GraphicsCommandList* cmdList, CmdLayer layer)
    {
        auto& alloc = cmdAllocs[(size_t)layer];
        alloc->Reset();
        cmdList->Reset(alloc.Get(), nullptr);
    }
}