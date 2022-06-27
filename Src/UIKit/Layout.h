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
    struct Layout : ResizablePanel, SolidStyle, StrokeStyle
    {
        explicit Layout(const D2D1_RECT_F& rect)
            :
            ResizablePanel(rect, Resu::SOLID_COLOR_BRUSH),
            SolidStyle({ 0.9f, 0.9f, 0.9f, 1.0f }),
            StrokeStyle(1.0f, { 0.8f, 0.8f, 0.8f, 1.0f })
        {
            SetResizable(false);
        }
        using GeometryInfo = GeometryInfo_T;

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

    protected:
        using ElementGeometryInfoMap = std::map<WeakPtr<Panel>, GeometryInfo_T, std::owner_less<WeakPtr<Panel>>>;

        ElementGeometryInfoMap m_elemGeoInfos = {};

        virtual void UpdateElement(typename ElementGeometryInfoMap::iterator& elemItor) = 0;

    public:
        // Override interface methods.

        // IDrawObject2D
        void OnRendererDrawD2D1ObjectHelper(Renderer* rndr) override
        {
            // Background
            Resu::SOLID_COLOR_BRUSH->SetColor(backgroundColor);
            Resu::SOLID_COLOR_BRUSH->SetOpacity(backgroundOpacity);

            Panel::DrawBackground(rndr);

            // Outline
            Resu::SOLID_COLOR_BRUSH->SetColor(strokeColor);
            Resu::SOLID_COLOR_BRUSH->SetOpacity(strokeOpacity);

            auto innerRect = Mathu::Stretch(m_absoluteRect, { -strokeWidth * 0.5f, -strokeWidth * 0.5f });

            rndr->d2d1DeviceContext->DrawRoundedRectangle(
                { innerRect, roundRadiusX, roundRadiusY }, Resu::SOLID_COLOR_BRUSH.Get(), strokeWidth);
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
                backgroundColor = { 0.9f, 0.9f, 0.9f, 1.0f };

                strokeWidth = 1.0f;
                strokeColor = { 0.8f, 0.8f, 0.8f, 1.0f };
            }
            else if (themeName == L"Dark")
            {
                backgroundColor = { 0.12f, 0.12f, 0.12f, 1.0f };

                strokeWidth = 1.0f;
                strokeColor = { 0.1f, 0.1f, 0.1f, 1.0f };
            }
        }
    };
}