#include "Common/Precompile.h"

#include "UIKit/MaskStyle.h"

#include "UIKit/Application.h"
#include "UIKit/BitmapUtils.h"

namespace d14engine::uikit
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
        Application::APP->MainRenderer()->BeginExternalEvent();

        maskBitmap = Bitmapu::LoadBitmapFromMemory(
            width, height, nullptr, D2D1_BITMAP_OPTIONS_TARGET);

        Application::APP->MainRenderer()->EndExternalEvent();
    }

    void MaskStyle::BeginDrawOnMask(ID2D1DeviceContext* context, const D2D1_MATRIX_3X2_F& transform)
    {
        context->SetTarget(maskBitmap.Get());

        context->BeginDraw();
        context->SetTransform(transform);

        context->Clear(maskColor);
    }

    void MaskStyle::EndDrawOnMask(ID2D1DeviceContext* context)
    {
        context->EndDraw();
    }
}