#include "Precompile.h"

#include "UI/MaskStyle.h"

#include "UI/Application.h"
#include "UI/BitmapUtils.h"

namespace d14engine::ui
{
    MaskStyle::MaskStyle(
        UINT maskBitmapWidth,
        UINT maskBitmapHeight,
        D2D1_COLOR_F maskColor)
        :
        maskColor(maskColor)
    {
        LoadMaskBitmap(maskBitmapWidth, maskBitmapHeight);
    }

    void MaskStyle::LoadMaskBitmap(UINT width, UINT height)
    {
        Application::RENDERER->BeginExternalEvent();

        m_maskBitmap = Bitmapu::LoadBitmapFromMemory(
            width, height, nullptr, D2D1_BITMAP_OPTIONS_TARGET);

        Application::RENDERER->EndExternalEvent();
    }

    void MaskStyle::BeginDrawOnMask(ID2D1DeviceContext* context, const D2D1_MATRIX_3X2_F& transform)
    {
        context->SetTarget(m_maskBitmap.Get());

        context->BeginDraw();
        context->SetTransform(transform);

        context->Clear(maskColor);
    }

    void MaskStyle::EndDrawOnMask(ID2D1DeviceContext* context)
    {
        context->EndDraw();
    }

    void MaskStyle::PostMaskToScene(ID2D1DeviceContext* context, const D2D1_RECT_F& destinationRectangle)
    {
        context->DrawBitmap(m_maskBitmap.Get(), destinationRectangle);
    }
}