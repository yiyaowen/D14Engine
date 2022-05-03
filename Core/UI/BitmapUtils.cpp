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
        THROW_IF_FAILED(Application::RNDR->d2d1DeviceContext->CreateBitmapBrush(texture, &brush));
        return brush;
    }

    ComPtr<ID2D1Bitmap1> BitmapUtils::LoadBitmapFromFile(
        WstrParam imageFile,
        WstrParam assetsPath,
        bool loadWithBitmapIO)
    {
        if (loadWithBitmapIO)
        {
            MyBGRA* data = nullptr;
            MyBmpInfo info = {};

            // >>> Bitmap reader from yiyaowen.BitmapViewer project. <<<
            THROW_IF_NULL(data = myReadBmp((assetsPath + imageFile).c_str(), &info));

            // B8G8R8A8 format only supports premultiplied alpha mode!
            for (UINT i = 0; i < info.nWidth; ++i)
            {
                for (UINT j = 0; j < info.nHeight; ++j)
                {
                    UINT idx = i + j * info.nWidth;
                    float ratio = data[idx].A / 255.0f;
                    data[idx].R = (UINT8)(data[idx].R * ratio);
                    data[idx].G = (UINT8)(data[idx].G * ratio);
                    data[idx].B = (UINT8)(data[idx].B * ratio);
                }
            }
            auto bitmap = LoadBitmapFromMemory((BYTE*)data, info.nWidth, info.nHeight);

            // To be compatible with myReadBmp's malloc.
            free(data);

            return bitmap;
        }
        else // Load with WIC.
        {
            ComPtr<IWICBitmapDecoder> decoder;
            THROW_IF_FAILED(g_imageFactory->CreateDecoderFromFilename(
                (assetsPath + imageFile).c_str(),   // Image to be decoded.
                nullptr,                            // Don't prefer a particular vendor.
                GENERIC_READ,                       // Desired read access to the file.
                WICDecodeMetadataCacheOnDemand,     // Only cache metadata when needed.
                &decoder));                         // Pointer to the decoder.

            ComPtr<IWICBitmapFrameDecode> frameDecode;
            THROW_IF_FAILED(decoder->GetFrame(0, &frameDecode));

            ComPtr<IWICFormatConverter> formatConverter;
            THROW_IF_FAILED(g_imageFactory->CreateFormatConverter(&formatConverter));

            THROW_IF_FAILED(formatConverter->Initialize(
                frameDecode.Get(),              // Input bitmap to convert.
                GUID_WICPixelFormat32bppPBGRA,  // Destination pixel format.
                WICBitmapDitherTypeNone,        // Specified dither pattern.
                nullptr,                        // Specify a particular palette.
                0.0f,                           // Alpha threshold.
                WICBitmapPaletteTypeCustom));   // Palette translation type.

            ComPtr<ID2D1Bitmap1> bitmap;
            THROW_IF_FAILED(Application::RNDR->d2d1DeviceContext->CreateBitmapFromWicBitmap(
                formatConverter.Get(),  // WIC bitmap source.
                nullptr,                // Bitmap properties.
                &bitmap));              // Destination bitmap.

            return bitmap;
        }
    }

    ComPtr<ID2D1Bitmap1> BitmapUtils::LoadBitmapFromMemory(
        BYTE* data,
        UINT width,
        UINT height,
        D2D1_BITMAP_OPTIONS options)
    {
        // Creating D2D1 bitmap depends on window dpi info.
        FLOAT dpi = (FLOAT)GetDpiForWindow(Application::RNDR->window.ptr);

        D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(
            options, // Create as common texture by default.
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            dpi, dpi); // Simply use the same DPI for x,y-axes.

        ComPtr<ID2D1Bitmap1> bitmap;
        // To simplify the method, we decide that the pixel format must be B8G8R8A8;
        // thus the pitch (byte count of each scanline) can be calculated as 4 * width.
        THROW_IF_FAILED(Application::RNDR->d2d1DeviceContext->CreateBitmap(
            { width, height },  // Image size.
            data,               // Image pixel data.
            4 * width,          // Pitch, byte size of each scanline.
            props,              // Bitmap properties.
            &bitmap));          // Destination bitmap.

        return bitmap;
    }

    ComPtr<ID2D1BitmapBrush1> BitmapUtils::LoadBrushFromFile(
        WstrParam imageFile,
        WstrParam assetsPath,
        bool loadWithBitmapIO)
    {
        return BitmapToBrush(LoadBitmapFromFile(imageFile, assetsPath, loadWithBitmapIO).Get());
    }

    ComPtr<ID2D1BitmapBrush1> BitmapUtils::LoadBrushFromMemory(
        BYTE* data,
        UINT width,
        UINT height)
    {
        return BitmapToBrush(LoadBitmapFromMemory(data, width, height).Get());
    }

    ComPtr<ID2D1Bitmap1> BitmapUtils::CreateBytePaddingBitmap(
        BYTE padding,
        UINT width,
        UINT height,
        D2D1_BITMAP_OPTIONS options)
    {
        UINT pixelCount = width * height;
        size_t byteSize = 4ull * pixelCount;

        std::unique_ptr<BYTE> data(new BYTE[byteSize]);
        FillMemory(data.get(), byteSize, padding);

        return LoadBitmapFromMemory(data.get(), width, height, options);
    }

    ComPtr<ID2D1Bitmap1> BitmapUtils::CreateSolidColorBitmap(
        const D2D1_COLOR_F& color,
        UINT width,
        UINT height,
        D2D1_BITMAP_OPTIONS options)
    {
        UINT pixelCount = width * height;
        size_t byteSize = 4ull * pixelCount;

        auto b = (UINT8)(color.b * 255.0f);
        auto g = (UINT8)(color.g * 255.0f);
        auto r = (UINT8)(color.r * 255.0f);
        auto a = (UINT8)(color.a * 255.0f);

        std::unique_ptr<BYTE> data(new BYTE[byteSize]);

        // TODO: find an optimized way of padding.
        for (UINT i = 0; i < pixelCount; ++i)
        {
            data.get()[4 * i + 0] = b;
            data.get()[4 * i + 1] = g;
            data.get()[4 * i + 2] = r;
            data.get()[4 * i + 3] = a;
        }

        return LoadBitmapFromMemory(data.get(), width, height, options);
    }
}