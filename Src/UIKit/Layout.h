#pragma once

#include "Common/Precompile.h"

#include "Renderer/MathUtils.h"
using namespace d14engine::renderer;

#include "UIKit/ResizablePanel.h"
#include "UIKit/ResourceUtils.h"
#include "UIKit/SolidStyle.h"
#include "UIKit/StrokeStyle.h"

namespace d14engine::uikit
{
    template<typename GeometryInfo_T>
    struct Layout : ResizablePanel
    {
        explicit Layout(const D2D1_RECT_F& rect)
            :
            Panel(rect, Resu::SOLID_COLOR_BRUSH),
            ResizablePanel(rect, Resu::SOLID_COLOR_BRUSH)
        {
            SetResizable(false);

            background.opacity = stroke.opacity = 0.0f;
        }
        using GeometryInfo = GeometryInfo_T;

        SolidStyle background = {};
        StrokeStyle stroke = {};

        void AddElement(ShrdPtrParam<Panel> elem, const GeometryInfo_T& geoInfo)
        {
            if (elem == nullptr) return;
            Panel::AddUIObject(elem);

            m_elemGeoInfos.insert({ elem, geoInfo });
            UpdateElement(elem);
        }

        void RemoveElement(ShrdPtrParam<Panel> elem)
        {
            Panel::RemoveUIObject(elem);

            m_elemGeoInfos.erase(elem);
        }

        // Compute specified element's position and size according to its geometry info
        // and then call the transform method to apply the updated position and size.
        // Note the element must have already been set as child object before updating.
        void UpdateElement(WeakPtrParam<Panel> elem)
        {
            auto elemItor = m_elemGeoInfos.find(elem);
            if (elemItor != m_elemGeoInfos.end()) UpdateElement(elemItor);
        }

        // Call UpdateElement for each element and remove expired elements by the way.
        void UpdateAllElements()
        {
            for (auto elemItor = m_elemGeoInfos.begin(); elemItor != m_elemGeoInfos.end(); )
            {
                if (elemItor->first.expired())
                {
                    elemItor = m_elemGeoInfos.erase(elemItor);
                }
                else UpdateElement(elemItor++);
            }
        }

        GeometryInfo_T* PeekElemGeoInfo(WeakPtrParam<Panel> elem)
        {
            auto elemItor = m_elemGeoInfos.find(elem);
            return (elemItor == m_elemGeoInfos.end()) ? nullptr : &elemItor->second;
        }

    protected:
        using ElementGeometryInfoMap = std::map<WeakPtr<Panel>, GeometryInfo_T, std::owner_less<WeakPtr<Panel>>>;

        ElementGeometryInfoMap m_elemGeoInfos = {};

        virtual void UpdateElement(typename ElementGeometryInfoMap::iterator& elemItor) = 0;

    protected:
        // Override interface methods.

        // IDrawObject2D
        void OnRendererDrawD2D1ObjectHelper(Renderer* rndr) override
        {
            // Background
            Resu::SOLID_COLOR_BRUSH->SetColor(background.color);
            Resu::SOLID_COLOR_BRUSH->SetOpacity(background.opacity);

            Panel::DrawBackground(rndr);

            // Outline
            Resu::SOLID_COLOR_BRUSH->SetColor(stroke.color);
            Resu::SOLID_COLOR_BRUSH->SetOpacity(stroke.opacity);

            auto innerRect = Mathu::Stretch(m_absoluteRect, { -stroke.width * 0.5f, -stroke.width * 0.5f });

            rndr->d2d1DeviceContext->DrawRoundedRectangle(
                { innerRect, roundRadiusX, roundRadiusY }, Resu::SOLID_COLOR_BRUSH.Get(), stroke.width);
        }

        // Panel
        void OnSizeHelper(SizeEvent& e) override
        {
            ResizablePanel::OnSizeHelper(e);

            UpdateAllElements();
        }

        void OnChangeThemeHelper(WstrViewParam themeName) override
        {
            ResizablePanel::OnChangeThemeHelper(themeName);

            // Don't change the opacity values in case some transparent layouts
            // display incorrectly when switch between different themems.
            if (themeName == L"Light")
            {
                background.color = D2D1::ColorF{ 0xf2f2f2 };

                stroke.width = 1.0f;
                stroke.color = D2D1::ColorF{ 0xe5e5e5 };
            }
            else if (themeName == L"Dark")
            {
                background.color = D2D1::ColorF{ 0x1f1f1f };

                stroke.width = 1.0f;
                stroke.color = D2D1::ColorF{ 0x1a1a1a };
            }
        }
    };
}