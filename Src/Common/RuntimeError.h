#pragma once

#include "Common/Precompile.h"

namespace d14engine
{
    struct RuntimeError
    {
        // __FILEW__
        Wstring fileName;
        // __LINE__
        UINT lineNum = 0;

        Wstring descText;

        RuntimeError(WstrParam fileName, UINT lineNum, WstrParam descText = L"")
            : fileName(fileName), lineNum(lineNum), descText(descText) { }

        virtual Wstring Message()
        {
            return L"In file: " + fileName + L", line " + std::to_wstring(lineNum) + L", detail: " + descText;
        }
    };

#define THROW_ERROR(Desc_Text) throw RuntimeError(__FILEW__, __LINE__, Desc_Text)

#define THROW_IF_NULL(Expression) \
do { \
    if ((Expression) == nullptr) \
    { \
        throw RuntimeError(__FILEW__, __LINE__, L"Unexpected nullptr encountered."); \
    } \
} while (0)

    struct DirectXError : RuntimeError
    {
        HRESULT hr = E_UNEXPECTED;

        DirectXError(HRESULT hr, WstrParam fileName, UINT lineNum)
            : RuntimeError(fileName, lineNum), hr(hr)
        {
            descText = _com_error(hr).ErrorMessage();
        }
    };

#define THROW_IF_FAILED(Expression) \
do { \
    HRESULT d14_hr = (Expression); \
    if (!SUCCEEDED(d14_hr)) \
    { \
        throw DirectXError(d14_hr, __FILEW__, __LINE__); \
    } \
} while (0)

}