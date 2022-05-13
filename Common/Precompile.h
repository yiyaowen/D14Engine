#pragma once

// Standard Library
#include <algorithm>
#include <array>
#include <exception>
#include <functional>
#include <iterator>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace d14engine
{
    template<typename T>
    using Function = std::function<T>;

    template<typename T>
    using SharedPtr = std::shared_ptr<T>;
    template<typename T>
    using ShrdPtrParam = const SharedPtr<T>&;

    template<typename T>
    using WeakPtr = std::weak_ptr<T>;
    template<typename T>
    using WeakPtrParam = const WeakPtr<T>&;

    template<typename T>
    using UniquePtr = std::unique_ptr<T>;
    template<typename T>
    using UniqPtrParam = const UniquePtr<T>&;

    template<typename T>
    using Optional = std::optional<T>;
    template<typename T>
    using OptParam = const Optional<T>&;

    using String = std::string;
    using StrParam = const String&;

    using StringView = std::string_view;
    using StrViewParam = const StringView&;

    using Wstring = std::wstring;
    using WstrParam = const Wstring&;

    using WstringView = std::wstring_view;
    using WstrViewParam = const WstringView&;
}

// Windows & DirectX SDK
#include <comdef.h>
#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1effects_2.h>
#include <d3d11.h>
#include <d3d11on12.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <dwmapi.h>
#include <dwrite_3.h>
#include <dxgi1_6.h>
#include <wincodec.h>
#include <Windows.h>
#include <windowsx.h>
#include <wrl.h>

#include "DirectX/d3dx12.h"

#ifdef _DEBUG
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

namespace d14engine
{
    // We will use lots of components in DirectX SDK,
    // so we decide to import the whole namespace at once.
    using namespace DirectX;

    using Microsoft::WRL::ComPtr;
    template<typename T>
    using ComPtrParam = const ComPtr<T>&;
}

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "D3D11.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "Dwmapi.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "WindowsCodecs.lib")