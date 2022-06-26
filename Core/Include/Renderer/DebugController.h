#pragma once

#include "Precompile.h"

#include "Renderer.h"

namespace d14engine::renderer
{
    struct DebugController
    {
        static void EnableD3D12DebugLayer();

        static void SuppressUselessWarnings(Renderer* rndr);

        static ComPtr<IDXGIDebug> QueryDxgiDebugInterface();
    };
}