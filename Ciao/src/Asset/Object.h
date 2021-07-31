#pragma once

namespace Ciao
{
    class ShaderProgram;
    class Object
    {
    public:
        virtual ~Object() = default;

        virtual void Create() = 0;
        virtual void Draw(std::shared_ptr<ShaderProgram>) = 0;
        virtual void Release() = 0;
    };
}


