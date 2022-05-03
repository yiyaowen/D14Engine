#pragma once

#include "Precompile.h"

namespace d14engine::ui
{
    struct MaskStyle
    {
        MaskStyle(
            UINT maskBitmapWidth,
            UINT maskBitmapHeight,
            BYTE maskBitmapPadding = 0);

    protected:
        ComPtr<ID2D1Bitmap1> m_maskBitmap;

        void LoadMaskBitmap(UINT width, UINT height, BYTE padding = 0);

        void BeginDrawOnMask(ID2D1DeviceContext* context, const D2D1_MATRIX_3X2_F& transform = D2D1::Matrix3x2F::Identity());

        void EndDrawOnMask(ID2D1DeviceContext* context);

        void PostMaskToScene(ID2D1DeviceContext* context, const D2D1_RECT_F& destinationRectangle);
    };
}