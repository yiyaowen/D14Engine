#pragma once

#include "Precompile.h"

#include "ISortable.h"

namespace d14engine::renderer
{
    struct Renderer;

    struct IDrawObject2D : ISortable<IDrawObject2D>
    {
        virtual bool IsD2D1ObjectVisible() = 0;

        virtual void SetD2D1ObjectVisible(bool value) = 0;

        virtual void OnRendererUpdateObject2D(Renderer* rndr) = 0;

        virtual void OnRendererDrawD2D1Layer(Renderer* rndr) = 0;

        virtual void OnRendererDrawD2D1Object(Renderer* rndr) = 0;
    };
}