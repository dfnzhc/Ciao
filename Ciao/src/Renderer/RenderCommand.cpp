#include "RenderCommand.h"

#include "Log.h"

namespace Ciao
{
    void VAORenderCommand::Draw()
    {
        std::shared_ptr<ShaderProgram> shader = m_Shader.lock();

        if (shader) {
            glBindVertexArray(m_VAO);
            shader->UseProgram();

            if (m_primSize > 0) {
                glDrawArrays(m_primType, 0, m_primSize);
            }
            else {
                CIAO_CORE_ERROR("Primitive size less than 0!");
            }

            glBindVertexArray(0);
        }
        else {
            CIAO_CORE_ERROR("Invalid shader program!");
        }
    }
}
