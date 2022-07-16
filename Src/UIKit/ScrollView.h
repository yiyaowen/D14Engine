#pragma once

#include "Common/Precompile.h"

#include "UIKit/MaskStyle.h"
#include "UIKit/ResizablePanel.h"
#include "UIKit/SolidStyle.h"
#include "UIKit/StrokeStyle.h"

namespace d14engine::uikit
{
	struct ScrollView : ResizablePanel
	{
		ScrollView(const D2D1_RECT_F& rect, ShrdPtrParam<Panel> content = nullptr);

		float deltaPixelsPerScroll = 30.0f;

		void OnInitializeFinish() override;

		MaskStyle mask = { 0, 0 };
		SolidStyle background = {};
		StrokeStyle stroke = {};

	public:
		SharedPtr<Panel> Content();
		void SetContent(ShrdPtrParam<Panel> content);

		enum class ScrollBarState { Idle, Hover, Down, Count };

		struct ScrollBarAppearance
		{
			float internalSize = {};
			float roundRadius = {};
			float externalOffset = {};

			D2D1_COLOR_F color = {};
			float opacity = {};
		}
		scrollBarAppearances[(size_t)ScrollBarState::Count] = {};

		const D2D1_POINT_2F& ViewportOffset();
		void SetViewportOffset(const D2D1_POINT_2F& absolute);

		D2D1_POINT_2F ViewportOffsetPercentage();
		// Pass through a pair of percentage values range from 0 to 1.
		void SetViewportOffsetPercentage(const D2D1_POINT_2F& relative);

	protected:
		SharedPtr<Panel> m_content = {};

		// Store mouse-button-down point's offset in self-coordinate.
		// These fields will be used to update viewport offset later.
		float m_horzBarHoldOffset = 0.0f, m_vertBarHoldOffset = 0.0f;

		D2D1_POINT_2F m_viewportOffset = { 0.0f, 0.0f };
		// Used to update viewport offset in mouse-move event.
		D2D1_POINT_2F m_originalViewportOffset = { 0.0f, 0.0f };

		bool m_isHorzBarHover = false, m_isHorzBarDown = false;
		bool m_isVertBarHover = false, m_isVertBarDown = false;

		D2D1_RECT_F HorzBarRect(ScrollBarState state);
		D2D1_RECT_F VertBarRect(ScrollBarState state);
		
	protected:
		// Override interface methods.

		// IDrawObject2D
		void OnRendererDrawD2D1LayerHelper(Renderer* rndr) override;

		void OnRendererDrawD2D1ObjectHelper(Renderer* rndr) override;

		// Panel
		void OnSizeHelper(SizeEvent& e) override;

		void OnChangeThemeHelper(WstrViewParam themeName) override;

		bool OnMouseButtonHelper(MouseButtonEvent& e) override;

		bool OnMouseMoveHelper(MouseMoveEvent& e) override;

		bool OnMouseLeaveHelper(MouseMoveEvent& e) override;

		bool OnMouseWheelHelper(MouseWheelEvent& e) override;
	};
}