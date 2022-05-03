#include "Precompile.h"

#include "UI/MaskStyle.h"

#include "UI/BitmapUtils.h"

namespace d14engine::ui
{
    MaskStyle::MaskStyle(
        UINT maskBitmapWidth,
        UINT maskBitmapHeight,
        BYTE maskBitmapPadding)
    {
        LoadMaskBitmap(maskBitmapWidth, maskBitmapHeight, maskBitmapPadding);
    }

    void MaskStyle::LoadMaskBitmap(UINT width, UINT height, BYTE padding)
    {
        m_maskBitmap = Bitmapu::CreateBytePaddingBitmap(
            padding, width, height, D2D1_BITMAP_OPTIONS_TARGET);
    }

    void MaskStyle::BeginDrawOnMask(ID2D1DeviceContext* context, const D2D1_MATRIX_3X2_F& transform)
    {
        context->SetTarget(m_maskBitmap.Get());

        context->BeginDraw();
        context->SetTransform(transform);
        context->Clear({ 0.0f, 0.0f, 0.0f, 0.0f });
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