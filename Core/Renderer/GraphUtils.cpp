#include "Precompile.h"

#include "Renderer/GraphUtils.h"

namespace d14engine::renderer
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC GraphUtils::PipelineStateDescTemplate()
    {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

        // Based on the template, these fields should be specified:
        // input layout, root signature, shaders, sample desc ......

        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState.DepthEnable = TRUE;
        psoDesc.DepthStencilState.StencilEnable = FALSE;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        // Hardcode this format instead of checking from the renderer since it's the most popular one.
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.SampleDesc.Count = 1;
        psoDesc.SampleDesc.Quality = 0;

        return psoDesc;
    }
}