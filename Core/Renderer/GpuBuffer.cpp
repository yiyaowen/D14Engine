#include "Precompile.h"

#include "Renderer/GpuBuffer.h"

namespace d14engine::renderer
{
    DefaultBuffer::DefaultBuffer(ID3D12Device* device, UINT64 byteSize)
    {
        THROW_IF_FAILED(device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(byteSize),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&resource)));

        THROW_IF_FAILED(device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(byteSize),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&intermidiate)));
    }

    void DefaultBuffer::UploadData(ID3D12GraphicsCommandList* cmdList, void* pSrc, UINT64 byteSize)
    {
        // Copy from source data to intermidiate buffer.
        BYTE* mapped = nullptr;
        THROW_IF_FAILED(intermidiate->Map(0, nullptr, (void**)&mapped));

        memcpy(mapped, pSrc, byteSize);

        intermidiate->Unmap(0, nullptr);

        // Copy from intermidiate buffer to resource buffer.
        D3D12_RESOURCE_BARRIER b = CD3DX12_RESOURCE_BARRIER::Transition(
            resource.Get(),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            D3D12_RESOURCE_STATE_COPY_DEST);
        cmdList->ResourceBarrier(1, &b);

        cmdList->CopyBufferRegion(resource.Get(), 0, intermidiate.Get(), 0, byteSize);

        std::swap(b.Transition.StateAfter, b.Transition.StateBefore);
        cmdList->ResourceBarrier(1, &b);
    }

    UploadBuffer::UploadBuffer(ID3D12Device* device, UINT elemCount, UINT64 elemByteSize)
        : elemCount(elemCount), elemByteSize(elemByteSize)
    {
        THROW_IF_FAILED(device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(elemCount * elemByteSize),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&resource)));

        THROW_IF_FAILED(resource->Map(0, nullptr, (void**)&mapped));
    }

    UploadBuffer::~UploadBuffer()
    {
        // Unmap reference before buffer released!
        if (resource != nullptr)
        {
            resource->Unmap(0, nullptr);
        }
        mapped = nullptr;
    }

    void UploadBuffer::CopyData(UINT dstIndexOffset, void* pSrc, UINT64 byteSize)
    {
        memcpy(&mapped[dstIndexOffset * elemByteSize], pSrc, byteSize);
    }
}