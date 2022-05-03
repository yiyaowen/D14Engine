#include "Precompile.h"

#include "UI/ElevatedButton.h"

#include "Renderer/MathUtils.h"
#include "UI/Application.h"

namespace d14engine::ui
{
    ElevatedButton::ElevatedButton(
        WstrParam text,
        ComPtrParam<ID2D1Bitmap1> icon,
        const D2D1_RECT_F& rect,
        float roundRadius)
        :
        FilledButton(text, icon, rect, roundRadius),
        ShadowStyle((UINT)Width(), (UINT)Height(), 0, 1.0f) { }

    void ElevatedButton::OnRendererDrawD2D1Layer(Renderer* rndr)
    {
        rndr->d2d1DeviceContext->SetTarget(m_shadowBitmap.Get());
        rndr->d2d1DeviceContext->BeginDraw();

        UIResu::SOLID_COLOR_BRUSH->SetOpacity(1.0f);

        rndr->d2d1DeviceContext->FillRoundedRectangle(
            { SelfCoordRect(), m_radiusX, m_radiusY }, UIResu::SOLID_COLOR_BRUSH.Get());

        rndr->d2d1DeviceContext->EndDraw();
    }

    void ElevatedButton::OnRendererDrawD2D1Object(Renderer* rndr)
    {
        // Shadow
        UIResu::SHADOW_EFFECT->SetInput(0, m_shadowBitmap.Get());

        UIResu::SHADOW_EFFECT->SetValue(D2D1_SHADOW_PROP_BLUR_STANDARD_DEVIATION, shadowStandardDeviation);
        UIResu::SHADOW_EFFECT->SetValue(D2D1_SHADOW_PROP_COLOR, shadowColor);
        UIResu::SHADOW_EFFECT->SetValue(D2D1_SHADOW_PROP_OPTIMIZATION, shadowOptimization);

        rndr->d2d1DeviceContext->DrawImage(
            UIResu::SHADOW_EFFECT.Get(), Mathu::Offset(AbsolutePosition(), { 0.0f, 2.0f }));

        // Background & Text
        FilledButton::OnRendererDrawD2D1Object(rndr);
    }

    void ElevatedButton::OnSizeHelper(SizeEvent& e)
    {
        Panel::OnSizeHelper(e);

        LoadShadowBitmap((UINT)Width(), (UINT)Height(), 0);
    }
}