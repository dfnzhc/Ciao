#pragma once

#if defined(_MSC_VER)
    /* Disable some warnings on MSVC++ */
    #pragma warning(disable : 4127 4702 4100 4515 4800 4146 4512)
    #define WIN32_LEAN_AND_MEAN     /* Don't ever include MFC on Windows */
    #define NOMINMAX                /* Don't override min/max */
#endif

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

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <fstream>
#include <execution>
#include <filesystem>
#include <string>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
#include <queue>
#include <stack>
#include <array>
#include <numeric>
#include <map>
#include <windows.h>
#include <limits>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_internal.h>

#include <tbb/tbb.h>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

#include <meshoptimizer.h>

#include "core/Logger.h"
#include "utils/Common.h"
#include "utils/Timer.h"