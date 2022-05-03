#include "Precompile.h"

#include "UI/ShadowStyle.h"

#include "UI/BitmapUtils.h"

namespace d14engine::ui
{
    ShadowStyle::ShadowStyle(
        UINT shadowBitmapWidth,
        UINT shadowBitmapHeight,
        BYTE shadowBitmapPadding,
        float shadowStandardDeviation,
        D2D1_COLOR_F shadowColor,
        D2D1_SHADOW_OPTIMIZATION shadowOptimization,
        float shadowExtensionOffset)
        :
        shadowStandardDeviation(shadowStandardDeviation),
        shadowColor(shadowColor),
        shadowOptimization(shadowOptimization),
        shadowExtensionOffset(shadowExtensionOffset)
    {
        LoadShadowBitmap(shadowBitmapWidth, shadowBitmapHeight, shadowBitmapPadding);
    }

    void ShadowStyle::LoadShadowBitmap(UINT width, UINT height, BYTE padding)
    {
        m_shadowBitmap = Bitmapu::CreateBytePaddingBitmap(
            padding, width, height, D2D1_BITMAP_OPTIONS_TARGET);
    }

    D2D1_RECT_F ShadowStyle::ShadedRect(const D2D1_RECT_F& flatRect)
    {
        return
        {
            flatRect.left - shadowExtensionOffset,
            flatRect.top - shadowExtensionOffset,
            flatRect.right + shadowExtensionOffset,
            flatRect.bottom + shadowExtensionOffset
        };
    }
}