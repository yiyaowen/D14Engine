#pragma once

#include "Precompile.h"

#include "ICamera.h"
#include "IDrawLayer.h"
#include "IDrawObject.h"
#include "IDrawObject2D.h"

#include "FrameResource.h"
#include "Letterbox.h"
#include "Timer.h"

namespace d14engine::renderer
{
    struct Renderer
    {
        struct CreateInfo
        {
            // Specify the renderer's dependent assets directory.
            // The renderer will search external resources in this path at initialization.
            Wstring assetsPath = L"./Assets/";

            // Specify the renderer's GPU device adapter by index.
            // Set to 0 to use the default adapter selected by DirectX.
            UINT adapterIndex = 0;

            // Specify the clear color for scene.
            XMVECTORF32 sceneColor = Colors::Gainsboro;
            // Specify the clear color for letterbox (background of scene).
            // This field will only be used when scene resolution is different from window resolution.
            XMVECTORF32 letterboxColor = Colors::Black;

            // Whether to enable multi-sampling anti-aliasing.
            bool enableMSAA = true;
            // Specify the sample count of MSAA.
            UINT msaaSampleCount = 4;
            // Specify the sample quality level of MSAA.
            // Set to UINT_MAX to use the highest available quality level.
            UINT msaaQualityLevel = UINT_MAX;

            // Whether to enter fullscreen mode at initialization.
            bool fullscreen = false;
            // Whether to force scene to scale to the specified resolution.
            bool forceResolution = false;
            // Specify the resolution for scene.
            // This field will only be used when forceResolution is set.
            struct Resolution
            {
                UINT width = 1920;
                UINT height = 1080;
            }
            resolution;
            // Specify the refresh rate for target scene resolution.
            struct RefreshRate
            {
                UINT numerator = 60;
                UINT denominator = 1;
            }
            refreshRate;

            // Whether to allow screen tearing (Vsync/Off).
            bool allowTearing = true;
        };

        Renderer(HWND window, const CreateInfo& info = {});

        virtual ~Renderer() { FlushCmdQueue(); }

        void OnWindowResize();
        void DrawNextFrame();

        void D3D12BeginEvent() { FlushCmdQueue(); ResetCmdList(); }
        void D3D12EndEvent() { SubmitCmdList(); FlushCmdQueue(); }

        void SelectMainCamera(ShrdPtrParam<IMainCamera> camera);

        // If a camera is associated, then it will be updated in the main render pass.
        // Try to minimize the number of associated cameras to improve performance.
        void AssociateCamera(ShrdPtrParam<ICamera> camera);
        void DisassociateCamera(ShrdPtrParam<ICamera> camera);

        void AddPreDrawLayer(ShrdPtrParam<IDrawLayer> layer);
        void RemovePreDrawLayer(ShrdPtrParam<IDrawLayer> layer);

        void AddPostDrawLayer(ShrdPtrParam<IDrawLayer> layer);
        void RemovePostDrawLayer(ShrdPtrParam<IDrawLayer> layer);

        void AddPreDrawObject(ShrdPtrParam<IDrawObject> obj, ShrdPtrParam<IDrawLayer> layer);
        void RemovePreDrawObject(ShrdPtrParam<IDrawObject> obj, ShrdPtrParam<IDrawLayer> layer);

        void AddPostDrawObject(ShrdPtrParam<IDrawObject> obj, ShrdPtrParam<IDrawLayer> layer);
        void RemovePostDrawObject(ShrdPtrParam<IDrawObject> obj, ShrdPtrParam<IDrawLayer> layer);

        void AddDrawObject2D(ShrdPtrParam<IDrawObject2D> obj2d);
        void RemoveDrawObject2D(ShrdPtrParam<IDrawObject2D> obj2d);

        // This field will be assigned as the origin create info passed in ctor.
        // The program shares these config fields during the renderer's lifecycle,
        // so the values of some fields might be changed dynamically at runtime.
        CreateInfo commonInfo;

        struct Window
        {
            HWND ptr;

            UINT style;

            // Indicate whether the swap chain is already in fullscreen mode;
            // unnecessary transition between fullscreen and windowed modes can be avoided;
            // for example, isFullscreen is true and the user try to enter fullscreen mode.
            bool isFullscreen = false;

            RECT wnd, clnt;
            // Client size, NOT window size!
            UINT width, height;
        }
        window = {};

        void UpdateWindowSizeInfo()
        {
            // Don't use GetWindowRect!
            GetClientRect(window.ptr, &window.clnt);
            window.width = window.clnt.right - window.clnt.left;
            window.height = window.clnt.bottom - window.clnt.top;
        }

        void EnterFullscreenMode();
        void RestoreWindowedMode();

        // We only initialize these fields once at initialization.
        // All renderers can share these informations without duplicated query.

        static ComPtr<IDXGIFactory6> g_commonFactory;

        static struct FactoryInfo
        {
            struct Features
            {
                bool allowTearing;
            }
            features;
        }
        g_commonFactoryInfo;

        static void QueryCommonFactoryInfo();

        static void QueryCommonFactoryFeatures();
        // Tearing, a.k.a Vsync/Off
        static void QueryTearingSupport();

        using AdapterArray = std::vector<ComPtr<IDXGIAdapter>>;

        static AdapterArray g_availableAdapters;

        UINT currAdapterIndex;

        void SelectAdapter(UINT index);

        ComPtr<ID3D12Device> device;

        using DisplayModeArray = std::vector<DXGI_MODE_DESC>;

        bool QueryMsaaSupport(const UINT& sampleCount, UINT& qualityLevel);
        void ChangeMsaaSettings(bool enable, UINT sampleCount, UINT qualityLevel);

        void SetDisplayMode(UINT index, bool fullscreen, bool forceResolution);

        struct DeviceInfo
        {
            struct Properties
            {
                struct DescHandleIncrementSize
                {
                    UINT64 RTV, DSV, CBV_SRV_UAV;
                }
                descHandleIncrementSize;

                DisplayModeArray availableDisplayModes;
            }
            properties;

            struct Features
            {
                struct MSAA
                {
                    bool support;
                    UINT sampleCount;
                    UINT qualityLevel;
                }
                msaa;

                D3D12_FEATURE_DATA_ROOT_SIGNATURE rootSignature;
            }
            features;

            struct Settings
            {
                struct MSAA
                {
                    bool enable;
                    UINT sampleCount;
                    UINT qualityLevel;
                }
                msaa;

                bool fullscreen;
                bool forceResolution;
                UINT currDisplayModeIndex;

                bool allowTearing;
            }
            settings;
        }
        deviceInfo = {};

        void QueryDeviceInfo();

        void QueryDeviceProperties();
        // RTV, DSV, SRV, UAV byte sizes.
        void QueryDescHandleIncrementSizes();
        // Resolution, Refresh rate ......
        void QueryAvailableDisplayModes();

        void QueryDeviceFeatures();
        // Multi-sampling Anti-aliasing
        void QueryMsaaSupport();
        // Versioned Root Signature
        void QueryRootSignatureFeature();

        // Check whether all user customized configs are valid.
        // 
        // The user may define its own requirements in origin create info,
        // so we must compare them with supported device features
        // to decide whether the renderer can be initialized successfully.
        //
        // Throw an error when any config is invalid and abort initialization.
        //
        void CheckUserConfig();

        void CheckMsaaConfig();
        void CheckDisplayModeConfig();
        void CheckTearingConfig();

        ComPtr<ID3D12Fence> fence;

        UINT64 fenceValue = 0;

        ComPtr<ID3D12CommandQueue> cmdQueue;
        ComPtr<ID3D12CommandAllocator> cmdAlloc;
        ComPtr<ID3D12GraphicsCommandList> cmdList;

        void CreateCommandObjects();

        using FrameResourceArray = std::array<UniquePtr<FrameResource>, FrameResource::CIRCLE_COUNT>;

        FrameResourceArray frameResources;

        UINT currFrameIndex = 0;

        FrameResource* CurrFrameResource() { return frameResources.at(currFrameIndex).get(); }

        ComPtr<IDXGISwapChain3> swapChain;

        const static DXGI_FORMAT RENDER_TARGET_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;

        void CreateSwapChain();

        ComPtr<ID3D12DescriptorHeap> rtvHeap;
        ComPtr<ID3D12DescriptorHeap> srvHeap;
        ComPtr<ID3D12DescriptorHeap> dsvHeap;

        void CreateRtvHeap();
        void CreateSrvHeap();
        void CreateDsvHeap();

        D3D12_CPU_DESCRIPTOR_HANDLE GetRtvHandle(UINT offsetIndex)
        {
            return CD3DX12_CPU_DESCRIPTOR_HANDLE(
                rtvHeap->GetCPUDescriptorHandleForHeapStart(),
                (INT)offsetIndex,
                (UINT)deviceInfo.properties.descHandleIncrementSize.RTV);
        }

        D3D12_CPU_DESCRIPTOR_HANDLE GetDsvHandle(UINT offsetIndex)
        {
            return CD3DX12_CPU_DESCRIPTOR_HANDLE(
                dsvHeap->GetCPUDescriptorHandleForHeapStart(),
                (INT)offsetIndex,
                (UINT)deviceInfo.properties.descHandleIncrementSize.DSV);
        }

        ComPtr<ID3D11On12Device> d3d11On12Device;
        ComPtr<ID3D11DeviceContext> d3d11DeviceContext;

        void CreateD3D11On12Objects();

        // Keep an independent Direct2D factory in each renderer for convenience,
        // though we could maintain a common factory object like DXGI factory.
        ComPtr<ID2D1Factory1> d2d1Factory;

        ComPtr<ID2D1Device> d2d1Device;
        ComPtr<ID2D1DeviceContext> d2d1DeviceContext;
        ComPtr<ID2D1Bitmap1> d2d1RenderTarget;

        // Also an independent DWrite factory in each renderer.
        ComPtr<IDWriteFactory3> dWriteFactory;

        void CreateD2D1Objects();

        void ResizeSwapChain();

        ComPtr<ID3D12Resource> backBuffers[FrameResource::CIRCLE_COUNT];

        void CreateBackBuffers();

        ID3D12Resource* CurrBackBuffer() { return backBuffers[currFrameIndex].Get(); }

        void TransitionBackBufferState(D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
        {
            cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrBackBuffer(), before, after));
        }

        D3D12_CPU_DESCRIPTOR_HANDLE BackRtvHandle() { return GetRtvHandle(currFrameIndex); }

        ComPtr<ID3D12Resource> sceneBuffer;

        void CreateSceneBuffer();

        void TransitionSceneBufferState(D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
        {
            cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(sceneBuffer.Get(), before, after));
        }

        D3D12_CPU_DESCRIPTOR_HANDLE SceneRtvHandle() { return GetRtvHandle(_countof(backBuffers)); }
        D3D12_CPU_DESCRIPTOR_HANDLE SceneDsvHandle() { return GetDsvHandle(0); }

        ComPtr<ID3D11Resource> wrappedBuffer;

        void CreateWrappedBuffer();

        const static DXGI_FORMAT DEPTH_STENCIL_FORMAT = DXGI_FORMAT_D24_UNORM_S8_UINT;

        ComPtr<ID3D12Resource> sceneDepthStencilBuffer;

        void CreateDepthStencilBuffer();

        // MSAA (depth stencil) buffers will keep empty if MSAA is disabled.
        ComPtr<ID3D12Resource> msaaBuffer, msaaDepthStencilBuffer;

        void CreateMsaaBuffer();
        void CreateMsaaDepthStencilBuffer();

        void TransitionMsaaBufferState(D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
        {
            cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(msaaBuffer.Get(), before, after));
        }

        D3D12_CPU_DESCRIPTOR_HANDLE MsaaRtvHandle() { return GetRtvHandle(_countof(backBuffers) + 1); }
        D3D12_CPU_DESCRIPTOR_HANDLE MsaaDsvHandle() { return GetDsvHandle(1); }

        void ReleaseWrappedReferences();

        void FlushCmdQueue();
        void ResetCmdList();
        void SubmitCmdList();

        void Update();
        void Draw();

        using DrawObjectSet = std::set<SharedPtr<IDrawObject>, ISortable<IDrawObject>::UniqueAscending>;
        using DrawLayerObjectMap = std::map<SharedPtr<IDrawLayer>, DrawObjectSet, ISortable<IDrawLayer>::UniqueAscending>;

        // Drawn before 2D objects.
        DrawLayerObjectMap preDrawLayerObjects;

        void DrawD3D12Layer(DrawLayerObjectMap& target);

        XMVECTORF32 sceneColor, letterboxColor;

        void SetSceneColor(const XMVECTORF32& sceneColor);
        void SetLetterboxColor(const XMVECTORF32& letterboxColor);

        void ClearTargetBuffer(
            D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle,
            D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle,
            FLOAT depth, UINT8 stencil)
        {
            cmdList->ClearRenderTargetView(rtvHandle, sceneColor, 0, nullptr);
            D3D12_CLEAR_FLAGS clearFlags = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL;
            cmdList->ClearDepthStencilView(dsvHandle, clearFlags, depth, stencil, 0, nullptr);
        }

        using DrawObject2DSet = std::set<SharedPtr<IDrawObject2D>, ISortable<IDrawObject2D>::UniqueAscending>;

        DrawObject2DSet drawObjects2D;

        void DrawD2D1Layer();

        // Drawn after 2D objects.
        DrawLayerObjectMap postDrawLayerObjects;

        void DrawD3D12LayerMSAA();

        // The letterbox will be empty when scene resolution always follows window resolution,
        // i.e. in fullscreen mode or when forceResolution is set.
        UniquePtr<Letterbox> letterbox = nullptr;

        UINT SceneWidth() { return (letterbox == nullptr) ? window.width : letterbox->SceneWidth(); }
        UINT SceneHeight() { return (letterbox == nullptr) ? window.height : letterbox->SceneHeight(); }

        void CopySceneToBackBuffer();

        // Initialize as nullptr explicitly to wait the user to select.
        SharedPtr<IMainCamera> camera = nullptr;

        using AssociatedCameraSet = std::unordered_set<SharedPtr<ICamera>>;

        AssociatedCameraSet associatedCameras;

        UINT fps = 0;
        UniquePtr<Timer> timer;

        // Since these 2D and text rendering settings are initialized with d2dDeviceContext and dWriteFactory,
        // there's no related configurable field provided in create info, but we can change them at runtime.

        D2D1_ANTIALIAS_MODE GetAntialiasMode2D();
        void SetAntialiasMode2D(D2D1_ANTIALIAS_MODE mode);

        D2D1_TEXT_ANTIALIAS_MODE GetTextAntialiasMode();
        void SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE mode);

        struct TextRenderingSettings
        {
            FLOAT gamma;
            FLOAT enhancedContrast;
            FLOAT clearTypeLevel;
            DWRITE_PIXEL_GEOMETRY pixelGeometry;
            DWRITE_RENDERING_MODE renderingMode;
        };

        TextRenderingSettings GetDefaultTextRenderingMode();
        void SetTextRenderingMode(const TextRenderingSettings& mode);
    };
}