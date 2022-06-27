#pragma once

#include "Common/Precompile.h"

#include "Renderer/IDrawObject.h"

namespace d14engine::renderer
{
    struct ICamera : IDrawObject
    {
        virtual void OnViewResize(UINT viewWidth, UINT viewHeight) = 0;
    };

    struct IMainCamera : ICamera
    {
        virtual D3D12_VIEWPORT Viewport() = 0;

        virtual D3D12_RECT ScissorRect() = 0;
    };
}