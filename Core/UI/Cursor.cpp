#include "Precompile.h"

#include "UI/Cursor.h"

#include "Renderer/MathUtils.h"
#include "UI/Application.h"

namespace d14engine::ui
{
    Cursor::IconMap Cursor::LoadBasicIconSeries()
    {
#define PUSH_ICON_PATH(Name, ...) { Cursor::IconIndex::Name, L#Name L".png ", __VA_ARGS__ }

        std::tuple<Cursor::IconIndex, Wstring, D2D1_POINT_2F> iconPaths[] =
        {
            PUSH_ICON_PATH(Arrow, { 0.0f, 0.0f }),
            PUSH_ICON_PATH(Hand, { -6.0f, 0.0f }),
            PUSH_ICON_PATH(Move, { -16.0f, -16.0f }),
            PUSH_ICON_PATH(Ibeam, { 0.0f, 0.0f }),
            PUSH_ICON_PATH(HorzSize, { -16.0f, -16.0f }),
            PUSH_ICON_PATH(VertSize, { -16.0f, -16.0f }),
            PUSH_ICON_PATH(MainDiagSize, { -16.0f, -16.0f }),
            PUSH_ICON_PATH(BackDiagSize, { -16.0f, -16.0f })
        };

#undef PUSH_ICON_PATH

        Cursor::IconMap icons = {};
        for (auto& path : iconPaths)
        {
            auto& icon = (icons[std::get<0>(path)] = {});

            icon.bitmap = Bitmapu::LoadBitmapFromFile(
                std::get<1>(path),
                Application::RNDR->commonInfo.assetsPath + L"Images/Cursors/WinClassics/");

            icon.displayOffset = std::get<2>(path);
        }

        return icons;
    }

    Cursor::Cursor(const IconMap& icons, const D2D1_RECT_F& rect) : Panel(rect)
    {
        for (auto& i : icons)
        {
            m_basicIcons[(size_t)i.first] = i.second;
        }
        // Ensure the cursor always shown at the top.
        ISortable<IDrawObject2D>::m_priority = INT_MAX;
    }

    void Cursor::RegisterIcon(IconIndex index, const Icon& icon)
    {
        m_basicIcons[(size_t)index] = icon;
    }

    void Cursor::RegisterIcon(WstrParam name, const Icon& icon)
    {
        m_iconLibrary[name] = icon;
    }

    void Cursor::SetIcon(IconIndex index)
    {
        auto& icon = m_basicIcons[(size_t)index];

        bitmap = icon.bitmap;
        m_displayOffset = icon.displayOffset;
    }

    void Cursor::SetIcon(WstrParam name)
    {
        auto& icon = m_iconLibrary.at(name);

        bitmap = icon.bitmap;
        m_displayOffset = icon.displayOffset;
    }

    void Cursor::OnRendererDrawD2D1Object(Renderer* rndr)
    {
        if (bitmap != nullptr)
        {
            rndr->d2d1DeviceContext->DrawBitmap(
                bitmap.Get(), Mathu::Offset(m_absoluteRect, m_displayOffset));
        }
    }
}