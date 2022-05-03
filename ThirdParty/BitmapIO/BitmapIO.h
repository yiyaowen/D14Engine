#pragma once

// In case that MinGW doesn't define this for us.
#ifndef UNICODE
#define UNICODE
#endif
// The same for MSVC and MinGW.
#include <windows.h>

typedef struct tagMyBGRA
{
    UINT8 B, G, R, A;
}
MyBGRA; // 32-bit true color

typedef struct tagMyBmpInfo
{
    UINT nWidth, nHeight;
}
MyBmpInfo;

//-------------------------------------------------------------------------------------------------
// Bimap Read Subroutine
//-------------------------------------------------------------------------------------------------

// Load BMP image file from disk.
// 
// @param szFileName: path to BMP image file.
// @param pInfo: output the BMP image info.
// 
// @return pixel data of BMP Image; NULL if failed.
//
MyBGRA* myReadBmp(PCWSTR szFileName, MyBmpInfo* pInfo);

//-------------------------------------------------------------------------------------------------
// Bimap Write Subroutine
//-------------------------------------------------------------------------------------------------

// Save BMP image data as file. Only support 32-bit true color BMP format.
//
// @param szFileName: path to BMP image file.
// @param pData: raw pixel data of BMP image.
// @param pInfo: input the BMP image info.
//
// @return whether the file saved successfully.
//
BOOL myWriteBmp(PCWSTR szFileName, const MyBGRA* pData, const MyBmpInfo* pInfo);

// Save BMP image data as text file. One pixel (3-channel) for one line.
BOOL myWriteTxt(PCWSTR szFileName, const MyBGRA* pData, const MyBmpInfo* pInfo);