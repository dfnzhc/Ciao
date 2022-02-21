#pragma once

namespace Ciao
{
    class Mesh
    {
    public:
        Mesh();
        ~Mesh();

        void BuildBVH();
        bool LoadFromFile(const std::string& filename);

        std::vector<vec4> verticesUVX_;  // Vertex + texture Coord (u/s)
        std::vector<vec4> normalsUVY_;   // Normal + texture Coord (v/t)

        std::string name_;
    };

    class MeshInstance
    {
    public:
        MeshInstance(std::string name, int meshId, mat4 xform, int matId);
        ~MeshInstance() {}

        mat4 transform_;
        std::string name_;

        int materialID_;
        int meshID_;
    };
}
