#pragma once
#include "Utils/UtilsMath.h"
#include "Renderer/Shader.h"

using namespace glm;

namespace Ciao
{
    struct VertexData
    {
        glm::vec3 pos;
        glm::vec3 n;
        glm::vec2 tc;
    };

    class GLMeshPVP final
    {
    public:
        GLMeshPVP(const std::vector<uint32_t>& indices, const void* vertexData, uint32_t verticesSize);

        explicit GLMeshPVP(const char* fileName);

        void drawElements(GLenum mode = GL_TRIANGLES) const;
        void drawArrays(GLenum mode, GLint first, GLint count);

        ~GLMeshPVP();

        GLuint vao_;
        uint32_t numIndices_;

        std::unique_ptr<GLBuffer> bufferIndices_;
        std::unique_ptr<GLBuffer> bufferVertices_;
    };

    class CanvasGL
    {
    public:
        void line(const glm::vec3& p1, const glm::vec3& p2, const glm::vec4& c);
        void flush();

    private:
        static constexpr uint32_t kMaxLines = 1024 * 1024;

        struct VertexData
        {
            glm::vec3 position;
            glm::vec4 color;
        };

        std::vector<VertexData> lines_;
        Shader shdLinesVertex_ = Shader("lines.vert");
        Shader shdLinesFragment_ = Shader("lines.frag");
        ShaderProgram progLines_ = ShaderProgram(shdLinesVertex_, shdLinesFragment_);
        GLMeshPVP mesh_ = GLMeshPVP({}, nullptr, sizeof(VertexData) * kMaxLines);
    };

    inline void renderCameraFrustumGL(CanvasGL& canvas, const mat4& camView, const mat4& camProj, const vec4& color, int numSegments = 1)
    {
        using glm::normalize;

        const vec4 corners[] = {
            vec4(-1, -1, -1, 1), vec4(1, -1, -1, 1),
            vec4(1, 1, -1, 1), vec4(-1, 1, -1, 1),
            vec4(-1, -1, 1, 1), vec4(1, -1, 1, 1),
            vec4(1, 1, 1, 1), vec4(-1, 1, 1, 1)
        };

        vec3 pp[8];

        const mat4 invMVP = glm::inverse(camProj * camView);

        for (int i = 0; i < 8; i++)
        {
            const vec4 q = invMVP * corners[i];
            pp[i] = glm::vec3(q) / q.w;
        }

        canvas.line(pp[0], pp[4], color);
        canvas.line(pp[1], pp[5], color);
        canvas.line(pp[2], pp[6], color);
        canvas.line(pp[3], pp[7], color);
        // near
        canvas.line(pp[0], pp[1], color);
        canvas.line(pp[1], pp[2], color);
        canvas.line(pp[2], pp[3], color);
        canvas.line(pp[3], pp[0], color);
        // x
        canvas.line(pp[0], pp[2], color);
        canvas.line(pp[1], pp[3], color);
        // far
        canvas.line(pp[4], pp[5], color);
        canvas.line(pp[5], pp[6], color);
        canvas.line(pp[6], pp[7], color);
        canvas.line(pp[7], pp[4], color);
        // x
        canvas.line(pp[4], pp[6], color);
        canvas.line(pp[5], pp[7], color);

        const float dimFactor = 0.7f;

        // bottom
        {
            vec3 p1 = pp[0];
            vec3 p2 = pp[1];
            const vec3 s1 = normalize(pp[4] - pp[0]);
            const vec3 s2 = normalize(pp[5] - pp[1]);
            for (int i = 0; i != numSegments; i++, p1 += s1, p2 += s2)
                canvas.line(p1, p2, color * dimFactor);
        }
        // top
        {
            vec3 p1 = pp[2];
            vec3 p2 = pp[3];
            const vec3 s1 = normalize(pp[6] - pp[2]);
            const vec3 s2 = normalize(pp[7] - pp[3]);
            for (int i = 0; i != numSegments; i++, p1 += s1, p2 += s2)
                canvas.line(p1, p2, color * dimFactor);
        }
        // left
        {
            vec3 p1 = pp[0];
            vec3 p2 = pp[3];
            const vec3 s1 = normalize(pp[4] - pp[0]);
            const vec3 s2 = normalize(pp[7] - pp[3]);
            for (int i = 0; i != numSegments; i++, p1 += s1, p2 += s2)
                canvas.line(p1, p2, color * dimFactor);
        }
        // right
        {
            vec3 p1 = pp[1];
            vec3 p2 = pp[2];
            const vec3 s1 = normalize(pp[5] - pp[1]);
            const vec3 s2 = normalize(pp[6] - pp[2]);
            for (int i = 0; i != numSegments; i++, p1 += s1, p2 += s2)
                canvas.line(p1, p2, color * dimFactor);
        }
    }

    inline void drawBox3dGL(CanvasGL& canvas, const mat4& m, const BoundingBox& box, const vec4& c)
    {
        std::array<vec3, 8> pts = {
            vec3(box.max_.x, box.max_.y, box.max_.z),
            vec3(box.max_.x, box.max_.y, box.min_.z),
            vec3(box.max_.x, box.min_.y, box.max_.z),
            vec3(box.max_.x, box.min_.y, box.min_.z),
            vec3(box.min_.x, box.max_.y, box.max_.z),
            vec3(box.min_.x, box.max_.y, box.min_.z),
            vec3(box.min_.x, box.min_.y, box.max_.z),
            vec3(box.min_.x, box.min_.y, box.min_.z),
        };

        for (auto& p : pts)
            p = vec3(m * vec4(p, 1.f));

        canvas.line(pts[0], pts[1], c);
        canvas.line(pts[2], pts[3], c);
        canvas.line(pts[4], pts[5], c);
        canvas.line(pts[6], pts[7], c);

        canvas.line(pts[0], pts[2], c);
        canvas.line(pts[1], pts[3], c);
        canvas.line(pts[4], pts[6], c);
        canvas.line(pts[5], pts[7], c);

        canvas.line(pts[0], pts[4], c);
        canvas.line(pts[1], pts[5], c);
        canvas.line(pts[2], pts[6], c);
        canvas.line(pts[3], pts[7], c);
    }
}
