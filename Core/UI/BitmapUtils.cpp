#include "Precompile.h"

#include "UI/BitmapUtils.h"

#include "UI/Application.h"

namespace d14engine::ui
{
    ComPtr<IWICImagingFactory> BitmapUtils::g_imageFactory;

    void BitmapUtils::Initialize()
    {
        THROW_IF_FAILED(CoInitializeEx(
            nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE));

        THROW_IF_FAILED(CoCreateInstance(
            CLSID_WICImagingFactory,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&g_imageFactory)));
    }

    ComPtr<ID2D1BitmapBrush1> BitmapUtils::BitmapToBrush(ID2D1Bitmap1* texture)
    {
        ComPtr<ID2D1BitmapBrush1> brush;
        THROW_IF_FAILED(Application::APP->MainRenderer()->d2d1DeviceContext->CreateBitmapBrush(texture, &brush));
        return brush;
    }

    ComPtr<ID2D1Bitmap1> BitmapUtils::LoadBitmapFromFile(WstrParam imageFile, WstrParam assetsPath)
    {
        ComPtr<IWICBitmapDecoder> decoder;
        THROW_IF_FAILED(g_imageFactory->CreateDecoderFromFilename(
            (assetsPath + imageFile).c_str(),
            nullptr,
            GENERIC_READ,
            WICDecodeMetadataCacheOnDemand,
            &decoder));

        ComPtr<IWICBitmapFrameDecode> frameDecode;
        THROW_IF_FAILED(decoder->GetFrame(0, &frameDecode));

        ComPtr<IWICFormatConverter> formatConverter;
        THROW_IF_FAILED(g_imageFactory->CreateFormatConverter(&formatConverter));

        THROW_IF_FAILED(formatConverter->Initialize(
            frameDecode.Get(),
            GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone,
            nullptr,
            0.0f,
            WICBitmapPaletteTypeCustom));

        ComPtr<ID2D1Bitmap1> bitmap;
        THROW_IF_FAILED(Application::APP->MainRenderer()->d2d1DeviceContext->CreateBitmapFromWicBitmap(
            formatConverter.Get(),
            nullptr,
            &bitmap));

        return bitmap;
    }

    ComPtr<ID2D1Bitmap1> BitmapUtils::LoadBitmapFromMemory(
        UINT width, UINT height, BYTE* data, D2D1_BITMAP_OPTIONS options)
    {
        // Creating D2D1 bitmap depends on window dpi info.
        FLOAT dpi = (FLOAT)GetDpiForWindow(Application::APP->MainRenderer()->window.ptr);

        D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(
            options, // Create as common texture by default.
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            dpi, dpi); // Simply use the same DPI for x,y-axes.

        ComPtr<ID2D1Bitmap1> bitmap;
        // To simplify the method, we decide that the pixel format must be B8G8R8A8;
        // thus the pitch (byte count of each scanline) can be calculated as 4 * width.
        THROW_IF_FAILED(Application::APP->MainRenderer()->d2d1DeviceContext->CreateBitmap(
            { width, height },
            data,
            4 * width,
            props,
            &bitmap));

        return bitmap;
    }

    ComPtr<ID2D1BitmapBrush1> BitmapUtils::LoadBrushFromFile(WstrParam imageFile, WstrParam assetsPath)
    {
        return BitmapToBrush(LoadBitmapFromFile(imageFile, assetsPath).Get());
    }

    ComPtr<ID2D1BitmapBrush1> BitmapUtils::LoadBrushFromMemory(UINT width, UINT height, BYTE* data)
    {
        return BitmapToBrush(LoadBitmapFromMemory(width, height, data).Get());
    }
}