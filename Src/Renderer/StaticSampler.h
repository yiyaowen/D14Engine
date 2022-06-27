#pragma once

#include "Common/Precompile.h"

namespace d14engine::renderer
{
    struct StaticSampler
    {
        static D3D12_STATIC_SAMPLER_DESC PointWrap(UINT shaderRegister);
        static D3D12_STATIC_SAMPLER_DESC PointMirror(UINT shaderRegister);
        static D3D12_STATIC_SAMPLER_DESC PointClamp(UINT shaderRegister);
        static D3D12_STATIC_SAMPLER_DESC PointBorder(UINT shaderRegister);
        static D3D12_STATIC_SAMPLER_DESC PointMirrorOnce(UINT shaderRegister);
        static D3D12_STATIC_SAMPLER_DESC LinearWrap(UINT shaderRegister);
        static D3D12_STATIC_SAMPLER_DESC LinearMirror(UINT shaderRegister);
        static D3D12_STATIC_SAMPLER_DESC LinearClamp(UINT shaderRegister);
        static D3D12_STATIC_SAMPLER_DESC LinearBorder(UINT shaderRegister);
        static D3D12_STATIC_SAMPLER_DESC LinearMirrorOnce(UINT shaderRegister);
        static D3D12_STATIC_SAMPLER_DESC AnisotropicWrap(UINT shaderRegister, UINT maxAnisotropy = 16);
        static D3D12_STATIC_SAMPLER_DESC AnisotropicMirror(UINT shaderRegister, UINT maxAnisotropy = 16);
        static D3D12_STATIC_SAMPLER_DESC AnisotropicClamp(UINT shaderRegister, UINT maxAnisotropy = 16);
        static D3D12_STATIC_SAMPLER_DESC AnisotropicBorder(UINT shaderRegister, UINT maxAnisotropy = 16);
        static D3D12_STATIC_SAMPLER_DESC AnisotropicMirrorOnce(UINT shaderRegister, UINT maxAnisotropy = 16);
    };
}