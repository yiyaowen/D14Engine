#pragma once

#include "Precompile.h"

namespace d14engine::ui
{
    struct ShadowStyle
    {
        ShadowStyle(
            UINT shadowBitmapWidth,
            UINT shadowBitmapHeight,
            BYTE shadowBitmapPadding = 255,
            float shadowStandardDeviation = 3.0f,
            D2D1_COLOR_F shadowColor = (D2D1::ColorF)D2D1::ColorF::Gray,
            D2D1_SHADOW_OPTIMIZATION shadowOptimization = D2D1_SHADOW_OPTIMIZATION_BALANCED,
            float shadowExtensionOffset = 0.0f);

        // This field decides the appearance of shadow.
        /// Use extension offset to change Panel's IsHit area.
        float shadowStandardDeviation;

        D2D1_COLOR_F shadowColor;

        D2D1_SHADOW_OPTIMIZATION shadowOptimization;

        // This field decides the extension of IsHit area.
        // Use standard deviation to change shadow appearance.
        float shadowExtensionOffset;

    protected:
        ComPtr<ID2D1Bitmap1> m_shadowBitmap;

        void LoadShadowBitmap(UINT width, UINT height, BYTE padding = 255);

        D2D1_RECT_F ShadedRect(const D2D1_RECT_F& flatRect);
    };
}