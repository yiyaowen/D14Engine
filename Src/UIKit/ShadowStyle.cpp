#include "Common/Precompile.h"

#include "UIKit/ShadowStyle.h"

#include "UIKit/Application.h"
#include "UIKit/BitmapUtils.h"
#include "UIKit/ResourceUtils.h"

namespace d14engine::uikit
{
    ShadowStyle::ShadowStyle(
        UINT shadowBitmapWidth,
        UINT shadowBitmapHeight,
        float shadowStandardDeviation,
        D2D1_COLOR_F shadowColor,
        D2D1_SHADOW_OPTIMIZATION shadowOptimization)
        :
        shadowStandardDeviation(shadowStandardDeviation),
        shadowColor(shadowColor),
        shadowOptimization(shadowOptimization)
    {
        LoadShadowBitmap(shadowBitmapWidth, shadowBitmapHeight);
    }

    void ShadowStyle::LoadShadowBitmap(UINT width, UINT height)
    {
        Resu::SHADOW_EFFECT->SetInput(0, nullptr);
        Application::APP->MainRenderer()->BeginExternalEvent();

        shadowBitmap = Bitmapu::LoadBitmapFromMemory(
            width, height, nullptr, D2D1_BITMAP_OPTIONS_TARGET);

        Application::APP->MainRenderer()->EndExternalEvent();
    }

    void ShadowStyle::BeginDrawOnShadow(ID2D1DeviceContext* context, const D2D1_MATRIX_3X2_F& transform)
    {
        context->SetTarget(shadowBitmap.Get());

        context->BeginDraw();
        context->SetTransform(D2D1::Matrix3x2F::Identity());

        context->Clear({ 0.0f, 0.0f, 0.0f, 0.0f });
    }

    void ShadowStyle::EndDrawOnShadow(ID2D1DeviceContext* context)
    {
        context->EndDraw();
    }

    void ShadowStyle::ConfigShadowEffectInput(ID2D1Effect* effect)
    {
        effect->SetInput(0, shadowBitmap.Get());

        effect->SetValue(D2D1_SHADOW_PROP_BLUR_STANDARD_DEVIATION, shadowStandardDeviation);
        effect->SetValue(D2D1_SHADOW_PROP_COLOR, shadowColor);
        effect->SetValue(D2D1_SHADOW_PROP_OPTIMIZATION, shadowOptimization);
    }
}