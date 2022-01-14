#pragma once


namespace Ciao
{
    class GLTexture
    {
    public:
        GLTexture() = default;
        GLTexture(std::string texName, unsigned char* data, int w, int h, int c);
        ~GLTexture() { }
        
        bool loadTexture(const std::string_view& filename);

    private:
        int width_ = 0, height_ = 0, components_ = 0;

        std::vector<unsigned char> data_;
        std::string name_ = "";
    };
}
