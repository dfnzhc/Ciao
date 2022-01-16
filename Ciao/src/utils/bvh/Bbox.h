#pragma once

namespace Ciao
{
    class Bbox
    {
    public:
        Bbox();
        Bbox(const vec3& p);
        Bbox(const vec3& p1, const vec3& p2);

        


        vec3 pMin_;
        vec3 pMax_;
    };
}
