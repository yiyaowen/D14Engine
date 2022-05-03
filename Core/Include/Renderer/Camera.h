#pragma once

#include "Precompile.h"

#include "ICamera.h"

#include "FrameResource.h"
#include "GpuBuffer.h"
#include "MathUtils.h"

namespace d14engine::renderer
{
    struct Camera : IMainCamera
    {
        // Implement interface methods.

        // IDrawObject
        bool IsD3D12ObjectVisible() override;

        void SetD3D12ObjectVisible(bool value) override;

        void OnRendererUpdateObject(Renderer* rndr) override;

        void OnRendererDrawD3D12Object(Renderer* rndr) override;

        // ICamera
        void OnViewResize(UINT viewWidth, UINT viewHeight) override;

        D3D12_VIEWPORT Viewport() override { return viewport; }

        D3D12_RECT ScissorRect() override { return scissorRect; }

        // Implement struct methods.

        Camera(Renderer* rndr);

        bool isVisible = true;

        XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };

        XMFLOAT3 right = { 1.0f, 0.0f, 0.0f };
        XMFLOAT3 up = { 0.0f, 1.0f, 0.0f };
        XMFLOAT3 look = { 0.0f, 0.0f, 1.0f };

        XMFLOAT4X4 viewTrans = Mathu::IdentityFloat4x4();

        void UpdateViewTrans();

        // Field of view (in radian).
        float fovAngle = XM_PIDIV4;
        // Distance to near plane of view frustum.
        float nearZ = 1.0f;
        // Distance to far plane of view frustum.
        float farZ = 1000.0f;

        D3D12_VIEWPORT viewport;
        D3D12_RECT scissorRect;

        XMFLOAT4X4 projTrans = Mathu::IdentityFloat4x4();

        void UpdateProjTrans();

        struct Data // Mapped to HLSL cbuffer.
        {
            // Mapped to viewTrans.
            XMFLOAT4X4 viewMatrix = Mathu::IdentityFloat4x4();
            // Mapped to projTrans.
            XMFLOAT4X4 projMatrix = Mathu::IdentityFloat4x4();
        }
        data;

        UINT rootParamIndex = 0;
        SharedPtr<ConstantBuffer> buffer;

        // Indicate whether the buffer need to update in this frame.
        // Set as full count to update the buffer automatically at initialization.
        UINT dirtyFrameCount = FrameResource::CIRCLE_COUNT;

        // How fast the camera moves (m/s).
        float moveSpeed = 1.0f;

        // Ratio of camera rotation and mouse movement (rad/pixel).
        float rotateScale = XM_PIDIV2 / 800.0f;

        // Ratio of camera zoom distance and wheel scroll (m/delta).
        float zoomScale = 0.1f;

        void Translate(float dx, float dy, float dz);
        // Call this method in every frame and update directions when key messages received.
        void TranslateIndirect(float dirX, float dirY, float dirZ, double deltaSecs);

        void Rotate(float pitch, float yaw, float roll);
        void RotateIndirect(float mouseDeltaX, float mouseDeltaY);

        void Zoom(float distance);
        // @param wheelDelta: count of WHEEL_DELTA.
        void ZoomIndirect(float wheelDelta);
    };
}