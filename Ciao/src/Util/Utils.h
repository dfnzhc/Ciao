#pragma once


namespace Ciao
{
    struct Bitmap;
    
    Bitmap convertEquirectangularMapToVerticalCross(const Bitmap& b);
    Bitmap convertVerticalCrossToCubeMapFaces(const Bitmap& b);
    
    int getNumMipMapLevels2D(int w, int h);

    void imguiTextureWindowGL(const char* title, uint32_t texId);

}
