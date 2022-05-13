#include "Precompile.h"

#include "UI/ShadowStyle.h"

#include "UI/Application.h"
#include "UI/BitmapUtils.h"
#include "UI/UIResourceUtils.h"

namespace d14engine::ui
{
    ShadowStyle::ShadowStyle(
        UINT shadowBitmapWidth,
        UINT shadowBitmapHeight,
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
        LoadShadowBitmap(shadowBitmapWidth, shadowBitmapHeight);
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

    void ShadowStyle::LoadShadowBitmap(UINT width, UINT height)
    {
        UIResu::SHADOW_EFFECT->SetInput(0, nullptr);
        Application::RENDERER->BeginExternalEvent();

        m_shadowBitmap = Bitmapu::LoadBitmapFromMemory(
            width, height, nullptr, D2D1_BITMAP_OPTIONS_TARGET);

        Application::RENDERER->EndExternalEvent();
    }

    void ShadowStyle::BeginDrawOnShadow(ID2D1DeviceContext* context, const D2D1_MATRIX_3X2_F& transform)
    {
        context->SetTarget(m_shadowBitmap.Get());

        context->BeginDraw();
        context->SetTransform(D2D1::Matrix3x2F::Identity());

        context->Clear({ 0.0f, 0.0f, 0.0f, 0.0f });
    }

    void ShadowStyle::EndDrawOnShadow(ID2D1DeviceContext* context)
    {
        context->EndDraw();
    }
}