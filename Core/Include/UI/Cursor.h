#pragma once

#include "Precompile.h"

#include "Panel.h"

namespace d14engine::ui
{
    struct Cursor : Panel
    {
        enum class IconIndex
        {
            Arrow,
            Hand,
            Move,
            Ibeam,
            HorzSize,
            VertSize,
            MainDiagSize,
            BackDiagSize,
            Count
        };

        struct Icon
        {
            ComPtr<ID2D1Bitmap1> bitmap;
            D2D1_POINT_2F displayOffset = { 0.0f, 0.0f };
        };

        using IconMap = std::unordered_map<IconIndex, Icon>;

        static IconMap LoadBasicIconSeries();

        Cursor(const IconMap& icons = {}, const D2D1_RECT_F& rect = { 0.0f, 0.0f, 32.0f, 32.0f });

        void RegisterIcon(IconIndex index, const Icon& icon);
        void RegisterIcon(WstrParam name, const Icon& icon);

        void SetIcon(IconIndex index);
        void SetIcon(WstrParam name);

    protected:
        using IconArray = std::array<Icon, (size_t)IconIndex::Count>;

        IconArray m_basicIcons;

        using IconLibrary = std::unordered_map<Wstring, Icon>;

        IconLibrary m_iconLibrary;

        D2D1_POINT_2F m_displayOffset = { 0.0f, 0.0f };

    public:
        // Override interface methods.

        // IDrawObject2D
        void OnRendererDrawD2D1Object(Renderer* rndr) override;
    };
}