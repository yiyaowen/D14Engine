#pragma once

#include "Precompile.h"
#include "RuntimeError.h"

namespace d14engine::renderer
{
    struct GpuBuffer { ComPtr<ID3D12Resource> resource; };

    // Maintain a resource buffer with DEFAULT type (static).
    // Pros: optimized for GPU, working at the best performance.
    // Cons: immutable at runtime, isolated between CPU and GPU.
    struct DefaultBuffer : GpuBuffer
    {
        // Bridge between CPU data and GPU buffer, used to upload resources.
        // This buffer should be of the same size and layout with target buffer.
        ComPtr<ID3D12Resource> intermidiate;

        // This ctor only create the buffer and its intermidiate peer (no init).
        // You should initialize the buffer via intermidiate peer later manually.
        DefaultBuffer(ID3D12Device* device, UINT64 byteSize)
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

        void UploadData(ID3D12GraphicsCommandList* cmdList, void* pSrc, UINT64 byteSize)
        {
            // Copy from source data to intermidiate buffer.
            BYTE* mapped;
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
    };

    // Maintain a resource buffer with UPLOAD type (dynamic).
    // Pros: modifiable at runtime, coherent between CPU and GPU.
    // Cons: wouldn't work at the best performance of GPU device.
    struct UploadBuffer : GpuBuffer
    {
        // Total element count in buffer.
        UINT elemCount;

        // Byte size of single element in buffer.
        UINT64 elemByteSize;

        // Where in memory the buffer is mapped to.
        // Use BYTE* to be compatible with DirectX.
        BYTE* mapped = nullptr;

        // This ctor only create the buffer and map it to the memory space.
        // You should initialize the buffer via mapped addr later manually.
        UploadBuffer(ID3D12Device* device, UINT elemCount, UINT64 elemByteSize)
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

        virtual ~UploadBuffer()
        {
            // Unmap reference before buffer released!
            if (resource != nullptr)
            {
                resource->Unmap(0, nullptr);
            }
            mapped = nullptr;
        }

        // Upload data via copying source to mapped addr.
        //
        // @param dstIndexOffset: element offset in destination buffer.
        // 
        // Note pSrc should be offset to correct position before calling.
        // If you bind a specific array with this buffer, pSrc should be
        // set to &array[srcIndexOffset] to upload the related element data.
        //
        void CopyData(UINT dstIndexOffset, void* pSrc, UINT64 byteSize)
        {
            memcpy(&mapped[dstIndexOffset * elemByteSize], pSrc, byteSize);
        }
    };

    // Inherited from UploadBuffer (specialized for HLSL cbuffer).
    // This type of buffer is also created with UPLOAD type (dynamic); the only difference from UploadBuffer
    // is that the data in ConstantBuffer will be aligned by minimum hardware allocation size (usually 256 bytes).
    struct ConstantBuffer : UploadBuffer
    {
        ConstantBuffer(ID3D12Device* device, UINT elemCount, UINT64 elemByteSize)
            : UploadBuffer(device, elemCount, CalcElemSize(elemByteSize)) { }

    private:
        static UINT64 CalcElemSize(UINT64 rawByteSize)
        { return (rawByteSize + 255) & ~255; } // Round up to 256.
    };
}