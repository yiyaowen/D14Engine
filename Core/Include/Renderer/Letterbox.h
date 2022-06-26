#pragma once

#include "Precompile.h"

#include "GpuBuffer.h"

namespace d14engine::renderer
{
    struct Renderer;

    class Letterbox
    {
    public:
        explicit Letterbox(Renderer* rndr);

    public:
        void OnParentSize(UINT sceneWidth, UINT sceneHeight, UINT windowWidth, UINT windowHeight);

        UINT SceneWidth() { return m_sceneWidth; }
        UINT SceneHeight() { return m_sceneHeight; }

        void PostSceneToBackBuffer(Renderer* rndr);

    private:
        UINT m_sceneWidth = {}, m_sceneHeight = {};

        D3D12_VIEWPORT m_viewport = {};
        D3D12_RECT m_scissorRect = {};

        struct Vertex
        {
            XMFLOAT4 position = {};
            XMFLOAT2 texcoord = {};
        };

        SharedPtr<DefaultBuffer> m_vertexBuffer = {};
        D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView = {};

        ComPtr<ID3D12RootSignature> m_rootSignature = {};
        ComPtr<ID3D12PipelineState> m_pipelineState = {};
    };
}