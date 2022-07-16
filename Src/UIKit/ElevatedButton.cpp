#include "Common/Precompile.h"

#include "UIKit/ElevatedButton.h"

#include "Renderer/MathUtils.h"
using namespace d14engine::renderer;

namespace d14engine::uikit
{
    ElevatedButton::ElevatedButton(
        WstrParam text,
        const D2D1_RECT_F& rect,
        float roundRadius,
        ComPtrParam<ID2D1Bitmap1> icon)
        :
        Panel(rect, Resu::SOLID_COLOR_BRUSH),
        FilledButton(text, rect, roundRadius, icon),
        shadow(Mathu::Rounding(Width()), Mathu::Rounding(Height()))
    {
        // TODO: add shadow area self-adaption.
        shadow.standardDeviation = 5.0f;
    }

    void ElevatedButton::OnRendererDrawD2D1LayerHelper(Renderer* rndr)
    {
        FilledButton::OnRendererDrawD2D1LayerHelper(rndr);

        shadow.BeginDrawOnShadow(rndr->d2d1DeviceContext.Get());
        {
            Resu::SOLID_COLOR_BRUSH->SetOpacity(shadow.opacity);

            rndr->d2d1DeviceContext->FillRoundedRectangle(
                {
                    // TODO: add shadow size self-adaption.
                    Mathu::MoveVertex(SelfCoordRect(), { 3.0f, 5.0f, -3.0f, -1.0f }),
                    roundRadiusX, roundRadiusY
                },
                Resu::SOLID_COLOR_BRUSH.Get());
        }
        shadow.EndDrawOnShadow(rndr->d2d1DeviceContext.Get());
    }

    void ElevatedButton::OnRendererDrawD2D1ObjectHelper(Renderer* rndr)
    {
        // Shadow
        shadow.ConfigShadowEffectInput(Resu::SHADOW_EFFECT.Get());

        rndr->d2d1DeviceContext->DrawImage(Resu::SHADOW_EFFECT.Get(), AbsolutePosition());

        // Foreground
        FilledButton::OnRendererDrawD2D1ObjectHelper(rndr);
    }

    void ElevatedButton::OnSizeHelper(SizeEvent& e)
    {
        FilledButton::OnSizeHelper(e);

        shadow.LoadShadowBitmap(Mathu::Rounding(e.size.width), Mathu::Rounding(e.size.height));
    }

    void ElevatedButton::OnChangeThemeHelper(WstrViewParam themeName)
    {
        FilledButton::OnChangeThemeHelper(themeName);

        // TODO: add shadow color self-adaption.
        if (themeName == L"Light")
        {
            shadow.color = D2D1::ColorF{ 0xb2b2b2 };
        }
        else if (themeName == L"Dark")
        {
            shadow.color = D2D1::ColorF{ 0x000000 };
        }
    }

    bool ElevatedButton::OnMouseButtonHelper(MouseButtonEvent& e)
    {
        if (e.status.LeftDown())
        {
            shadow.opacity = 0.0f;
        }
        else if (e.status.LeftUp())
        {
            shadow.opacity = 1.0f;
        }
        return FilledButton::OnMouseButtonHelper(e);
    }

    bool ElevatedButton::OnMouseLeaveHelper(MouseMoveEvent& e)
    {
        shadow.opacity = 1.0f;

        return FilledButton::OnMouseLeaveHelper(e);
    }

    void ElevatedButton::SetEnabled(bool value)
    {
        FilledButton::SetEnabled(value);

        shadow.opacity = value ? 1.0f : 0.0f;
    }
}