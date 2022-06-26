#pragma once

#include "Precompile.h"

namespace d14engine::renderer
{
    struct GpuBuffer { ComPtr<ID3D12Resource> resource = {}; };

    // Maintain a resource buffer with DEFAULT type (static).
    // Pros: optimized for GPU, working at the best performance.
    // Cons: immutable at runtime, isolated between CPU and GPU.
    struct DefaultBuffer : GpuBuffer
    {
        // Bridge between CPU data and GPU buffer, used to upload resources.
        // This buffer should be of the same size and layout with target buffer.
        ComPtr<ID3D12Resource> intermidiate = {};

        // This ctor only create the buffer and its intermidiate peer (no init).
        // You should initialize the buffer via intermidiate peer later manually.
        DefaultBuffer(ID3D12Device* device, UINT64 byteSize);

        void UploadData(ID3D12GraphicsCommandList* cmdList, void* pSrc, UINT64 byteSize);
    };

    // Maintain a resource buffer with UPLOAD type (dynamic).
    // Pros: modifiable at runtime, coherent between CPU and GPU.
    // Cons: wouldn't work at the best performance of GPU device.
    struct UploadBuffer : GpuBuffer
    {
        // Total element count in buffer.
        UINT elemCount = {};

        // Byte size of single element in buffer.
        UINT64 elemByteSize = {};

        // Where in memory the buffer is mapped to.
        // Use BYTE* to be compatible with DirectX.
        BYTE* mapped = nullptr;

        // This ctor only create the buffer and map it to the memory space.
        // You should initialize the buffer via mapped addr later manually.
        UploadBuffer(ID3D12Device* device, UINT elemCount, UINT64 elemByteSize);

        virtual ~UploadBuffer();

        // Upload data via copying source to mapped addr.
        //
        // @param dstIndexOffset: element offset in destination buffer.
        // 
        // Note pSrc should be offset to correct position before calling.
        // If you bind a specific array with this buffer, pSrc should be
        // set to &array[srcIndexOffset] to upload the related element data.
        //
        void CopyData(UINT dstIndexOffset, void* pSrc, UINT64 byteSize);
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