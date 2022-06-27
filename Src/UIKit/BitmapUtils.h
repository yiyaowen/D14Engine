#pragma once

#include "Common/Precompile.h"

namespace d14engine::uikit
{
    struct BitmapUtils
    {
        static void Initialize();

        static ComPtr<ID2D1BitmapBrush1> BitmapToBrush(ID2D1Bitmap1* texture);

        static ComPtr<ID2D1Bitmap1> LoadBitmapFromFile(WstrParam imageFile, WstrParam assetsPath = L"");

        static ComPtr<ID2D1Bitmap1> LoadBitmapFromMemory( // Set data to nullptr to create an uninitialized bitmap.
            UINT width, UINT height, BYTE* data = nullptr, D2D1_BITMAP_OPTIONS options = D2D1_BITMAP_OPTIONS_NONE);

        static ComPtr<ID2D1BitmapBrush1> LoadBrushFromFile(WstrParam imageFile, WstrParam assetsPath = L"");

        static ComPtr<ID2D1BitmapBrush1> LoadBrushFromMemory(UINT width, UINT height, BYTE* data = nullptr);
    
    private:
        static ComPtr<IWICImagingFactory> g_imageFactory;
    };
    // Introduce this alias to avoid too long prefix.
    using Bitmapu = BitmapUtils;
}