#include "Common/Precompile.h"

#include "Renderer/Camera.h"

#include "Renderer/Renderer.h"

namespace d14engine::renderer
{
    bool Camera::IsD3D12ObjectVisible()
    {
        return isVisible;
    }

    void Camera::SetD3D12ObjectVisible(bool value)
    {
        isVisible = value;
    }

    void Camera::OnRendererUpdateObject(Renderer* rndr)
    {
        if (dirtyFrameCount > 0)
        {
            --dirtyFrameCount;
            buffer->CopyData(0, &data, sizeof(data));
        }
    }

    void Camera::OnRendererDrawD3D12Object(Renderer* rndr)
    {
        rndr->cmdList->SetGraphicsRootConstantBufferView(
            rootParamIndex, buffer->resource->GetGPUVirtualAddress());
    }

    void Camera::OnViewResize(UINT viewWidth, UINT viewHeight)
    {
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;
        viewport.Width = (FLOAT)viewWidth;
        viewport.Height = (FLOAT)viewHeight;
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;

        scissorRect.left = 0;
        scissorRect.right = viewWidth;
        scissorRect.top = 0;
        scissorRect.bottom = viewHeight;

        UpdateProjTrans();
    }

    Camera::Camera(Renderer* rndr)
    {
        buffer = std::make_shared<ConstantBuffer>(rndr->device.Get(), 1, sizeof(data));

        OnViewResize(rndr->SceneWidth(), rndr->SceneHeight());
        UpdateViewTrans();
        // Projection transform matrix has already been updated in OnViewResize.
    }

    void Camera::Translate(float dx, float dy, float dz)
    {
        position.x += dx;
        position.y += dy;
        position.z += dz;

        UpdateViewTrans();
    }

    void Camera::TranslateIndirect(float dirX, float dirY, float dirZ, double deltaSecs)
    {
        float dist = moveSpeed * (float)deltaSecs;
        Translate(dirX * dist, dirY * dist, dirZ * dist);
    }

    void Camera::Rotate(float pitch, float yaw, float roll)
    {
        auto rotmat = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

        XMStoreFloat3(&right, XMVector3Transform(XMLoadFloat3(&right), rotmat));
        XMStoreFloat3(&up, XMVector3Transform(XMLoadFloat3(&up), rotmat));
        XMStoreFloat3(&look, XMVector3Transform(XMLoadFloat3(&look), rotmat));

        UpdateViewTrans();
    }

    void Camera::RotateIndirect(float mouseDeltaX, float mouseDeltaY)
    {
        // Note mouse move along x-axis is mapped to rotate along y-axis and vice versa.
        Rotate(mouseDeltaY * rotateScale, mouseDeltaX * rotateScale, 0.0f);
    }

    void Camera::Zoom(float distance)
    {
        Translate(look.x * distance, look.y * distance, look.z * distance);
    }

    void Camera::ZoomIndirect(float wheelDelta)
    {
        Zoom(wheelDelta * zoomScale);
    }

    void Camera::UpdateViewTrans()
    {
        // Note DirectX 3D uses the left-handed coordinate system.
        // X-axis extends from left to right, Y-axis from down to top and Z-axis from outside to inside.

        XMVECTOR P = XMLoadFloat3(&position);

        XMVECTOR R = XMLoadFloat3(&right);
        XMVECTOR U = XMLoadFloat3(&up);
        XMVECTOR L = XMLoadFloat3(&look);

        // We have to do this since directions may not be normalized or orthometric (float error when rotate).
        // Note we also hardcode y-value of right direction to zero to avoid unexpected tilt in roll direction.
        R = XMVector3Normalize(XMVectorSetY(R, 0.0f));
        L = XMVector3Normalize(XMVector3Cross(R, U));
        U = XMVector3Cross(L, R);

        // Start recompute view transform matrix.

        float x = -XMVectorGetX(XMVector3Dot(P, R));
        float y = -XMVectorGetX(XMVector3Dot(P, U));
        float z = -XMVectorGetX(XMVector3Dot(P, L));

        XMStoreFloat3(&right, R);
        XMStoreFloat3(&up, U);
        XMStoreFloat3(&look, L);

        viewTrans(0, 0) = right.x;
        viewTrans(1, 0) = right.y;
        viewTrans(2, 0) = right.z;
        viewTrans(3, 0) = x;

        viewTrans(0, 1) = up.x;
        viewTrans(1, 1) = up.y;
        viewTrans(2, 1) = up.z;
        viewTrans(3, 1) = y;

        viewTrans(0, 2) = look.x;
        viewTrans(1, 2) = look.y;
        viewTrans(2, 2) = look.z;
        viewTrans(3, 2) = z;

        viewTrans(0, 3) = 0.0f;
        viewTrans(1, 3) = 0.0f;
        viewTrans(2, 3) = 0.0f;
        viewTrans(3, 3) = 1.0f;
    }

    void Camera::UpdateProjTrans()
    {
        XMStoreFloat4x4(&projTrans, XMMatrixPerspectiveFovLH(fovAngle, viewport.Width / viewport.Height, nearZ, farZ));
    }
}