#pragma once
#include "Object.h"

namespace Ciao
{
    class Grid : public Object
    {
    public:
        void Create() override;
        void Draw(std::shared_ptr<ShaderProgram>) override;
        void Release() override;

    private:
        GLuint vao;
    };
}
