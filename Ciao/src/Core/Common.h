#pragma once

#if defined(_MSC_VER)
    /* Disable some warnings on MSVC++ */
    #pragma warning(disable : 4127 4702 4100 4515 4800 4146 4512)
    #define WIN32_LEAN_AND_MEAN     /* Don't ever include MFC on Windows */
    #define NOMINMAX                /* Don't override min/max */
#endif

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <fstream>
#include <string>
#include <string>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <assert.h>
#include <stdexcept>
#include <queue>
#include <stack>
#include <array>

#ifdef CIAO_DEBUG
    #if defined(CIAO_PLATFORM_WINDOWS)
        #define CIAO_BREAK() __debugbreak();
    #else
        #define CIAO_BREAK()
    #endif
    #define CIAO_ENABLE_ASSERT
#else
    #define CIAO_BREAK()
#endif


#if defined(CIAO_PLATFORM_WINDOWS)
    #include <windows.h>
#endif

#define Epsilon 1e-4f

#define M_PI         3.14159265358979323846f
#define INV_PI       0.31830988618379067154f
#define INV_TWOPI    0.15915494309189533577f
#define INV_FOURPI   0.07957747154594766788f
#define SQRT_TWO     1.41421356237309504880f
#define INV_SQRT_TWO 0.70710678118654752440f

namespace Ciao {

    template<typename T>
    using Scope = std::unique_ptr<T>;
    
    template<typename T, typename ... Args>
    constexpr Scope<T> CreateScope(Args&& ... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    using Ref = std::shared_ptr<T>;

    template<typename T, typename ... Args>
    constexpr Ref<T> CreateRef(Args&& ... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
    
}