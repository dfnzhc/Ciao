#pragma once

namespace Ciao
{
    struct RenderOptions
    {
        vec2 renderResolution;
        vec2 windowResolution;
        vec3 uniformLightCol;
        vec3 backgroundCol;
        int tileWidth;
        int tileHeight;
        
        
        int texArrayWidth;
        int texArrayHeight;
        
    };
}
