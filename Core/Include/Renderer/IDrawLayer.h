#pragma once

#include "Precompile.h"

#include "ICamera.h"
#include "ISortable.h"

namespace d14engine::renderer
{
    struct Renderer;

    struct IDrawLayer : ISortable<IDrawLayer>
    {
        virtual SharedPtr<ICamera> AssociatedCamera() = 0;

        virtual bool IsD3D12LayerVisible() = 0;

        virtual void SetD3D12LayerVisible(bool value) = 0;

        // Don't update the associated camera in this method!
        virtual void OnRendererUpdateLayer(Renderer* rndr) = 0;

        virtual void OnRendererDrawD3D12Layer(Renderer* rndr) = 0;
    };
}