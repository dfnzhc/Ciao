#include "pch.h"
#include "Grid.h"

#include "Shader.h"

namespace Ciao
{
    void Ciao::Grid::Create()
    {
        glCreateVertexArrays(1, &vao);
    }

    void Ciao::Grid::Draw(std::shared_ptr<ShaderProgram> shader)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        shader->UseProgram();
        glBindVertexArray(vao);
        glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, 6, 1, 0);

        glDisable(GL_BLEND);
    }

    void Ciao::Grid::Release()
    {
        glDeleteVertexArrays(1, &vao);
    }
}
