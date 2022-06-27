#include "Common/Precompile.h"

#include "Renderer/StaticSampler.h"

namespace d14engine::renderer
{
    D3D12_STATIC_SAMPLER_DESC StaticSampler::PointWrap(UINT shaderRegister)
    {
        return CD3DX12_STATIC_SAMPLER_DESC(
            shaderRegister,
            D3D12_FILTER_MIN_MAG_MIP_POINT,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP);
    }

    D3D12_STATIC_SAMPLER_DESC StaticSampler::PointMirror(UINT shaderRegister)
    {
        return CD3DX12_STATIC_SAMPLER_DESC(
            shaderRegister,
            D3D12_FILTER_MIN_MAG_MIP_POINT,
            D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
            D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
            D3D12_TEXTURE_ADDRESS_MODE_MIRROR);
    }

    D3D12_STATIC_SAMPLER_DESC StaticSampler::PointClamp(UINT shaderRegister)
    {
        return CD3DX12_STATIC_SAMPLER_DESC(
            shaderRegister,
            D3D12_FILTER_MIN_MAG_MIP_POINT,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP);
    }

    D3D12_STATIC_SAMPLER_DESC StaticSampler::PointBorder(UINT shaderRegister)
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

    D3D12_STATIC_SAMPLER_DESC StaticSampler::PointMirrorOnce(UINT shaderRegister)
    {
        return CD3DX12_STATIC_SAMPLER_DESC(
            shaderRegister,
            D3D12_FILTER_MIN_MAG_MIP_POINT,
            D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE,
            D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE,
            D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE);
    }

    D3D12_STATIC_SAMPLER_DESC StaticSampler::LinearWrap(UINT shaderRegister)
    {
        return CD3DX12_STATIC_SAMPLER_DESC(
            shaderRegister,
            D3D12_FILTER_MIN_MAG_MIP_LINEAR,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP);
    }

    D3D12_STATIC_SAMPLER_DESC StaticSampler::LinearMirror(UINT shaderRegister)
    {
        return CD3DX12_STATIC_SAMPLER_DESC(
            shaderRegister,
            D3D12_FILTER_MIN_MAG_MIP_LINEAR,
            D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
            D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
            D3D12_TEXTURE_ADDRESS_MODE_MIRROR);
    }

    D3D12_STATIC_SAMPLER_DESC StaticSampler::LinearClamp(UINT shaderRegister)
    {
        return CD3DX12_STATIC_SAMPLER_DESC(
            shaderRegister,
            D3D12_FILTER_MIN_MAG_MIP_LINEAR,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP);
    }

    D3D12_STATIC_SAMPLER_DESC StaticSampler::LinearBorder(UINT shaderRegister)
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

    D3D12_STATIC_SAMPLER_DESC StaticSampler::LinearMirrorOnce(UINT shaderRegister)
    {
        return CD3DX12_STATIC_SAMPLER_DESC(
            shaderRegister,
            D3D12_FILTER_MIN_MAG_MIP_LINEAR,
            D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE,
            D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE,
            D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE);
    }

    D3D12_STATIC_SAMPLER_DESC StaticSampler::AnisotropicWrap(UINT shaderRegister, UINT maxAnisotropy)
    {
        return CD3DX12_STATIC_SAMPLER_DESC(
            shaderRegister,
            D3D12_FILTER_ANISOTROPIC,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            0.0f, maxAnisotropy);
    }

    D3D12_STATIC_SAMPLER_DESC StaticSampler::AnisotropicMirror(UINT shaderRegister, UINT maxAnisotropy)
    {
        return CD3DX12_STATIC_SAMPLER_DESC(
            shaderRegister,
            D3D12_FILTER_ANISOTROPIC,
            D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
            D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
            D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
            0.0f, maxAnisotropy);
    }

    D3D12_STATIC_SAMPLER_DESC StaticSampler::AnisotropicClamp(UINT shaderRegister, UINT maxAnisotropy)
    {
        return CD3DX12_STATIC_SAMPLER_DESC(
            shaderRegister,
            D3D12_FILTER_ANISOTROPIC,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            0.0f, maxAnisotropy);
    }

    D3D12_STATIC_SAMPLER_DESC StaticSampler::AnisotropicBorder(UINT shaderRegister, UINT maxAnisotropy)
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

    D3D12_STATIC_SAMPLER_DESC StaticSampler::AnisotropicMirrorOnce(UINT shaderRegister, UINT maxAnisotropy)
    {
        return CD3DX12_STATIC_SAMPLER_DESC(
            shaderRegister,
            D3D12_FILTER_ANISOTROPIC,
            D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE,
            D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE,
            D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE,
            0.0f, maxAnisotropy);
    }
}