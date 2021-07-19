#pragma once

namespace Ciao
{
    class Object
    {
    public:
        virtual ~Object() = default;

        virtual void Create() = 0;
        virtual void Draw() = 0;
        virtual void Release() = 0;
    };
}


