#pragma once
#include <filesystem>

namespace Ciao
{
    struct RenderOptions
    {
    };

    struct CameraConfig
    {
        int pitch;
        int yaw;
    };

    struct MeshConfig
    {
        std::string name;
        std::string source;
        std::string meshDataPath;
        std::string meshData;
        std::string materialData;

        inline bool hasReadyMadeData() const
        {
            return std::filesystem::exists(Res(meshDataPath)) &&
                std::filesystem::exists(Res(meshData)) &&
                std::filesystem::exists(Res(materialData));
        }
    };
}
