#include "Common/Precompile.h"

#include "Renderer/Letterbox.h"

#include "Renderer/GraphUtils.h"
#include "Renderer/Renderer.h"
#include "Renderer/StaticSampler.h"

namespace d14engine::renderer
{
    Letterbox::Letterbox(Renderer* rndr)
    {
        // Create root signature.
        {
            CD3DX12_ROOT_PARAMETER1 rootParameter;
            rootParameter.InitAsDescriptorTable(
                1, &CD3DX12_DESCRIPTOR_RANGE1(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0), D3D12_SHADER_VISIBILITY_PIXEL);

            // Allow input layout and pixel shader access and deny uneccessary access to certain pipeline stages.
            D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
                D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
                D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
                D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
                D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

            CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
            rootSignatureDesc.Init_1_1(1, &rootParameter, 1, &StaticSampler::LinearBorder(0), rootSignatureFlags);

            ComPtr<ID3DBlob> signature;
#ifdef _DEBUG
            THROW_IF_ERROR(D3DX12SerializeVersionedRootSignature(
                &rootSignatureDesc, rndr->deviceInfo.features.rootSignature.HighestVersion, &signature, &error));
#else
            THROW_IF_FAILED(D3DX12SerializeVersionedRootSignature(
                &rootSignatureDesc, rootSignatureFeature.HighestVersion, &signature, &error));
#endif
            THROW_IF_FAILED(rndr->device->CreateRootSignature(
                0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));
        }
        // Create pipeline state.
        {
            ComPtr<ID3DBlob> vertexShader;
            ComPtr<ID3DBlob> pixelShader;

#ifdef _DEBUG
            UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

            THROW_IF_ERROR(D3DCompileFromFile((
                rndr->commonInfo.assetsPath + L"Shaders/Letterbox.hlsl").c_str(),
                nullptr, nullptr, "VS", "vs_5_0", compileFlags, 0, &vertexShader, &error));

            THROW_IF_ERROR(D3DCompileFromFile((
                rndr->commonInfo.assetsPath + L"Shaders/Letterbox.hlsl").c_str(),
                nullptr, nullptr, "PS", "ps_5_0", compileFlags, 0, &pixelShader, &error));
#else
            THROW_IF_FAILED(D3DCompileFromFile((
                rndr->commonInfo.assetsPath + L"Shaders/Letterbox.hlsl").c_str(),
                nullptr, nullptr, "VS", "vs_5_0", compileFlags, 0, &vertexShader, &error));

            THROW_IF_FAILED(D3DCompileFromFile((
                rndr->commonInfo.assetsPath + L"Shaders/Letterbox.hlsl").c_str(),
                nullptr, nullptr, "PS", "ps_5_0", compileFlags, 0, &pixelShader, &error));
#endif
            D3D12_INPUT_ELEMENT_DESC inputLayout[] =
            {
                { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
            };

            auto psoDesc = Graphu::PipelineStateDescTemplate();
            psoDesc.InputLayout = { inputLayout, _countof(inputLayout) };
            psoDesc.pRootSignature = m_rootSignature.Get();
            psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
            psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
            // No need to do depth test.
            psoDesc.DepthStencilState.DepthEnable = FALSE;

            THROW_IF_FAILED(rndr->device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));
        }
        // Create quadrangle vertex buffer.
        {
            Vertex quad[] =
            {
                { { -1.0f, +1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } }, // Top Left
                { { +1.0f, +1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } }, // Top Right
                { { -1.0f, -1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } }, // Bottom Left
                { { +1.0f, -1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } }, // Bottom Right
            };
            m_vertexBuffer = std::make_shared<DefaultBuffer>(rndr->device.Get(), sizeof(quad));
            m_vertexBuffer->UploadData(rndr->cmdList.Get(), quad, sizeof(quad));

            m_vertexBufferView.BufferLocation = m_vertexBuffer->resource->GetGPUVirtualAddress();
            m_vertexBufferView.SizeInBytes = sizeof(quad);
            m_vertexBufferView.StrideInBytes = sizeof(Vertex);
        }
    }

    void Letterbox::OnParentSize(UINT sceneWidth, UINT sceneHeight, UINT windowWidth, UINT windowHeight)
    {
        m_sceneWidth = sceneWidth;
        m_sceneHeight = sceneHeight;

        float horzRatio = sceneWidth / (float)windowWidth;
        float vertRatio = sceneHeight / (float)windowHeight;

        float x = 1.0f;
        float y = 1.0f;

        if (horzRatio < vertRatio)
        {
            // The scaled scene's height will fit to the window's height and 
            // its width will be smaller than the window's width.
            x = horzRatio / vertRatio;
        }
        else
        {
            // The scaled scene's width will fit to the window's width and 
            // its height may be smaller than the window's height.
            y = vertRatio / horzRatio;
        }

        m_viewport.TopLeftX = windowWidth * (1.0f - x) / 2.0f;
        m_viewport.TopLeftY = windowHeight * (1.0f - y) / 2.0f;
        m_viewport.Width = x * windowWidth;
        m_viewport.Height = y * windowHeight;
        m_viewport.MinDepth = 0.0f;
        m_viewport.MaxDepth = 1.0f;

        m_scissorRect.left = (LONG)m_viewport.TopLeftX;
        m_scissorRect.right = (LONG)(m_viewport.TopLeftX + m_viewport.Width);
        m_scissorRect.top = (LONG)m_viewport.TopLeftY;
        m_scissorRect.bottom = (LONG)(m_viewport.TopLeftY + m_viewport.Height);
    }

    void Letterbox::PostSceneToBackBuffer(Renderer* rndr)
    {
        rndr->TransitionBackBufferState(D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        rndr->TransitionSceneBufferState(D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        rndr->cmdList->RSSetViewports(1, &m_viewport);
        rndr->cmdList->RSSetScissorRects(1, &m_scissorRect);

        auto rtvHandle = rndr->BackRtvHandle();

        // No need to do depth test.
        rndr->cmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
        rndr->cmdList->ClearRenderTargetView(rtvHandle, rndr->letterboxColor, 0, nullptr);

        rndr->cmdList->SetGraphicsRootSignature(m_rootSignature.Get());
        rndr->cmdList->SetPipelineState(m_pipelineState.Get());

        ID3D12DescriptorHeap* ppHeaps[] = { rndr->srvHeap.Get() };
        rndr->cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

        // Bind scene buffer to shader texture slot.
        rndr->cmdList->SetGraphicsRootDescriptorTable(0, rndr->srvHeap->GetGPUDescriptorHandleForHeapStart());

        // Don't use TRIANGLELIST!
        rndr->cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        rndr->cmdList->IASetVertexBuffers(0, 1, &m_vertexBufferView);

        rndr->cmdList->DrawInstanced(4, 1, 0, 0);

        rndr->TransitionBackBufferState(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        rndr->TransitionSceneBufferState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COMMON);
    }
}