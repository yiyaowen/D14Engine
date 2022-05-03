#pragma once

#include "Precompile.h"

namespace d14engine::ui
{
    struct BitmapUtils
    {
        static void Initialize();

        static ComPtr<ID2D1BitmapBrush1> BitmapToBrush(ID2D1Bitmap1* texture);

        static ComPtr<ID2D1Bitmap1> LoadBitmapFromFile(
            WstrParam imageFile,
            WstrParam assetsPath = L"",
            bool loadWithBitmapIO = false);

        static ComPtr<ID2D1Bitmap1> LoadBitmapFromMemory(
            BYTE* data,
            UINT width,
            UINT height,
            D2D1_BITMAP_OPTIONS options = D2D1_BITMAP_OPTIONS_NONE);

        static ComPtr<ID2D1BitmapBrush1> LoadBrushFromFile(
            WstrParam imageFile,
            WstrParam assetsPath,
            bool loadWithBitmapIO = false);

        static ComPtr<ID2D1BitmapBrush1> LoadBrushFromMemory(
            BYTE* data,
            UINT width,
            UINT height);

        static ComPtr<ID2D1Bitmap1> CreateBytePaddingBitmap(
            BYTE padding,
            UINT width,
            UINT height,
            D2D1_BITMAP_OPTIONS options = D2D1_BITMAP_OPTIONS_NONE);

        static ComPtr<ID2D1Bitmap1> CreateSolidColorBitmap(
            const D2D1_COLOR_F& color,
            UINT width,
            UINT height,
            D2D1_BITMAP_OPTIONS options = D2D1_BITMAP_OPTIONS_NONE);

    private:
        static ComPtr<IWICImagingFactory> g_imageFactory;
    };
    // Introduce this alias to avoid too long prefix.
    using Bitmapu = BitmapUtils;
}