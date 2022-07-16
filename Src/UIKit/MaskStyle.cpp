#include "Common/Precompile.h"

#include "UIKit/MaskStyle.h"

#include "UIKit/Application.h"
#include "UIKit/BitmapUtils.h"

namespace d14engine::uikit
{
    MaskStyle::MaskStyle(
        UINT bitmapWidth,
        UINT bitmapHeight,
        D2D1_COLOR_F color)
        :
        color(color)
    {
        LoadMaskBitmap(bitmapWidth, bitmapHeight);
    }

    void MaskStyle::LoadMaskBitmap(UINT width, UINT height)
    {
        Application::APP->MainRenderer()->BeginExternalEvent();

        bitmap = Bitmapu::LoadBitmapFromMemory(
            width, height, nullptr, D2D1_BITMAP_OPTIONS_TARGET);

        Application::APP->MainRenderer()->EndExternalEvent();
    }

    void MaskStyle::BeginMaskDraw(ID2D1DeviceContext* context, const D2D1_MATRIX_3X2_F& transform)
    {
        context->SetTarget(bitmap.Get());

        context->BeginDraw();
        context->SetTransform(transform);

        context->Clear(color);
    }

    void MaskStyle::EndMaskDraw(ID2D1DeviceContext* context)
    {
        context->EndDraw();
    }
}