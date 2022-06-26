#include "Precompile.h"

#include "UI/ElevatedButton.h"

#include "Renderer/MathUtils.h"
using namespace d14engine::renderer;

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
        ShadowStyle((UINT)Width(), (UINT)Height(), 3.0f, { 0.8f, 0.8f, 0.8f, 1.0f }) { }

    void ElevatedButton::OnRendererDrawD2D1LayerHelper(Renderer* rndr)
    {
        FilledButton::OnRendererDrawD2D1LayerHelper(rndr);

        BeginDrawOnShadow(rndr->d2d1DeviceContext.Get());

        UIResu::SOLID_COLOR_BRUSH->SetOpacity(shadowOpacity);

        rndr->d2d1DeviceContext->FillRoundedRectangle(
            { SelfCoordRect(), roundRadiusX, roundRadiusY }, UIResu::SOLID_COLOR_BRUSH.Get());

        EndDrawOnShadow(rndr->d2d1DeviceContext.Get());
    }

    void ElevatedButton::OnRendererDrawD2D1ObjectHelper(Renderer* rndr)
    {
        // Shadow
        ConfigShadowEffectInput(UIResu::SHADOW_EFFECT.Get());

        rndr->d2d1DeviceContext->DrawImage(
            UIResu::SHADOW_EFFECT.Get(), Mathu::Offset(AbsolutePosition(), { 0.0f, 1.0f }));

        // Foreground
        FilledButton::OnRendererDrawD2D1ObjectHelper(rndr);
    }

    void ElevatedButton::OnSizeHelper(SizeEvent& e)
    {
        Panel::OnSizeHelper(e);

        LoadShadowBitmap((UINT)(Width() + 0.5f), (UINT)(Height() + 0.5f));
    }

    void ElevatedButton::OnChangeThemeHelper(WstrViewParam themeName)
    {
        FilledButton::OnChangeThemeHelper(themeName);

        if (themeName == L"Light")
        {
            shadowColor = { 0.8f, 0.8f, 0.8f, 1.0f };
            shadowOpacity = 1.0f;

        }
        else if (themeName == L"Dark")
        {
            shadowColor = { 0.2f, 0.2f, 0.2f, 1.0f };
            shadowOpacity = 1.0f;
        }
    }

    bool ElevatedButton::OnMouseButtonHelper(MouseButtonEvent& e)
    {
        if (e.status.LeftDown())
        {
            shadowOpacity = 0.0f;
        }
        else if (e.status.LeftUp())
        {
            shadowOpacity = 1.0f;
        }
        return FilledButton::OnMouseButtonHelper(e);
    }

    bool ElevatedButton::OnMouseLeaveHelper(MouseMoveEvent& e)
    {
        shadowOpacity = 1.0f;

        return FilledButton::OnMouseLeaveHelper(e);
    }
}