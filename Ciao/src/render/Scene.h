#pragma once
#include "Camera.h"
#include "EnvironmentMap.h"
#include "GLTexture.h"
#include "Material.h"
#include "Mesh.h"
#include "RenderOption.h"

namespace Ciao
{
    enum LightType
    {
        RectLight,
        SphereLight,
        DistantLight
    };

    struct Light
    {
        vec3 position;
        vec3 emission;
        vec3 u;
        vec3 v;
        float radius;
        float area;
        float type;
    };

    struct Indices
    {
        int x, y, z;
    };
    
    class Scene
    {
    public:
        Scene() : camera(nullptr), envMap(nullptr), initialized(false), dirty(true) {
        }
        ~Scene();

        int AddMesh(const std::string& filename);
        int AddTexture(const std::string& filename);
        int AddMaterial(const Material& material);
        int AddMeshInstance(const MeshInstance& meshInstance);
        int AddLight(const Light& light);

        void AddCamera(vec3 eye, vec3 lookat, float fov);
        void AddEnvMap(const std::string& filename);

        void ProcessScene();
        void RebuildInstances();

        // Options
        RenderOptions renderOptions;

        // Meshes
        std::vector<Mesh*> meshes;

        // Scene Mesh Data 
        std::vector<Indices> vertIndices;
        std::vector<vec4> verticesUVX; // Vertex + texture Coord (u/s)
        std::vector<vec4> normalsUVY; // Normal + texture Coord (v/t)
        std::vector<mat4> transforms;

        // Materials
        std::vector<Material> materials;

        // Instances
        std::vector<MeshInstance> meshInstances;

        // Lights
        std::vector<Light> lights;

        // Environment Map
        EnvironmentMap* envMap;

        // Camera
        Camera* camera;

        // Texture Data
        std::vector<GLTexture*> textures;
        std::vector<unsigned char> textureMapsArray;

        bool initialized;
        bool dirty;
        // To check if scene elements need to be resent to GPU
        bool instancesModified = false;
        bool envMapModified = false;
    };
}
