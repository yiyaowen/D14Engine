#include "Precompile.h"

#include "BitmapIO.h"

#include <cstdio>

#pragma warning(push)

// Disable security func warnings.
#pragma warning(disable : 4996)
// Disable buffer read/write warnings.
#pragma warning(disable : 6385)
#pragma warning(disable : 6386)

static BYTE* gCurrPos = NULL;
// We use this to simulate fread in memory block.
static void myMemRead(void* pDst, size_t nByteSize)
{
    memcpy(pDst, gCurrPos, nByteSize);
    gCurrPos += nByteSize;
}
#define PTR_SIZE_ARG(X) &X, sizeof(X)

// Load binary data from file; return NULL if failed.
static BYTE* myLoadFileData(PCWSTR szFileName)
{
    // Try to open file.
    FILE* fp = _wfopen(szFileName, L"rb");
    if (fp == NULL) return NULL;

    // Get file byte size.
    fseek(fp, 0, SEEK_END);
    size_t nByteSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Read file data into memory.
    BYTE* data = (BYTE*)malloc(nByteSize);
    if (data == NULL) return NULL;

    // Check whether all data is loaded.
    if (fread(data, 1, nByteSize, fp) != nByteSize)
    {
        free(data);
        fclose(fp);
        return NULL;
    }
    else
    {
        fclose(fp);
        return data;
    }
}

MyBGRA* myReadBmp(PCWSTR szFileName, MyBmpInfo* pInfo)
{
    // We decide that user must provide a valid info struct.
    if (pInfo == NULL) return NULL;

    BYTE* data = myLoadFileData(szFileName);
    if (data == NULL) return NULL;

    // Initialize memory pointer.
    gCurrPos = data;

    // Get BMP file header.
    BITMAPFILEHEADER bmpfh;
    BITMAPINFOHEADER bmpih;

    myMemRead(PTR_SIZE_ARG(bmpfh));
    myMemRead(PTR_SIZE_ARG(bmpih));

    // BM, Windows
    if (bmpfh.bfType != 0x4D42) return NULL;

    pInfo->nWidth = bmpih.biWidth;
    // BMP's height can be negative, which means the image is flipped.
    pInfo->nHeight = abs(bmpih.biHeight);

    // We don't support image with too large size.
    if (max(pInfo->nWidth, pInfo->nHeight) > 65536) return NULL;

    // Allocate memory for pixel data.
    size_t nPixelCount = pInfo->nWidth * pInfo->nHeight;
    MyBGRA* pixel = (MyBGRA*)malloc(sizeof(MyBGRA) * nPixelCount);
    if (pixel == NULL) goto pixel_data_allocate_failed;

    // The number of bytes in each scan line of BMP is a multiple of 4.
    UINT nRowLength = 4 * ((bmpih.biWidth * bmpih.biBitCount + 31) / 32);

    //-------------------------------------------------------------------------------------------------
    // For 24-bit or 32-bit color mode, the image stores BGR(A) directly.
    //-------------------------------------------------------------------------------------------------
    if (bmpih.biBitCount == 24 || bmpih.biBitCount == 32)
    {
        // Move to pixel data start.
        gCurrPos = data + bmpfh.bfOffBits;

        UINT8 bgr[3];

        // Store as B G R, B G R, B G R ......
        if (bmpih.biBitCount == 24)
        {
            for (UINT j = 0; j < pInfo->nHeight; ++j)
            {
                // 3 Bytes for 1 Pixel
                for (UINT i = 0; i < nRowLength; i += 3)
                {
                    // Skip padded zero bytes.
                    if (i / 3 >= pInfo->nWidth)
                    {
                        gCurrPos += (nRowLength - i);
                        break; // Move to next scan line.
                    }
                    myMemRead(bgr, sizeof(UINT8) * 3);

                    // Get pixel index in data.
                    UINT idx = i / 3 + j * pInfo->nWidth;

                    pixel[idx].B = bgr[0];
                    pixel[idx].G = bgr[1];
                    pixel[idx].R = bgr[2];
                    pixel[idx].A = 255; // Keep opaque by default.
                }
            }
        }
        // Store as B G R A, B G R A, B G R A ......
        else
        {
            myMemRead(pixel, sizeof(MyBGRA) * nPixelCount);
        }
    }
    //-------------------------------------------------------------------------------------------------
    // For 16-bit color mode, the image stores BGR as 5 (B), 5 (G), 5 (R), 1 (reserved).
    //-------------------------------------------------------------------------------------------------
    else if (bmpih.biBitCount == 16)
    {
        // We decide not to support BMP with compression format.
        if (bmpih.biCompression != BI_RGB) return NULL;

        // Move to pixel data start.
        gCurrPos = data + bmpfh.bfOffBits;

        UINT16 bgr;
        UINT8 tmpB, tmpG, tmpR;

        for (UINT j = 0; j < pInfo->nHeight; ++j)
        {
            // 2 Bytes for 1 Pixel
            for (UINT i = 0; i < nRowLength; i += 2)
            {
                myMemRead(PTR_SIZE_ARG(bgr));
                // Skip padded zero bytes.
                if (i / 2 >= pInfo->nWidth) continue;

                // Get pixel index in data.
                UINT idx = i / 2 + j * pInfo->nWidth;

                tmpB = (bgr & 0x001F) >> 0; // 0000 0000 0001 1111
                tmpG = (bgr & 0x03E0) >> 5; // 0000 0011 1110 0000
                tmpR = (bgr & 0x7C00) >> 10;// 0111 1100 0000 0000

#define MY_RGB_555_TO_888(X) (((X) << 3) + ((X) & 0x07))

                pixel[idx].B = MY_RGB_555_TO_888(tmpB);
                pixel[idx].G = MY_RGB_555_TO_888(tmpG);
                pixel[idx].R = MY_RGB_555_TO_888(tmpR);
                pixel[idx].A = 255; // Keep opaque by default.

#undef MY_RGB_555_TO_888
            }
        }
    }
    //-------------------------------------------------------------------------------------------------
    // For other modes, the image stores color indices.
    //-------------------------------------------------------------------------------------------------
    else
    {
        // Create indexed color palette.
        UINT nColorCount = 2 << bmpih.biBitCount;
        MyBGRA* palette = (MyBGRA*)malloc(sizeof(MyBGRA) * nColorCount);
        if (palette == NULL) goto palette_data_create_failed;
        myMemRead(palette, sizeof(MyBGRA) * nColorCount);

        // Move to pixel data start.
        gCurrPos = data + bmpfh.bfOffBits;

        UINT8 coloridx;

        // 1 Byte for 8 Pixels
        if (bmpih.biBitCount == 1)
        {
            for (UINT j = 0; j < pInfo->nHeight; ++j)
            {
                for (UINT i = 0; i < nRowLength; ++i)
                {
                    // Get color index in palette.
                    myMemRead(PTR_SIZE_ARG(coloridx));
                    // Get pixel index in data.
                    UINT idx = 8 * i + j * pInfo->nWidth;

#define MY_MONOCHROME(Bitpos) pixel[idx + (Bitpos)] = palette[(coloridx >> (7 - (Bitpos))) & 0x01];

                    // Handle remainder at line end.
                    if (8 * i >= pInfo->nWidth) continue;
                    else if (8 * i < pInfo->nWidth && 8 * (i + 1) > pInfo->nWidth)
                    {
                        for (UINT k = 0; k < (pInfo->nWidth - 8 * i); ++k)
                        {
                            MY_MONOCHROME(k)
                        }
                        continue;
                    }

                    // Populate pixel with monochrome byte.
                    for (UINT k = 0; k < 8; ++k) MY_MONOCHROME(k)

#undef MY_MONOCHROME
                }
            }
        }
        // 1 Byte for 2 Pixels
        else if (bmpih.biBitCount == 4)
        {
            for (UINT j = 0; j < pInfo->nHeight; ++j)
            {
                for (UINT i = 0; i < nRowLength; ++i)
                {
                    // Get color index in palette.
                    myMemRead(PTR_SIZE_ARG(coloridx));
                    // Get pixel index in data.
                    UINT idx = 2 * i + j * pInfo->nWidth;

                    // Don't forget the last pixel!
                    if (2 * i + 1 == pInfo->nWidth)
                    {
                        pixel[idx] = palette[coloridx >> 4];
                    }
                    // Skip padded zero bytes.
                    if (2 * i + 1 >= pInfo->nWidth) continue;

                    // Populate pixel with color in palette.
                    pixel[idx] = palette[coloridx >> 4];
                    pixel[idx + 1] = palette[coloridx & 0x0F];
                }
            }
        }
        // 1 Byte for 1 Pixel
        else if (bmpih.biBitCount == 8)
        {
            for (UINT j = 0; j < pInfo->nHeight; ++j)
            {
                for (UINT i = 0; i < nRowLength; ++i)
                {
                    // Get color index in palette.
                    myMemRead(PTR_SIZE_ARG(coloridx));
                    // Skip padded zero bytes.
                    if (i >= pInfo->nWidth) continue;

                    // Get pixel index in data.
                    UINT idx = i + j * pInfo->nWidth;

                    pixel[idx] = palette[coloridx];
                }
            }
        }
        else { free(palette); goto unknown_bit_count_encountered; }

        // Note we have allocated memory for palette data.
        free(palette);
    }

    // Note we have allocated memory for file data.
    free(data);
    return pixel;

palette_data_create_failed:
unknown_bit_count_encountered:
    free(pixel);

pixel_data_allocate_failed:
    free(data);
    return NULL;
}

BOOL myWriteBmp(PCWSTR szFileName, const MyBGRA* pData, const MyBmpInfo* pInfo)
{
    FILE* fp = _wfopen(szFileName, L"wb");

    // Set BMP file header.
    BITMAPFILEHEADER bmpfh;
    bmpfh.bfType = 0x4D42; // BM, Windows
    bmpfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    // The image stores BGR(A) directly in 32-bit color mode.
    size_t nPixelDataByteSize = sizeof(MyBGRA) * pInfo->nWidth * pInfo->nHeight;
    bmpfh.bfSize = bmpfh.bfOffBits + (DWORD)nPixelDataByteSize;
    bmpfh.bfReserved1 = bmpfh.bfReserved2 = 0;

    BITMAPINFOHEADER bmpih;
    bmpih.biSize = sizeof(BITMAPINFOHEADER);
    bmpih.biWidth = pInfo->nWidth;
    bmpih.biHeight = pInfo->nHeight;
    bmpih.biPlanes = 1; // always be 1
    bmpih.biBitCount = 32; // true color
    bmpih.biCompression = BI_RGB; // no compression
    bmpih.biSizeImage = 0; // could be 0 for RGB
    // We don't care about these fields.
    bmpih.biXPelsPerMeter = bmpih.biYPelsPerMeter = 0;
    bmpih.biClrUsed = bmpih.biClrImportant = 0;

    fwrite(PTR_SIZE_ARG(bmpfh), 1, fp);
    fwrite(PTR_SIZE_ARG(bmpih), 1, fp);

    fwrite(pData, nPixelDataByteSize, 1, fp);

    // Whether closed successfully. EOF(-1) if failed.
    return fclose(fp) == 0;
}

BOOL myWriteTxt(PCWSTR szFileName, const MyBGRA* pData, const MyBmpInfo* pInfo)
{
    FILE* fp = _wfopen(szFileName, L"w");

    fprintf(fp, "width\theight\n");
    fprintf(fp, "%d\t%d\n", pInfo->nWidth, pInfo->nHeight);

    fprintf(fp, "R\tG\tB\n");
    for (UINT i = 0; i < pInfo->nWidth; ++i)
        for (UINT j = 0; j < pInfo->nHeight; ++j)
        {
            UINT idx = i + j * pInfo->nHeight;
            // Discard alpha-channel.
            fprintf(fp, "%d\t%d\t%d\n",
                    pData[idx].R, pData[idx].G, pData[idx].B);
        }

    // Whether closed successfully. EOF(-1) if failed.
    return fclose(fp) == 0;
}

#pragma warning(pop)