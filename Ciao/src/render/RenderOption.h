#pragma once

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
    };

    
}
