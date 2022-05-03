#pragma once

#include "Precompile.h"

namespace d14engine::renderer
{
    struct StaticSampler
    {
        static D3D12_STATIC_SAMPLER_DESC PointWrap(UINT shaderRegister)
        {
            return CD3DX12_STATIC_SAMPLER_DESC(
                shaderRegister,
                D3D12_FILTER_MIN_MAG_MIP_POINT,
                D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                D3D12_TEXTURE_ADDRESS_MODE_WRAP);
        }
        static D3D12_STATIC_SAMPLER_DESC PointMirror(UINT shaderRegister)
        {
            return CD3DX12_STATIC_SAMPLER_DESC(
                shaderRegister,
                D3D12_FILTER_MIN_MAG_MIP_POINT,
                D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
                D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
                D3D12_TEXTURE_ADDRESS_MODE_MIRROR);
        }
        static D3D12_STATIC_SAMPLER_DESC PointClamp(UINT shaderRegister)
        {
            return CD3DX12_STATIC_SAMPLER_DESC(
                shaderRegister,
                D3D12_FILTER_MIN_MAG_MIP_POINT,
                D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
                D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
                D3D12_TEXTURE_ADDRESS_MODE_CLAMP);
        }
        static D3D12_STATIC_SAMPLER_DESC PointBorder(UINT shaderRegister)
        {
            return CD3DX12_STATIC_SAMPLER_DESC(
                shaderRegister,
                D3D12_FILTER_MIN_MAG_MIP_POINT,
                D3D12_TEXTURE_ADDRESS_MODE_BORDER,
                D3D12_TEXTURE_ADDRESS_MODE_BORDER,
                D3D12_TEXTURE_ADDRESS_MODE_BORDER,
                0.0F, 16U, D3D12_COMPARISON_FUNC_LESS_EQUAL,
                D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK);
        }
        static D3D12_STATIC_SAMPLER_DESC PointMirrorOnce(UINT shaderRegister)
        {
            return CD3DX12_STATIC_SAMPLER_DESC(
                shaderRegister,
                D3D12_FILTER_MIN_MAG_MIP_POINT,
                D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE,
                D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE,
                D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE);
        }
        static D3D12_STATIC_SAMPLER_DESC LinearWrap(UINT shaderRegister)
        {
            return CD3DX12_STATIC_SAMPLER_DESC(
                shaderRegister,
                D3D12_FILTER_MIN_MAG_MIP_LINEAR,
                D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                D3D12_TEXTURE_ADDRESS_MODE_WRAP);
        }
        static D3D12_STATIC_SAMPLER_DESC LinearMirror(UINT shaderRegister)
        {
            return CD3DX12_STATIC_SAMPLER_DESC(
                shaderRegister,
                D3D12_FILTER_MIN_MAG_MIP_LINEAR,
                D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
                D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
                D3D12_TEXTURE_ADDRESS_MODE_MIRROR);
        }
        static D3D12_STATIC_SAMPLER_DESC LinearClamp(UINT shaderRegister)
        {
            return CD3DX12_STATIC_SAMPLER_DESC(
                shaderRegister,
                D3D12_FILTER_MIN_MAG_MIP_LINEAR,
                D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
                D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
                D3D12_TEXTURE_ADDRESS_MODE_CLAMP);
        }
        static D3D12_STATIC_SAMPLER_DESC LinearBorder(UINT shaderRegister)
        {
            return CD3DX12_STATIC_SAMPLER_DESC(
                shaderRegister,
                D3D12_FILTER_MIN_MAG_MIP_LINEAR,
                D3D12_TEXTURE_ADDRESS_MODE_BORDER,
                D3D12_TEXTURE_ADDRESS_MODE_BORDER,
                D3D12_TEXTURE_ADDRESS_MODE_BORDER,
                0.0F, 16U, D3D12_COMPARISON_FUNC_LESS_EQUAL,
                D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK);
        }
        static D3D12_STATIC_SAMPLER_DESC LinearMirrorOnce(UINT shaderRegister)
        {
            return CD3DX12_STATIC_SAMPLER_DESC(
                shaderRegister,
                D3D12_FILTER_MIN_MAG_MIP_LINEAR,
                D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE,
                D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE,
                D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE);
        }
        static D3D12_STATIC_SAMPLER_DESC AnisotropicWrap(UINT shaderRegister, UINT maxAnisotropy = 16)
        {
            return CD3DX12_STATIC_SAMPLER_DESC(
                shaderRegister,
                D3D12_FILTER_ANISOTROPIC,
                D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                0.0f, maxAnisotropy);
        }
        static D3D12_STATIC_SAMPLER_DESC AnisotropicMirror(UINT shaderRegister, UINT maxAnisotropy = 16)
        {
            return CD3DX12_STATIC_SAMPLER_DESC(
                shaderRegister,
                D3D12_FILTER_ANISOTROPIC,
                D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
                D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
                D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
                0.0f, maxAnisotropy);
        }
        static D3D12_STATIC_SAMPLER_DESC AnisotropicClamp(UINT shaderRegister, UINT maxAnisotropy = 16)
        {
            return CD3DX12_STATIC_SAMPLER_DESC(
                shaderRegister,
                D3D12_FILTER_ANISOTROPIC,
                D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
                D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
                D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
                0.0f, maxAnisotropy);
        }
        static D3D12_STATIC_SAMPLER_DESC AnisotropicBorder(UINT shaderRegister, UINT maxAnisotropy = 16)
        {
            return CD3DX12_STATIC_SAMPLER_DESC(
                shaderRegister,
                D3D12_FILTER_ANISOTROPIC,
                D3D12_TEXTURE_ADDRESS_MODE_BORDER,
                D3D12_TEXTURE_ADDRESS_MODE_BORDER,
                D3D12_TEXTURE_ADDRESS_MODE_BORDER,
                0.0F, maxAnisotropy,
                D3D12_COMPARISON_FUNC_LESS_EQUAL,
                D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK);
        }
        static D3D12_STATIC_SAMPLER_DESC AnisotropicMirrorOnce(UINT shaderRegister, UINT maxAnisotropy = 16)
        {
            return CD3DX12_STATIC_SAMPLER_DESC(
                shaderRegister,
                D3D12_FILTER_ANISOTROPIC,
                D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE,
                D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE,
                D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE,
                0.0f, maxAnisotropy);
        }
    };
}