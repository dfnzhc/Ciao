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
#include <string>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <assert.h>
#include <stdexcept>
#include <queue>
#include <stack>
#include <array>
#include <numeric>
#include <map>

#include <windows.h>

#include "Core/Common.h"
#include "Core/Log.h"

#include <glad/glad.h>
#include <SDL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

#include <meshoptimizer.h>
