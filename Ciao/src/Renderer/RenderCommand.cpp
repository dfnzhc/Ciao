#include "pch.h"

#include "RenderCommand.h"
#include "Asset/Object.h"

namespace Ciao
{
    void VAORenderCommand::Draw()
    {
        if (m_Shader) {
            glBindVertexArray(m_VAO);
            m_Shader->UseProgram();

            if (m_primSize > 0) {
                glDrawArrays(m_primType, 0, m_primSize);
            }
            else {
                CIAO_CORE_ERROR("Primitive size less than 0!");
            }

            glBindVertexArray(0);
        }
        else {
            CIAO_CORE_ERROR("Invalid shader program setting!");
        }
    }

    void ObjRenderCommand::Draw()
    {
        if (m_Obj && m_Shader) {
            m_Shader->UseProgram();
            m_Obj->Draw();
        }
        else {
            CIAO_CORE_ERROR("Invalid object or shader program setting!");
        }
    }
}
