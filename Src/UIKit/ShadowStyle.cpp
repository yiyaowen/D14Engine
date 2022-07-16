#include "Common/Precompile.h"

#include "UIKit/ShadowStyle.h"

#include "UIKit/Application.h"
#include "UIKit/BitmapUtils.h"
#include "UIKit/ResourceUtils.h"

namespace d14engine::uikit
{
    ShadowStyle::ShadowStyle(
        UINT bitmapWidth,
        UINT bitmapHeight,
        D2D1_COLOR_F color,
        float standardDeviation,
        D2D1_SHADOW_OPTIMIZATION optimization)
        :
        color(color),
        standardDeviation(standardDeviation),
        optimization(optimization)
    {
        LoadShadowBitmap(bitmapWidth, bitmapHeight);
    }

    void ShadowStyle::LoadShadowBitmap(UINT width, UINT height)
    {
        Resu::SHADOW_EFFECT->SetInput(0, nullptr);
        Application::APP->MainRenderer()->BeginExternalEvent();

        bitmap = Bitmapu::LoadBitmapFromMemory(
            width, height, nullptr, D2D1_BITMAP_OPTIONS_TARGET);

        Application::APP->MainRenderer()->EndExternalEvent();
    }

    void ShadowStyle::BeginDrawOnShadow(ID2D1DeviceContext* context, const D2D1_MATRIX_3X2_F& transform)
    {
        context->SetTarget(bitmap.Get());

        context->BeginDraw();
        context->SetTransform(D2D1::Matrix3x2F::Identity());

        context->Clear(D2D1::ColorF{ 0x000000, 0.0f });
    }

    void ShadowStyle::EndDrawOnShadow(ID2D1DeviceContext* context)
    {
        context->EndDraw();
    }

    void ShadowStyle::ConfigShadowEffectInput(ID2D1Effect* effect)
    {
        effect->SetInput(0, bitmap.Get());

        effect->SetValue(D2D1_SHADOW_PROP_BLUR_STANDARD_DEVIATION, standardDeviation);
        effect->SetValue(D2D1_SHADOW_PROP_COLOR, color);
        effect->SetValue(D2D1_SHADOW_PROP_OPTIMIZATION, optimization);
    }
}