#include "Precompile.h"

#include "UI/ElevatedButton.h"

#include "UI/Application.h"

namespace d14engine::ui
{
    ElevatedButton::ElevatedButton(
        WstrParam text,
        const D2D1_RECT_F& rect,
        float roundRadius,
        ComPtrParam<ID2D1Bitmap1> icon,
        const D2D1_COLOR_F& normalColor,
        const D2D1_COLOR_F& activeColor)
        :
        FilledButton(text, rect, roundRadius, icon, normalColor, activeColor),
        ShadowStyle((UINT)Width(), (UINT)Height(), 1.0f) { }

    void ElevatedButton::OnRendererDrawD2D1Layer(Renderer* rndr)
    {
        BeginDrawOnShadow(rndr->d2d1DeviceContext.Get());

        UIResu::SOLID_COLOR_BRUSH->SetOpacity(shadowColorOpaque);

        rndr->d2d1DeviceContext->FillRoundedRectangle(
            { SelfCoordRect(), roundRadiusX, roundRadiusY }, UIResu::SOLID_COLOR_BRUSH.Get());

        EndDrawOnShadow(rndr->d2d1DeviceContext.Get());
    }

    void ElevatedButton::OnRendererDrawD2D1Object(Renderer* rndr)
    {
        // Shadow
        UIResu::SHADOW_EFFECT->SetInput(0, m_shadowBitmap.Get());

        UIResu::SHADOW_EFFECT->SetValue(D2D1_SHADOW_PROP_BLUR_STANDARD_DEVIATION, shadowStandardDeviation);
        UIResu::SHADOW_EFFECT->SetValue(D2D1_SHADOW_PROP_COLOR, shadowColor);
        UIResu::SHADOW_EFFECT->SetValue(D2D1_SHADOW_PROP_OPTIMIZATION, shadowOptimization);

        rndr->d2d1DeviceContext->DrawImage(UIResu::SHADOW_EFFECT.Get(), AbsolutePosition());

        // Foreground
        FilledButton::OnRendererDrawD2D1Object(rndr);
    }

    void ElevatedButton::OnSizeHelper(SizeEvent& e)
    {
        Panel::OnSizeHelper(e);

        LoadShadowBitmap((UINT)Width(), (UINT)Height());
    }

    bool ElevatedButton::OnMouseButtonHelper(MouseButtonEvent& e)
    {
        if (e.status.LeftDown())
        {
            shadowColorOpaque = 0.0f;
        }
        else if (e.status.LeftUp())
        {
            shadowColorOpaque = 1.0f;
        }
        return FilledButton::OnMouseButtonHelper(e);
    }

    bool ElevatedButton::OnMouseLeaveHelper(MouseLeaveEvent& e)
    {
        shadowColorOpaque = 1.0f;

        return FilledButton::OnMouseLeaveHelper(e);
    }
}