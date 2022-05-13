#include "Precompile.h"

#include "UI/AnimTextBox.h"

#include "UI/Application.h"

namespace d14engine::ui
{
    AnimTextBox::AnimTextBox(const D2D1_RECT_F& rect, float roundRadius)
        :
        TextBox(rect, roundRadius),
        StrokeStyle(1.5f, { 0.8f, 0.8f, 0.8f, 1.0f })
    {
        solidColor = { 0.95f, 0.95f, 0.95f, 1.0f };
    }

    void AnimTextBox::OnRendererUpdateObject2D(Renderer* rndr)
    {
        TextBox::OnRendererUpdateObject2D(rndr);

        auto deltaSecs = (float)rndr->timer->deltaSecs;

        // Bottom line dash animation.
        //
        //       acceleration             uniform              deceleration
        // start -----------> waypoint 1 ----------> waypoint2 -----------> end
        //          motion                 motion                 motion
        //
        //  s --- total distance
        //  v --- uniform speed
        //  a --- ac/deceleration
        // t1 --- variable speed motion time, i.e. start to waypoint 1 (waypoint 2 to end)
        // t2 --- uniform speed motion time, i.e. waypoint 1 to waypoint 2.
        // 
        // v = s / (t1 + t2)
        // 
        // Then simple to get the distance between start and waypoint 1:
        // 
        // s1 = v * t1 / 2 = s * t1 / [2 * (t1 + t2)]
        // 
        // We can compute the variable speed (between start and waypoint 1) from current position:
        // 
        // v1 = sqrt[ (2 * s * x) / (t1 * (t1 + t2)) ]
        {
            float s = VisibleTextWidth();
            float t1 = bottomLineVariableSpeedSecs;
            float t2 = bottomLineUniformSpeedSecs;

            if (m_currBottomLineLength < s)
            {
                float s1 = s * t1 / (2.0f * (t1 + t2));

                // Set uniform speed as default value; also note the formula of variable speed
                // contains this item, so we can reuse it in place to reduce some calculation.
                float currSpeed = s / (t1 + t2);

                // Acceleration motion
                if (m_currBottomLineLength < s1)
                {
                    // We must add a small increment (0.1f) to push it moving;
                    // otherwise it will always stay at the starting position.
                    currSpeed = sqrt(2.0f * currSpeed * (m_currBottomLineLength + 0.1f) / t1);
                }
                // Deceleration motion, symmetrical with acceleration.
                else if (m_currBottomLineLength > (s - s1))
                {
                    // Do NOT repeat the above trick here (i.e. add a small increment),
                    // since that might cause passing through a negative value to sqrt.
                    currSpeed = sqrt(2.0f * currSpeed * (s - m_currBottomLineLength) / t1);
                }
                m_currBottomLineLength += deltaSecs * currSpeed;
            }
            m_currBottomLineLength = std::clamp(m_currBottomLineLength, 0.0f, s);
        }
    }

    void AnimTextBox::OnRendererDrawD2D1Object(Renderer* rndr)
    {
        TextBox::OnRendererDrawD2D1Object(rndr);

        // Frame
        UIResu::SOLID_COLOR_BRUSH->SetColor(strokeColor);
        UIResu::SOLID_COLOR_BRUSH->SetOpacity(strokeColorOpaque);

        rndr->d2d1DeviceContext->DrawRoundedRectangle(
            { m_absoluteRect, roundRadiusX, roundRadiusY }, UIResu::SOLID_COLOR_BRUSH.Get(), strokeWidth);

        // Bottom Line
        {
            UIResu::SOLID_COLOR_BRUSH->SetOpacity(1.0f);

            D2D1_POINT_2F left, right;

            auto visibleTextAreaLeftTop = VisibleTextAbsolutePosition();

            // Static One
            UIResu::SOLID_COLOR_BRUSH->SetColor(staticBottomLineColor);

            left = { visibleTextAreaLeftTop.x, visibleTextAreaLeftTop.y + VisibleTextHeight() + staticBottomLineOffsetY };
            right = { left.x + VisibleTextWidth(), left.y};

            rndr->d2d1DeviceContext->DrawLine(left, right, UIResu::SOLID_COLOR_BRUSH.Get(), staticBottomLineStrokeWidth);

            if (m_currBottomLineLength > 0.0f)
            {
                // Dynamic One
                UIResu::SOLID_COLOR_BRUSH->SetColor(bottomLineColor);

                left = { visibleTextAreaLeftTop.x, visibleTextAreaLeftTop.y + VisibleTextHeight() + bottomLineOffsetY};
                right = { left.x + m_currBottomLineLength, left.y };

                rndr->d2d1DeviceContext->DrawLine(left, right, UIResu::SOLID_COLOR_BRUSH.Get(), bottomLineStrokeWidth);
            }
        }
    }

    bool AnimTextBox::OnGetFocusHelper()
    {
        solidColor = { 0.98f, 0.98f, 0.98f, 1.0f };

        Application::IncreaseAnimateCount();

        return TextBox::OnGetFocusHelper();
    }

    bool AnimTextBox::OnLoseFocusHelper()
    {
        solidColor = { 0.95f, 0.95f, 0.95f, 1.0f };
        staticBottomLineColor = { 0.7f, 0.7f, 0.7f, 1.0f };

        m_currBottomLineLength = 0.0f;

        Application::DecreaseAnimateCount();

        return TextBox::OnLoseFocusHelper();
    }

    bool AnimTextBox::OnMouseEnterHelper(MouseEnterEvent& e)
    {
        if (!Application::APP->IsUIObjectFocused(weak_from_this()))
        {
            solidColor = { 0.92f, 0.92f, 0.92f, 1.0f };
            staticBottomLineColor = { 0.5f, 0.5f, 0.5f, 1.0f };
        }
        return Panel::OnMouseEnterHelper(e);
    }

    bool AnimTextBox::OnMouseLeaveHelper(MouseLeaveEvent& e)
    {
        if (!Application::APP->IsUIObjectFocused(weak_from_this()))
        {
            solidColor = { 0.95f, 0.95f, 0.95f, 1.0f };
            staticBottomLineColor = { 0.7f, 0.7f, 0.7f, 1.0f };
        }
        return Panel::OnMouseLeaveHelper(e);
    }
}