#pragma once

#include "Precompile.h"

#include "ISortable.h"

namespace d14engine::renderer
{
    struct Renderer;

    struct IDrawObject : ISortable<IDrawObject>
    {
        virtual bool IsD3D12ObjectVisible() = 0;

        virtual void SetD3D12ObjectVisible(bool value) = 0;

        virtual void OnRendererUpdateObject(Renderer* rndr) = 0;

        virtual void OnRendererDrawD3D12Object(Renderer* rndr) = 0;
    };
}