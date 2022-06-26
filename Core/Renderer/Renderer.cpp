#include "Precompile.h"

#include "Renderer/Renderer.h"

#include "Renderer/DebugController.h"
#include "Renderer/GraphUtils.h"

namespace d14engine::renderer
{
    Renderer::Renderer(HWND window, const CreateInfo& info) : commonInfo(info)
    {
        this->window.ptr = window;

        // All renderers will share the common factory and available adapter list,
        // so there's no need to initialize these components if they already exist.
        if (g_commonFactory == nullptr)
        {
            THROW_IF_FAILED(CreateDXGIFactory(IID_PPV_ARGS(&g_commonFactory)));

            QueryCommonFactoryInfo();

            UINT adapterIndex = 0;
            ComPtr<IDXGIAdapter> adapter;
            while (g_commonFactory->EnumAdapters(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND)
            {
                g_availableAdapters.push_back(adapter);
                ++adapterIndex; // Find next display adapter.
            }
        }

        // The renderer will be initialized based on selected adapter,
        // so all components will be recreated if the adapter is changed.
        SelectAdapter(info.adapterIndex);
    }

    void Renderer::OnWindowResize()
    {
        UpdateWindowSizeInfo();

        // Wait all commands finish before change any resource.
        FlushCmdQueue();
        ResetCmdList();
        // Clear all resource references before resize.
        ReleaseWrappedReferences();

        if (letterbox != nullptr)
        {
            auto& currDispModeIndex = deviceInfo.settings.currDisplayModeIndex;
            auto& availableDispModes = deviceInfo.properties.availableDisplayModes;

            auto& dispMode = availableDispModes.at(currDispModeIndex);

            // We must resize the letterbox ahead of swap chain and buffers
            // since the resources creation depends on the scene size info.
            letterbox->OnParentSize(dispMode.Width, dispMode.Height, window.ClientWidth (), window.ClientHeight());
        }

        ResizeSwapChain();
        // Back buffer index will be reset after swap chain resized.
        currFrameIndex = swapChain->GetCurrentBackBufferIndex();

        CreateBackBuffers();
        CreateSceneBuffer();
        CreateWrappedBuffer();
        CreateDepthStencilBuffer();

        // Update miscellaneous components.
        if (camera != nullptr) camera->OnViewResize(SceneWidth(), SceneHeight());

        if (deviceInfo.settings.msaa.enable)
        {
            CreateMsaaBuffer();
            CreateMsaaDepthStencilBuffer();
        }

        SubmitCmdList();
        // Wait until all resources are updated.
        FlushCmdQueue();

        // Reset all fence values since the renderer has restarted the render pass.
        for (auto& fr : frameResources) fr->fenceValue = fenceValue;
    }

    void Renderer::DrawNextFrame()
    {
        Update();
        Draw();
        Tick();
    }

    void Renderer::BeginExternalEvent()
    {
        d2d1DeviceContext->SetTarget(nullptr);

        d3d11DeviceContext->ClearState();
        d3d11DeviceContext->Flush();

        FlushCmdQueue();
        ResetCmdList();
    }

    void Renderer::EndExternalEvent()
    {
        SubmitCmdList();
        FlushCmdQueue();
    }

    void Renderer::SelectMainCamera(ShrdPtrParam<IMainCamera> camera)
    {
        if (associatedCameras.find(camera) == associatedCameras.end())
        {
            this->camera = camera;
        }
    }

    void Renderer::AssociateCamera(ShrdPtrParam<ICamera> camera)
    {
        if (camera != nullptr || // Main camera can't be associated!
            associatedCameras.find(camera) != associatedCameras.end())
        {
            associatedCameras.insert(camera);
        }
    }

    void Renderer::DisassociateCamera(ShrdPtrParam<ICamera> camera)
    {
        associatedCameras.erase(camera);
    }

    bool Renderer::FindAssociatedCamera(ShrdPtrParam<ICamera> camera)
    {
        return associatedCameras.find(camera) != associatedCameras.end();
    }

    void Renderer::AddPreDrawLayer(ShrdPtrParam<IDrawLayer> layer)
    {
        if (layer == nullptr) return;
        preDrawLayerObjects[layer] = {};
    }

    void Renderer::RemovePreDrawLayer(ShrdPtrParam<IDrawLayer> layer)
    {
        preDrawLayerObjects.erase(layer);
    }

    bool Renderer::FindPreDrawLayer(ShrdPtrParam<IDrawLayer> layer)
    {
        return preDrawLayerObjects.find(layer) != preDrawLayerObjects.end();
    }

    void Renderer::AddPostDrawLayer(ShrdPtrParam<IDrawLayer> layer)
    {
        if (layer == nullptr) return;
        postDrawLayerObjects[layer] = {};
    }

    void Renderer::RemovePostDrawLayer(ShrdPtrParam<IDrawLayer> layer)
    {
        postDrawLayerObjects.erase(layer);
    }

    bool Renderer::FindPostDrawLayer(ShrdPtrParam<IDrawLayer> layer)
    {
        return postDrawLayerObjects.find(layer) != postDrawLayerObjects.end();
    }

    void Renderer::AddPreDrawObject(ShrdPtrParam<IDrawObject> obj, ShrdPtrParam<IDrawLayer> layer)
    {
        if (obj == nullptr || layer == nullptr) return;
        auto pLayer = preDrawLayerObjects.find(layer);

        if (pLayer != preDrawLayerObjects.end())
        {
            pLayer->second.insert(obj);
        }
        // Insert automatically if specified draw layer doesn't exist.
        else preDrawLayerObjects[layer] = { obj };
    }

    void Renderer::RemovePreDrawObject(ShrdPtrParam<IDrawObject> obj, ShrdPtrParam<IDrawLayer> layer)
    {
        auto pLayer = preDrawLayerObjects.find(layer);

        if (pLayer != preDrawLayerObjects.end())
        {
            pLayer->second.erase(obj);
        }
    }

    bool Renderer::FindPreDrawObject(ShrdPtrParam<IDrawObject> obj, ShrdPtrParam<IDrawLayer> layer)
    {
        auto pLayer = preDrawLayerObjects.find(layer);

        if (pLayer != preDrawLayerObjects.end())
        {
            return pLayer->second.find(obj) != pLayer->second.end();
        }
        else return false;
    }

    void Renderer::AddPostDrawObject(ShrdPtrParam<IDrawObject> obj, ShrdPtrParam<IDrawLayer> layer)
    {
        if (obj == nullptr || layer == nullptr) return;
        auto pLayer = postDrawLayerObjects.find(layer);

        if (pLayer != postDrawLayerObjects.end())
        {
            pLayer->second.insert(obj);
        }
        // Insert automatically if specified draw layer doesn't exist.
        else postDrawLayerObjects[layer] = { obj };
    }

    void Renderer::RemovePostDrawObject(ShrdPtrParam<IDrawObject> obj, ShrdPtrParam<IDrawLayer> layer)
    {
        auto pLayer = postDrawLayerObjects.find(layer);

        if (pLayer != postDrawLayerObjects.end())
        {
            pLayer->second.erase(obj);
        }
    }

    bool Renderer::FindPostDrawObject(ShrdPtrParam<IDrawObject> obj, ShrdPtrParam<IDrawLayer> layer)
    {
        auto pLayer = postDrawLayerObjects.find(layer);

        if (pLayer != postDrawLayerObjects.end())
        {
            return pLayer->second.find(obj) != pLayer->second.end();
        }
        else return false;
    }

    void Renderer::AddDrawObject2D(ShrdPtrParam<IDrawObject2D> obj2d)
    {
        if (obj2d == nullptr) return;
        drawObjects2D.insert(obj2d);
    }

    void Renderer::RemoveDrawObject2D(ShrdPtrParam<IDrawObject2D> obj2d)
    {
        drawObjects2D.erase(obj2d);
    }

    bool Renderer::FindDrawObject2D(ShrdPtrParam<IDrawObject2D> obj2d)
    {
        return drawObjects2D.find(obj2d) != drawObjects2D.end();
    }

    void Renderer::EnterFullscreenMode()
    {
        // Save the old window rect so we can restore it when exiting fullscreen mode.
        GetWindowRect(window.ptr, &window.windowRect);

        window.style = GetWindowLong(window.ptr, GWL_STYLE);
        // Make the window borderless so that the client area can fill the screen.
        SetWindowLong(window.ptr, GWL_STYLE, WS_POPUP);

        RECT fullscreenRect;
        // Try to get current output screen size info from the swap chain firstly;
        // otherwise we have to query the primary device size info from Win32 GDI interface.
        try
        {
            ComPtr<IDXGIOutput> currOutput;
            THROW_IF_FAILED(swapChain->GetContainingOutput(&currOutput));

            DXGI_OUTPUT_DESC outputDesc;
            THROW_IF_FAILED(currOutput->GetDesc(&outputDesc));

            fullscreenRect = outputDesc.DesktopCoordinates;
        }
        catch (...)
        {
            DEVMODE devMode = {};
            devMode.dmSize = sizeof(DEVMODE);

            EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &devMode);

            fullscreenRect =
            {
                devMode.dmPosition.x,
                devMode.dmPosition.y,
                devMode.dmPosition.x + (LONG)devMode.dmPelsWidth,
                devMode.dmPosition.y + (LONG)devMode.dmPelsHeight
            };
        }

        SetWindowPos(
            window.ptr,
            HWND_TOPMOST,
            fullscreenRect.left,
            fullscreenRect.top,
            fullscreenRect.right,
            fullscreenRect.bottom,
            SWP_FRAMECHANGED | SWP_NOACTIVATE);

        ShowWindow(window.ptr, SW_MAXIMIZE);
    }

    void Renderer::RestoreWindowedMode()
    {
        SetWindowLong(window.ptr, GWL_STYLE, window.style);

        SetWindowPos(
            window.ptr,
            HWND_NOTOPMOST,
            window.windowRect.left,
            window.windowRect.top,
            window.WindowWidth(),
            window.WindowHeight(),
            SWP_FRAMECHANGED | SWP_NOACTIVATE);

        ShowWindow(window.ptr, SW_NORMAL);
    }

    ComPtr<IDXGIFactory6> Renderer::g_commonFactory = {};

    Renderer::FactoryInfo Renderer::g_commonFactoryInfo = {};

    void Renderer::QueryCommonFactoryInfo()
    {
        QueryCommonFactoryFeatures();
    }

    void Renderer::QueryCommonFactoryFeatures()
    {
        QueryTearingSupport();
    }

    void Renderer::QueryTearingSupport()
    {
        BOOL allowTearing = FALSE;

        auto hr = g_commonFactory->CheckFeatureSupport(
            DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));

        // Make sure the result is returned by succeeded operation.
        g_commonFactoryInfo.features.allowTearing = SUCCEEDED(hr) && allowTearing;
    }

    Renderer::AdapterArray Renderer::g_availableAdapters = {};

    void Renderer::SelectAdapter(UINT index)
    {
        // Check whether the user selected adapter index is valid.
        if (index >= g_availableAdapters.size())
        {
            auto descText = L"Selected display adapter index out of range. Only " +
                std::to_wstring(g_availableAdapters.size()) + L" is available:\n";
            // Generate detailed message about available adapters.
            for (int i = 0; i < g_availableAdapters.size(); ++i)
            {
                auto& adpt = g_availableAdapters[i];
                DXGI_ADAPTER_DESC adptDesc; adpt->GetDesc(&adptDesc);
                descText += (L"Adapter " + std::to_wstring(i) + L": " + adptDesc.Description + L"\n");
            }
            THROW_ERROR(descText);
        }
        // Place this after check passed thus the renderer wouldn't be spoiled if specified index is invalid.
        currAdapterIndex = index;
        auto currAdapter = g_availableAdapters[currAdapterIndex].Get();

#ifdef _DEBUG
        // Enable debug layer in advance to debug device creating!
        DebugController::EnableD3D12DebugLayer();
#endif

        THROW_IF_FAILED(D3D12CreateDevice(currAdapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device)));

        auto& rendererMainDevice = device;
        NAME_D3D12_OBJECT(rendererMainDevice);

#ifdef _DEBUG
        // Note this operation depends on D3D12 device!
        DebugController::SuppressUselessWarnings(this);
#endif

        QueryDeviceInfo();

        // Check user configuration immediately after GPU device selected
        // to avoid useless initialization if any given config is invalid.
        CheckUserConfig();

        THROW_IF_FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));

        auto& rendererMainFence = fence;
        NAME_D3D12_OBJECT(rendererMainFence);

        CreateCommandObjects();

        // No need to flush the command queue as it was created just now.
        ResetCmdList();

        for (auto& frs : frameResources)  frs = std::make_unique<FrameResource>(device.Get());

        // We must initialize the letterbox ahead of descriptor heaps
        // since the resources creation depends on the letterbox state info.
        if (deviceInfo.settings.forceResolution)
        {
            letterbox = std::make_unique<Letterbox>(this);
        }
        // The letterbox will be used to check whether scene resolution is equal to window resolution,
        // so we must assign nullptr to the letterbox explicitly if the resolutions can be different.
        else letterbox = nullptr;

        CreateRtvHeap();
        CreateDsvHeap();

        // The SRV heap is only used by the letterbox to post scene to back buffer.
        // If letterbox hasn't been initialized, then there's no need to create the SRV heap.
        if (letterbox != nullptr) CreateSrvHeap();

        // We need to update window size info manually since OnWindowResize hasn't been called yet.
        UpdateWindowSizeInfo();
        // Creating swap chain depends on correct window size info.
        CreateSwapChain();

        CreateD3D11On12Objects();
        CreateD2D1Objects();

        // Create miscellaneous components.
        (timer = std::make_unique<Timer>())->Start();

        SubmitCmdList();
        FlushCmdQueue();

        // Initialize all size dependent resources.
        OnWindowResize();
    }

    bool Renderer::QueryMsaaSupport(const UINT& sampleCount, UINT& qualityLevel)
    {
        auto originalMsaaFeature = deviceInfo.features.msaa;

        // The query result will overwrite the MSAA feature fields,
        // so we need to resotre the original values after query finished.
        auto undoMsaaFeature = Finally([&] { deviceInfo.features.msaa = originalMsaaFeature; });

        commonInfo.enableMSAA = true;
        commonInfo.msaaSampleCount = sampleCount;
        commonInfo.msaaQualityLevel = qualityLevel;

        QueryMsaaSupport();

        bool supportMSAA = deviceInfo.features.msaa.support;
        qualityLevel = deviceInfo.features.msaa.qualityLevel;

        return supportMSAA;
    }

    void Renderer::ChangeMsaaSettings(bool enable, UINT sampleCount, UINT qualityLevel)
    {
        bool changeFailed = true;

        auto originalMsaaFeature = deviceInfo.features.msaa;

        // Make sure the renderer wouldn't be spoiled if changing MSAA settings failed.
        auto undoMsaaFeature = Finally([&] { if (changeFailed) deviceInfo.features.msaa = originalMsaaFeature; });

        try
        {
            if (enable && sampleCount > 1)
            {
                commonInfo.enableMSAA = true;
                commonInfo.msaaSampleCount = sampleCount;
                commonInfo.msaaQualityLevel = qualityLevel;

                QueryMsaaSupport();
                CheckMsaaConfig();
            }
            else
            {
                deviceInfo.settings.msaa.enable = false;
                deviceInfo.settings.msaa.sampleCount = 1;
                deviceInfo.settings.msaa.qualityLevel = 0;
            }

            // The descriptor count in RTV heap depends on MSAA settings!
            // Note we will create the MSAA buffer's RTV handle at the end of heap.
            CreateRtvHeap();

            // Update all resources related to MSAA settings.
            OnWindowResize();

            changeFailed = false;
        }
        catch (...) { throw; }
    }

    void Renderer::SetDisplayMode(UINT index, bool fullscreen, bool forceResolution)
    {
        commonInfo.fullscreen = fullscreen;
        commonInfo.forceResolution = forceResolution;

        auto& dispMode = deviceInfo.properties.availableDisplayModes.at(index);

        commonInfo.resolution.width = dispMode.Width;
        commonInfo.resolution.height = dispMode.Height;

        commonInfo.refreshRate.numerator = dispMode.RefreshRate.Numerator;
        commonInfo.refreshRate.denominator = dispMode.RefreshRate.Denominator;

        try
        {
            CheckDisplayModeConfig();

            FlushCmdQueue();
            ResetCmdList();

            if (deviceInfo.settings.forceResolution)
            {
                letterbox = std::make_unique<Letterbox>(this);
                CreateSrvHeap();
            }
            // The letterbox will be used to check whether scene resolution is equal to window resolution,
            // so we must assign nullptr to the letterbox explicitly if the resolutions can be different.
            else letterbox = nullptr;

            SubmitCmdList();

            // Resize the target window manually when tearing (Vsync/Off) is available;
            // otherwise we have to depend on the swap chain to switch fullscreen mode.
            if (g_commonFactoryInfo.features.allowTearing)
            {
                // Skip unnecessary transition between fullscreen and windowed modes.
                if (deviceInfo.settings.fullscreen && !window.isFullscreen)
                {
                    EnterFullscreenMode();
                }
                else if (!deviceInfo.settings.fullscreen && window.isFullscreen)
                {
                    RestoreWindowedMode();
                }
                window.isFullscreen = deviceInfo.settings.fullscreen;
            }
            else THROW_IF_FAILED(swapChain->SetFullscreenState(deviceInfo.settings.fullscreen, nullptr));

            // Update all size dependent resources.
            OnWindowResize();
        }
        catch (...) { throw; }
    }

    void Renderer::QueryDeviceInfo()
    {
        QueryDeviceProperties();

        QueryDeviceFeatures();
    }

    void Renderer::QueryDeviceProperties()
    {
        QueryDescHandleIncrementSizes();

        QueryAvailableDisplayModes();
    }

    void Renderer::QueryDescHandleIncrementSizes()
    {
        deviceInfo.properties.descHandleIncrementSize.RTV =
            device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        deviceInfo.properties.descHandleIncrementSize.DSV =
            device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

        deviceInfo.properties.descHandleIncrementSize.CBV_SRV_UAV =
            device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    void Renderer::QueryAvailableDisplayModes()
    {
        auto& currAdpt = g_availableAdapters.at(currAdapterIndex);

        ComPtr<IDXGIOutput> output;
        // TODO: support multiple display outputs.
        THROW_IF_FAILED(currAdpt->EnumOutputs(0, &output));

        UINT modeCount = 0;
        output->GetDisplayModeList(
            RENDER_TARGET_FORMAT,
            0,
            &modeCount,
            nullptr);

        deviceInfo.properties.availableDisplayModes.resize(modeCount);
        output->GetDisplayModeList(
            RENDER_TARGET_FORMAT,
            0,
            &modeCount,
            deviceInfo.properties.availableDisplayModes.data());
    }

    void Renderer::QueryDeviceFeatures()
    {
        QueryMsaaSupport();

        QueryRootSignatureFeature();
    }

    void Renderer::QueryMsaaSupport()
    {
        D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msaaInfo;
        msaaInfo.Format = RENDER_TARGET_FORMAT;
        msaaInfo.SampleCount = commonInfo.msaaSampleCount;
        msaaInfo.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
        msaaInfo.NumQualityLevels = 0;

        device->CheckFeatureSupport(
            D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
            &msaaInfo,
            sizeof(msaaInfo));

        deviceInfo.features.msaa.support = msaaInfo.NumQualityLevels > 0;
        deviceInfo.features.msaa.sampleCount = msaaInfo.SampleCount;
        // Note actual supported quality level is equal to queried quality level - 1.
        deviceInfo.features.msaa.qualityLevel = msaaInfo.NumQualityLevels - 1;
    }

    void Renderer::QueryRootSignatureFeature()
    {
        // This is the highest root signature version the renderer supports.
        // If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
        auto& version = (deviceInfo.features.rootSignature.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1);

        if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &version, sizeof(version))))
        {
            // The renderer also supports lower version of root signature.
            version = D3D_ROOT_SIGNATURE_VERSION_1_0;
        }
    }

    void Renderer::CheckUserConfig()
    {
        // Populate miscellaneous configs.
        sceneColor = commonInfo.sceneColor;
        letterboxColor = commonInfo.letterboxColor;

        CheckMsaaConfig();

        CheckDisplayModeConfig();

        CheckTearingConfig();
    }

    void Renderer::CheckDisplayModeConfig()
    {
        UINT dispModeIndex = 0;

        if (commonInfo.forceResolution)
        {            
            auto& availableDisplayModes = deviceInfo.properties.availableDisplayModes;

            for (dispModeIndex = 0; dispModeIndex < availableDisplayModes.size(); ++dispModeIndex)
            {
                auto& mode = availableDisplayModes[dispModeIndex];
                if (mode.Width == commonInfo.resolution.width &&
                    mode.Height == commonInfo.resolution.height &&
                    mode.RefreshRate.Numerator == commonInfo.refreshRate.numerator &&
                    mode.RefreshRate.Denominator == commonInfo.refreshRate.denominator)
                {
                    break; // Display mode matched.
                }
            }
            // Specific display mode not supported.
            if (dispModeIndex == availableDisplayModes.size())
            {
                auto descText = L"Specified display mode not supported. Total " +
                                std::to_wstring(availableDisplayModes.size()) +
                                L" display modes are available on current selected GPU device:\n";
                // Generate detailed message about display modes.
                for (int n = 0; n < availableDisplayModes.size(); ++n)
                {
                    auto& mode = availableDisplayModes[n];
                    descText += (L"Display mode " + std::to_wstring(n) + L": "
                                 L"Width " + std::to_wstring(mode.Width) + L", "
                                 L"Height " + std::to_wstring(mode.Height) + L", "
                                 L"Refresh Rate " + std::to_wstring(mode.RefreshRate.Numerator) +
                                 L"/" + std::to_wstring(mode.RefreshRate.Denominator) + L"\n");
                }
                THROW_ERROR(descText);
            }
        }
        deviceInfo.settings.fullscreen = commonInfo.fullscreen;
        deviceInfo.settings.forceResolution = commonInfo.forceResolution;
        deviceInfo.settings.currDisplayModeIndex = dispModeIndex;
    }

    void Renderer::CheckMsaaConfig()
    {
        if (!deviceInfo.features.msaa.support && commonInfo.enableMSAA)
        {
            THROW_ERROR(std::to_wstring(commonInfo.msaaSampleCount) +
                        L"xMSAA not supported on specific GPU device.");
        }
        deviceInfo.settings.msaa.enable = commonInfo.enableMSAA;
        deviceInfo.settings.msaa.sampleCount = commonInfo.msaaSampleCount;
        // Use the best available MSAA quality level or the user designated quality level.
        deviceInfo.settings.msaa.qualityLevel = std::min(deviceInfo.features.msaa.qualityLevel, commonInfo.msaaQualityLevel);
    }

    void Renderer::CheckTearingConfig()
    {
        if (!g_commonFactoryInfo.features.allowTearing && commonInfo.allowTearing)
        {
            THROW_ERROR(L"Tearing (a.k.a Vsync/Off) support is not available.");
        }
    }

    void Renderer::CreateCommandObjects()
    {
        // Command queue.
        D3D12_COMMAND_QUEUE_DESC cqdesc = {};
        cqdesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        cqdesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

        THROW_IF_FAILED(device->CreateCommandQueue(&cqdesc, IID_PPV_ARGS(&cmdQueue)));

        auto& rendererMainCmdQueue = cmdQueue;
        NAME_D3D12_OBJECT(rendererMainCmdQueue);

        // Command allocator.
        THROW_IF_FAILED(device->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAlloc)));

        auto& rendererMainCmdAlloc = cmdAlloc;
        NAME_D3D12_OBJECT(rendererMainCmdAlloc);

        // Command list.
        THROW_IF_FAILED(device->CreateCommandList(
            0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAlloc.Get(), nullptr, IID_PPV_ARGS(&cmdList)));

        auto& rendererMainCmdList = cmdList;
        NAME_D3D12_OBJECT(rendererMainCmdList);

        // Command list must be closed before reset.
        cmdList->Close();
    } 

    void Renderer::CreateSwapChain()
    {
        DXGI_SWAP_CHAIN_DESC1 desc = {};

        // The resolution of the swap chain back buffers will match the resolution of the window.
        // We will also keep a separate buffer that is not part of the swap chain as intermidiate
        // render target, whose resolution will control the rendering resolution of actual scene.
        desc.Width = window.ClientWidth();
        desc.Height = window.ClientHeight();

        desc.Format = RENDER_TARGET_FORMAT;
        // DirectX 12 3D does NOT support creating MSAA swap chain.
        // In canonical method, a MSAA swap chain will be resolved automatically during presenting,
        // which is NOT supported in UWP program. One solution: create a MSAA render target instead.
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;

        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.BufferCount = _countof(backBuffers);

        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        // It is recommended to always use the tearing flag when it is available.
        if (g_commonFactoryInfo.features.allowTearing) desc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

        ComPtr<IDXGISwapChain1> tmpSwapChain;
        THROW_IF_FAILED(g_commonFactory->CreateSwapChainForHwnd(
            cmdQueue.Get(), window.ptr, &desc, nullptr, nullptr, &tmpSwapChain));

        THROW_IF_FAILED(tmpSwapChain.As(&swapChain));
        currFrameIndex = swapChain->GetCurrentBackBufferIndex();

        // Miscellaneous settings about swap chain.
        if (g_commonFactoryInfo.features.allowTearing)
        {
            // When tearing support is available we would handle [ALT + Enter] key presses in the
            // window message loop rather than let DXGI handle it by calling [SetFullscreenState].
            g_commonFactory->MakeWindowAssociation(window.ptr, DXGI_MWA_NO_ALT_ENTER);
        }
    }

    void Renderer::CreateRtvHeap()
    {
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};

        rtvHeapDesc.NumDescriptors = _countof(backBuffers) + 1;
        // In RTV heap: back buffers, scene buffer, MSAA buffer (if MSAA is enabled) .......
        if (deviceInfo.settings.msaa.enable) rtvHeapDesc.NumDescriptors += 1;

        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        THROW_IF_FAILED(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap)));

        auto& rendererMainRtvHeap = rtvHeap;
        NAME_D3D12_OBJECT(rendererMainRtvHeap);
    }

    void Renderer::CreateSrvHeap()
    {
        D3D12_DESCRIPTOR_HEAP_DESC cbvSrvHeapDesc = {};
        cbvSrvHeapDesc.NumDescriptors = 1;
        cbvSrvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        cbvSrvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        THROW_IF_FAILED(device->CreateDescriptorHeap(&cbvSrvHeapDesc, IID_PPV_ARGS(&srvHeap)));

        auto& rendererMainSrvHeap = srvHeap;
        NAME_D3D12_OBJECT(rendererMainSrvHeap);
    }

    void Renderer::CreateDsvHeap()
    {
        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};

        dsvHeapDesc.NumDescriptors = 1;
        // In DSV heap: scene buffer, MSAA buffer (if MSAA is enabled) .......
        if (deviceInfo.settings.msaa.enable) dsvHeapDesc.NumDescriptors += 1;

        dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        THROW_IF_FAILED(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap)));

        auto& rendererMainDsvHeap = dsvHeap;
        NAME_D3D12_OBJECT(rendererMainDsvHeap);
    }

    void Renderer::CreateD3D11On12Objects()
    {
        ComPtr<ID3D11Device> d3d11Device;
        THROW_IF_FAILED(D3D11On12CreateDevice(
            device.Get(),
            D3D11_CREATE_DEVICE_BGRA_SUPPORT,
            nullptr, // Feature Levels
            0, // Num Feature Levels
            (IUnknown**)cmdQueue.GetAddressOf(),
            1, // Num Command Queues
            0, // Node Mask
            &d3d11Device, // D3D11 Device
            &d3d11DeviceContext,
            nullptr)); // Chosen Feature Level

        // Map D3D11 device to D3D12 layer.
        THROW_IF_FAILED(d3d11Device.As(&d3d11On12Device));
    }

    void Renderer::CreateD2D1Objects()
    {
        THROW_IF_FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, IID_PPV_ARGS(&d2d1Factory)));

        ComPtr<IDXGIDevice> dxgiDevice;
        THROW_IF_FAILED(d3d11On12Device.As(&dxgiDevice));

        THROW_IF_FAILED(d2d1Factory->CreateDevice(dxgiDevice.Get(), &d2d1Device));

        THROW_IF_FAILED(d2d1Device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2d1DeviceContext));

        // We don't use IID_PPV_ARGS here since this func receives IUnknown** instead of void**.
        THROW_IF_FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(dWriteFactory), &dWriteFactory));
    }

    void Renderer::ResizeSwapChain()
    {
        // Clear original back buffers before resize.
        for (auto& buffer : backBuffers)
        {
            buffer.Reset();
        }
        sceneDepthStencilBuffer.Reset();
        // Reseting empty buffer is allowed.
        msaaDepthStencilBuffer.Reset();

        DXGI_SWAP_CHAIN_DESC desc = {};
        swapChain->GetDesc(&desc);
        // Note desc.Flags must be compatible with original swap chain!
        THROW_IF_FAILED(swapChain->ResizeBuffers(
            _countof(backBuffers),
            window.ClientWidth(),
            window.ClientHeight(),
            desc.BufferDesc.Format,
            desc.Flags));
    }

    void Renderer::CreateBackBuffers()
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE handle(rtvHeap->GetCPUDescriptorHandleForHeapStart());

        for (UINT i = 0; i < _countof(backBuffers); ++i)
        {
            // Acquire allocated back buffers from swap chain.
            THROW_IF_FAILED(swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i])));

            auto& rendererMainBackBuffer = backBuffers[i];
            NAME_D3D12_OBJECT_INDEXED(rendererMainBackBuffer, i);

            // Create related handles in RTV heap.
            device->CreateRenderTargetView(backBuffers[i].Get(), nullptr, handle);
            handle.Offset(1, (UINT)deviceInfo.properties.descHandleIncrementSize.RTV);
        }
    }

    void Renderer::CreateSceneBuffer()
    {
        // Create scene buffer resource.
        D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Tex2D(
            RENDER_TARGET_FORMAT,
            SceneWidth(),
            SceneHeight(),
            1, // Array Size
            1, // Mip Levels
            1, // Sample Count
            0); // Sample Quality

        bufferDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = RENDER_TARGET_FORMAT;
        memcpy(clearValue.Color, sceneColor, 4 * sizeof(FLOAT));

        THROW_IF_FAILED(device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            D3D12_RESOURCE_STATE_COMMON,
            &clearValue,
            IID_PPV_ARGS(&sceneBuffer)));

        auto& rendererMainSceneBuffer = sceneBuffer;
        NAME_D3D12_OBJECT(rendererMainSceneBuffer);

        device->CreateRenderTargetView(sceneBuffer.Get(), nullptr,
            CD3DX12_CPU_DESCRIPTOR_HANDLE( // Place at the end of back buffers.
                rtvHeap->GetCPUDescriptorHandleForHeapStart(),
                _countof(backBuffers) * (INT)deviceInfo.properties.descHandleIncrementSize.RTV));

        // The SRV is only used by the letterbox to post scene to back buffer.
        // If letterbox hasn't been initialized, then there's no need to create the SRV.
        if (letterbox != nullptr) device->CreateShaderResourceView(
            sceneBuffer.Get(), nullptr, srvHeap->GetCPUDescriptorHandleForHeapStart());
    }

    void Renderer::CreateWrappedBuffer()
    {
        // Creating D2D1 render target depends on window dpi info.
        FLOAT dpi = (FLOAT)GetDpiForWindow(window.ptr);

        D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(
            D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
            D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
            dpi, dpi); // Simply use the same DPI for x,y-axes.

        D3D11_RESOURCE_FLAGS flags = {};
        flags.BindFlags = D3D11_BIND_RENDER_TARGET;

        // Create wrapped buffer.
        THROW_IF_FAILED(d3d11On12Device->CreateWrappedResource(
            sceneBuffer.Get(),
            &flags,
            D3D12_RESOURCE_STATE_COMMON, // In State
            D3D12_RESOURCE_STATE_COMMON, // Out State
            IID_PPV_ARGS(&wrappedBuffer)));

        // Create D2D1 render target.
        ComPtr<IDXGISurface> surface;
        THROW_IF_FAILED(wrappedBuffer.As(&surface));

        THROW_IF_FAILED(d2d1DeviceContext->CreateBitmapFromDxgiSurface(
            surface.Get(), &props, &d2d1RenderTarget));
    }

    void Renderer::CreateDepthStencilBuffer()
    {
        // Create depth stencil buffer resource.
        D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Tex2D(
            DEPTH_STENCIL_FORMAT, // NOT render target buffer!
            SceneWidth(),
            SceneHeight(),
            1, // Array Size
            1, // Mipmap Level Count
            1, // Sample Count
            0); // Sample Quality

        bufferDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = DEPTH_STENCIL_FORMAT;
        clearValue.DepthStencil.Depth = 1.0f;
        clearValue.DepthStencil.Stencil = 0;

        THROW_IF_FAILED(device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            D3D12_RESOURCE_STATE_COMMON,
            &clearValue,
            IID_PPV_ARGS(&sceneDepthStencilBuffer)));

        device->CreateDepthStencilView(
            sceneDepthStencilBuffer.Get(), nullptr, dsvHeap->GetCPUDescriptorHandleForHeapStart());

        // Depth stencil buffer must be transitioned to write mode before using.
        cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
            sceneDepthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));
    }

    void Renderer::CreateMsaaBuffer()
    {
        // Create MSAA buffer resource.
        D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Tex2D(
            RENDER_TARGET_FORMAT,
            SceneWidth(),
            SceneHeight(),
            1, // Array Size
            1, // Mip Levels
            deviceInfo.settings.msaa.sampleCount, // Sample Count
            deviceInfo.settings.msaa.qualityLevel); // Sample Quality

        bufferDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = RENDER_TARGET_FORMAT;
        memcpy(clearValue.Color, sceneColor, 4 * sizeof(FLOAT));

        THROW_IF_FAILED(device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            &clearValue,
            IID_PPV_ARGS(&msaaBuffer)));

        // Create MSAA buffer handle in RTV heap.
        D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
        rtvDesc.Format = RENDER_TARGET_FORMAT;
        rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;

        device->CreateRenderTargetView(msaaBuffer.Get(), &rtvDesc,
            CD3DX12_CPU_DESCRIPTOR_HANDLE( // Place at the end of scene buffer.
            rtvHeap->GetCPUDescriptorHandleForHeapStart(),
            (_countof(backBuffers) + 1) * (INT)deviceInfo.properties.descHandleIncrementSize.RTV));
    }

    void Renderer::CreateMsaaDepthStencilBuffer()
    {
        // Create MSAA depth stencil buffer resource.
        D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Tex2D(
            DEPTH_STENCIL_FORMAT, // NOT render target buffer!
            SceneWidth(),
            SceneHeight(),
            1, // Array Size
            1, // Mipmap Level Count
            deviceInfo.settings.msaa.sampleCount, // Sample Count
            deviceInfo.settings.msaa.qualityLevel); // Sample Quality

        bufferDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = DEPTH_STENCIL_FORMAT;
        clearValue.DepthStencil.Depth = 1.0f;
        clearValue.DepthStencil.Stencil = 0;

        THROW_IF_FAILED(device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            D3D12_RESOURCE_STATE_COMMON,
            &clearValue,
            IID_PPV_ARGS(&msaaDepthStencilBuffer)));

        // Create MSAA depth stencil buffer handle in DSV heap.
        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
        dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
        dsvDesc.Format = DEPTH_STENCIL_FORMAT;
        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;

        device->CreateDepthStencilView(msaaDepthStencilBuffer.Get(), &dsvDesc,
            CD3DX12_CPU_DESCRIPTOR_HANDLE( // Place at the end of scene buffer.
                dsvHeap->GetCPUDescriptorHandleForHeapStart(),
                (INT)deviceInfo.properties.descHandleIncrementSize.DSV));

        // Depth stencil buffer must be transitioned to write mode before using.
        cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
            msaaDepthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));
    }

    void Renderer::ReleaseWrappedReferences()
    {
        // It is recommended to release references in reverse order of creating.
        d2d1RenderTarget.Reset();
        wrappedBuffer.Reset();

        d2d1DeviceContext->SetTarget(nullptr);
        // Don't flush the d2d1 device context here since it's redundant and invalid.
        // It doesn't flush the d3d11 device context and may fail if called outside the BeginDraw/EndDraw call.

        d3d11DeviceContext->ClearState();
        d3d11DeviceContext->Flush();
    }

    void Renderer::FlushCmdQueue()
    {
        ++fenceValue;
        // Increment in case that the command queue is already empty.
        THROW_IF_FAILED(cmdQueue->Signal(fence.Get(), fenceValue));

        if (fence->GetCompletedValue() < fenceValue)
        {
            HANDLE hEvent;
            THROW_IF_NULL(hEvent = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS));
            // We need to wait all submitted commands to finish, so use fenceValue.
            THROW_IF_FAILED(fence->SetEventOnCompletion(fenceValue, hEvent));
            WaitForSingleObject(hEvent, INFINITE);
        }
    }

    void Renderer::ResetCmdList()
    {
        THROW_IF_FAILED(cmdAlloc->Reset());
        THROW_IF_FAILED(cmdList->Reset(cmdAlloc.Get(), nullptr));
    }

    void Renderer::SubmitCmdList()
    {
        THROW_IF_FAILED(cmdList->Close());
        ID3D12CommandList* ppCmdList[] = { cmdList.Get() };
        cmdQueue->ExecuteCommandLists(1, ppCmdList);
    }

    void Renderer::Update()
    {
        // Check whether current frame resource is prepared.
        if (CurrFrameResource()->fenceValue != 0 && fence->GetCompletedValue() < CurrFrameResource()->fenceValue)
        {
            HANDLE hEvent;
            THROW_IF_NULL(hEvent = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS));
            // We need to wait the commands of current frame resource that was submitted
            // in previous render pass to finish, so use CurrFrameResource()->fenceValue. 
            THROW_IF_FAILED(fence->SetEventOnCompletion(CurrFrameResource()->fenceValue, hEvent));
            WaitForSingleObject(hEvent, INFINITE);
            CloseHandle(hEvent);
        }
        if (skipUpdating) return;

        CurrFrameResource()->ResetCmdList(cmdList.Get(), FrameResource::CmdLayer::Update);

        // Start updaing dynamic resources.
        const static auto updateDrawLayerObjects = [&](DrawLayerObjectMap& target)
        {
            for (auto& layer : target)
            {
                if (layer.first->IsD3D12LayerVisible())
                {
                    layer.first->OnRendererUpdateLayer(this);
                }
                for (auto& obj : layer.second)
                {
                    if (obj->IsD3D12ObjectVisible())
                    {
                        obj->OnRendererUpdateObject(this);
                    }
                }
            }
        };
        updateDrawLayerObjects(preDrawLayerObjects);
        updateDrawLayerObjects(postDrawLayerObjects);

        for (auto& obj2d : drawObjects2D)
        {
            if (obj2d->IsD2D1ObjectVisible())
            {
                obj2d->OnRendererUpdateObject2D(this);
            }
        }

        // Update miscellaneous components.
        if (camera != nullptr && camera->IsD3D12ObjectVisible())
        {
            camera->OnRendererUpdateObject(this);
        }

        for (auto& icam : associatedCameras)
        {
            if (icam->IsD3D12ObjectVisible())
            {
                icam->OnRendererUpdateObject(this);
            }
        }

        SubmitCmdList();
    }

    void Renderer::Draw()
    {
        if (camera != nullptr)
        {
            if (deviceInfo.settings.msaa.enable)
            {
                DrawD3D12LayerMSAA();
            }
            else DrawD3D12Layer(preDrawLayerObjects);

            DrawD2D1Layer();
            
            DrawD3D12Layer(postDrawLayerObjects);
        }
        else
        {
            CurrFrameResource()->ResetCmdList(cmdList.Get(), FrameResource::CmdLayer::ClearBkgn);

            // Transition state of scene buffer, NOT back buffer!
            TransitionSceneBufferState(D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET);

            // No need to do depth test, so DSV is skipped.
            cmdList->ClearRenderTargetView(SceneRtvHandle(), sceneColor, 0, nullptr);

            TransitionSceneBufferState(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON);

            SubmitCmdList();

            DrawD2D1Layer();
        }

        CurrFrameResource()->ResetCmdList(cmdList.Get(), FrameResource::CmdLayer::PostScene);

        if (letterbox != nullptr)
        {
            letterbox->PostSceneToBackBuffer(this);
        }
        else CopySceneToBackBuffer();

        SubmitCmdList();

        THROW_IF_FAILED(swapChain->Present(0, 0));

        CurrFrameResource()->fenceValue = ++fenceValue;
        THROW_IF_FAILED(cmdQueue->Signal(fence.Get(), fenceValue));

        // The renderer's frame resource count is equal to its back buffer count,
        // i.e. the frame resource index is exactly the back buffer index.
        currFrameIndex = swapChain->GetCurrentBackBufferIndex();
    }

    void Renderer::Tick()
    {
        static UINT FRAME_COUNT_SUM = 0;
        static double ONE_SEC_POINT = 0;

        timer->Tick();

        ++FRAME_COUNT_SUM;
        // Update FPS info if already pass one second.
        if (timer->ElapsedSecs() >= ONE_SEC_POINT + 1)
        {
            fps = FRAME_COUNT_SUM;
            // Prepare for next second.
            FRAME_COUNT_SUM = 0;
            ONE_SEC_POINT = timer->ElapsedSecs();
        }
    }

    void Renderer::DrawD3D12Layer(DrawLayerObjectMap& target)
    {
        // Compare the object address instead of container entity!
        // >>> &target == &preDrawLayerObjects <<<

        CurrFrameResource()->ResetCmdList(cmdList.Get(), (&target == &preDrawLayerObjects) ?
            FrameResource::CmdLayer::PreD3D12 : FrameResource::CmdLayer::PostD3D12);

        TransitionSceneBufferState(D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET);

        // Draw on scnene buffer.
        auto rtvHandle = SceneRtvHandle();
        auto dsvHandle = SceneDsvHandle();

        cmdList->OMSetRenderTargets(1, &rtvHandle, TRUE, &dsvHandle);
        // Clearing RTV is meaningless for post D3D12 layer since it's drawn on the existed scene.
        if (&target == &preDrawLayerObjects)
        {
            ClearTargetBuffer(rtvHandle, dsvHandle, 1.0f, 0);
        }
        else // However, we still need to clear DSV to make sure post scene is shown on pre scene.
        {
            D3D12_CLEAR_FLAGS clearFlags = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL;
            cmdList->ClearDepthStencilView(dsvHandle, clearFlags, 1.0f, 0, 0, nullptr);
        }

        SharedPtr<ICamera> icam = nullptr;
        // Start drawing D3D12 objects.
        for (auto& layer : target)
        {
            if (layer.first->IsD3D12LayerVisible())
            {
                layer.first->OnRendererDrawD3D12Layer(this);
            }
            // We must handle the draw layer firstly to bind preset resources!
            // For example, if the camera is going to bind a descriptor table,
            // then the root signature must be bound ahead in the the draw layer.
            if ((icam = layer.first->AssociatedCamera()) != nullptr)
            {
                if (icam->IsD3D12ObjectVisible())
                {
                    icam->OnRendererDrawD3D12Object(this);
                }
            }
            // Draw objects in each layer.
            for (auto& obj : layer.second)
            {
                if (obj->IsD3D12ObjectVisible())
                {
                    obj->OnRendererDrawD3D12Object(this);
                }
            }
        }

        TransitionSceneBufferState(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON);

        SubmitCmdList();
    }

    void Renderer::SetSceneColor(const XMVECTORF32& sceneColor)
    {
        this->sceneColor = sceneColor;
        OnWindowResize();
    }

    void Renderer::SetLetterboxColor(const XMVECTORF32& letterboxColor)
    {
        this->letterboxColor = letterboxColor;
        // The letterbox depends on the renderer's letterbox color,
        // so we must recreate it to respond to the setting change.
        if (letterbox != nullptr)
        {
            letterbox = std::make_unique<Letterbox>(this);
        }
        OnWindowResize();
    }

    void Renderer::DrawD2D1Layer()
    {
        CurrFrameResource()->ResetCmdList(cmdList.Get(), FrameResource::CmdLayer::D2D1);

        // Execute the front drawing layers for D2D1 objects,
        // which can be used to prepare some helpful effects.
        for (auto& obj2d : drawObjects2D)
        {
            if (obj2d->IsD2D1ObjectVisible())
            {
                obj2d->OnRendererDrawD2D1Layer(this);
            }
        }

        // Acquire the wrapped buffer related to scene buffer.
        d3d11On12Device->AcquireWrappedResources(wrappedBuffer.GetAddressOf(), 1);
        // Draw D2D1 objects on scene buffer via its wrapped buffer.
        d2d1DeviceContext->SetTarget(d2d1RenderTarget.Get());

        d2d1DeviceContext->BeginDraw();
        d2d1DeviceContext->SetTransform(D2D1::Matrix3x2F::Identity());

        // Start drawing D2D1 objects.
        for (auto& obj2d : drawObjects2D)
        {
            if (obj2d->IsD2D1ObjectVisible())
            {
                obj2d->OnRendererDrawD2D1Object(this);
            }
        }

        THROW_IF_FAILED(d2d1DeviceContext->EndDraw());

        // Releasing will transition scene buffer to designated Out-state.
        d3d11On12Device->ReleaseWrappedResources(wrappedBuffer.GetAddressOf(), 1);
        // Flushing will submit the D3D11 command list to the shared command queue.
        d3d11DeviceContext->Flush();

        SubmitCmdList();
    }

    void Renderer::DrawD3D12LayerMSAA()
    {
        // Note only pre D3D12 layer can be drawin in MSAA mode.

        CurrFrameResource()->ResetCmdList(cmdList.Get(), FrameResource::CmdLayer::PreD3D12);

        // Note MSAA buffer is always in RENDER_TARGET state.

        cmdList->RSSetViewports(1, &camera->Viewport());
        cmdList->RSSetScissorRects(1, &camera->ScissorRect());

        // Draw on MSAA buffer.
        auto rtvHandle = MsaaRtvHandle();
        auto dsvHandle = MsaaDsvHandle();

        cmdList->OMSetRenderTargets(1, &rtvHandle, TRUE, &dsvHandle);
        ClearTargetBuffer(rtvHandle, dsvHandle, 1.0f, 0);

        SharedPtr<ICamera> icam = nullptr;
        // Start drawing D3D12 objects.
        for (auto& layer : preDrawLayerObjects)
        {
            if (layer.first->IsD3D12LayerVisible())
            {
                layer.first->OnRendererDrawD3D12Layer(this);
            }
            // We must handle the draw layer firstly to bind preset resources!
            // For example, if the camera is going to bind a descriptor table,
            // then the root signature must be bound ahead in the the draw layer.
            if ((icam = layer.first->AssociatedCamera()) != nullptr)
            {
                if (icam->IsD3D12ObjectVisible())
                {
                    icam->OnRendererDrawD3D12Object(this);
                }
            }
            // Draw objects in each layer.
            for (auto& obj : layer.second)
            {
                if (obj->IsD3D12ObjectVisible())
                {
                    obj->OnRendererDrawD3D12Object(this);
                }
            }
        }

        // Resolve result from MSAA buffer to scene buffer.
        TransitionMsaaBufferState(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_RESOLVE_SOURCE);
        TransitionSceneBufferState(D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RESOLVE_DEST);

        cmdList->ResolveSubresource(sceneBuffer.Get(), 0, msaaBuffer.Get(), 0, RENDER_TARGET_FORMAT);

        TransitionMsaaBufferState(D3D12_RESOURCE_STATE_RESOLVE_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
        TransitionSceneBufferState(D3D12_RESOURCE_STATE_RESOLVE_DEST, D3D12_RESOURCE_STATE_COMMON);

        SubmitCmdList();
    }

    void Renderer::CopySceneToBackBuffer()
    {
        TransitionBackBufferState(D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_COPY_DEST);
        TransitionSceneBufferState(D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_SOURCE);

        cmdList->CopyTextureRegion(
            &CD3DX12_TEXTURE_COPY_LOCATION(CurrBackBuffer()), 0, 0, 0,
            &CD3DX12_TEXTURE_COPY_LOCATION(sceneBuffer.Get()), &CD3DX12_BOX(0, 0, SceneWidth(), SceneHeight()));

        TransitionBackBufferState(D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PRESENT);
        TransitionSceneBufferState(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COMMON);
    }

    D2D1_ANTIALIAS_MODE Renderer::GetAntialiasMode2D()
    {
        return d2d1DeviceContext->GetAntialiasMode();
    }

    void Renderer::SetAntialiasMode2D(D2D1_ANTIALIAS_MODE mode)
    {
        d2d1DeviceContext->SetAntialiasMode(mode);
    }

    D2D1_TEXT_ANTIALIAS_MODE Renderer::GetTextAntialiasMode()
    {
        return d2d1DeviceContext->GetTextAntialiasMode();
    }

    void Renderer::SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE mode)
    {
        d2d1DeviceContext->SetTextAntialiasMode(mode);
    }

    Renderer::TextRenderingSettings Renderer::GetDefaultTextRenderingMode()
    {
        ComPtr<IDWriteRenderingParams> param;
        THROW_IF_FAILED(dWriteFactory->CreateRenderingParams(&param));

        return
        {
            param->GetGamma(),
            param->GetEnhancedContrast(),
            param->GetClearTypeLevel(),
            param->GetPixelGeometry(),
            param->GetRenderingMode()
        };
    }

    void Renderer::SetTextRenderingMode(const TextRenderingSettings& mode)
    {
        ComPtr<IDWriteRenderingParams> param;
        THROW_IF_FAILED(dWriteFactory->CreateCustomRenderingParams(
            mode.gamma,
            mode.enhancedContrast,
            mode.clearTypeLevel,
            mode.pixelGeometry,
            mode.renderingMode,
            &param));

        d2d1DeviceContext->SetTextRenderingParams(param.Get());
    }
}