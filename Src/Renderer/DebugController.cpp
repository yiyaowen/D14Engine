#include "Common/Precompile.h"

#include "Renderer/DebugController.h"

namespace d14engine::renderer
{
    void DebugController::EnableD3D12DebugLayer()
    {
        ComPtr<ID3D12Debug> debugController;
        THROW_IF_FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
        debugController->EnableDebugLayer();
    }

    void DebugController::SuppressUselessWarnings(Renderer* rndr)
    {
        ComPtr<ID3D12InfoQueue> infoQueue;
        THROW_IF_FAILED(rndr->device.As(&infoQueue));

        D3D12_MESSAGE_ID hide[] =
        {
            /*
            * DirectX will swear that the D3D12 resource is not sharable with D3D11 layer
            * when we try to create a wrapped resource based on D3D11On12 mechanism.
            * However, the operation returns S_OK and everything works well, so we guess
            * this might be a bug of D3D12 interop library, and many online blogs also indicate this.
            */
            D3D12_MESSAGE_ID_REFLECTSHAREDPROPERTIES_INVALIDOBJECT,
            /*
            * It seems that D2D1's FillGeometry series method would call ClearRenderTarget internally;
            * however, we usually need to draw a different color on the existed scene buffer,
            * which would cause DirectX swear that "mismatching clear value leads to slow operation".
            */
            D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
            /*
            * After introducing the front drawing layer for D2D1 objects,
            * some D2D1 effects could cause DirectX to swear about the error.
            * We simply suppress this since read/write unbound DSV is valid.
            */
            D3D12_MESSAGE_ID_CREATEGRAPHICSPIPELINESTATE_DEPTHSTENCILVIEW_NOT_SET,
            /*
            * This warning may occur when recreate a D2D1 resource (e.g. ID2D1Bitmap) that was bound to
            * a render target in previous render passes, even if all states have already been cleared.
            * I guess the descriptor clearing is delayed after the associated resources are destroyed.
            */
            D3D12_MESSAGE_ID_DESCRIPTOR_HANDLE_WITH_INVALID_RESOURCE,
            /*
            * The application allows drawing empty scissor rectangle, so we simply ignore this warning.
            * For example, D2D1 needs to draw an empty rectangle (0,0,0,0) if minimal-size is (0,0).
            */
            D3D12_MESSAGE_ID_DRAW_EMPTY_SCISSOR_RECTANGLE
        };

        D3D12_INFO_QUEUE_FILTER filter = {};
        filter.DenyList.NumIDs = _countof(hide);
        filter.DenyList.pIDList = hide;

        THROW_IF_FAILED(infoQueue->AddStorageFilterEntries(&filter));
    }

    ComPtr<IDXGIDebug> DebugController::QueryDxgiDebugInterface()
    {
        typedef HRESULT(__stdcall* fPtr)(const IID&, void**);

        // There's no LIB provided for dxgidebug, so we have to load it from DLL.
        HMODULE hDll = LoadLibrary(L"dxgidebug.dll");
        THROW_IF_NULL(hDll);

        fPtr DXGIGetDebugInterface = (fPtr)GetProcAddress(hDll, "DXGIGetDebugInterface");
        THROW_IF_NULL(DXGIGetDebugInterface);

        ComPtr<IDXGIDebug> dxgiDebugController;
        DXGIGetDebugInterface(IID_PPV_ARGS(&dxgiDebugController));

        // We won't free the library during the program's lifecycle,
        // because obviously we only need this in DEBUG environment
        // and the DXGI debug controller will be referenced later.
        return dxgiDebugController;
    }
}