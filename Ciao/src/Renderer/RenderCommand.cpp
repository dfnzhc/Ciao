#include "pch.h"

#include "RenderCommand.h"

#include "Application.h"
#include "Framebuffer.h"
#include "RenderManager.h"
#include "Asset/Object.h"
#include "Asset/ModelEntity.h"

namespace Ciao
{
    void DrawVao::Execute()
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

    void DrawObject::Execute()
    {
        if (m_Obj && m_Shader) {
            m_Obj->Draw(m_Shader);
        }
        else {
            CIAO_CORE_ERROR("Invalid object or shader program setting!");
        }
    }

    void DrawBuildInObj::Execute()
    {
        if (m_Shader && m_VertCnt > 0) {
            glDepthFunc(GL_LEQUAL);
            m_Shader->UseProgram();
            glDrawArrays(GL_TRIANGLES, 0, m_VertCnt);
            glDepthFunc(GL_LESS);
        }
        else {
            CIAO_CORE_ERROR("Invalid vertices count or shader program setting!");
        }
    }

    void DrawModelEntity::Execute()
    {
        if (m_Model) {
            m_Model->SetDrawShaderIdx(m_DrawIdx);
            m_Model->Draw();
        }
        else {
            CIAO_CORE_ERROR("Invalid model setting!");
        }
    }

    void PushFramebuffer::Execute()
    {
        if (m_Framebuffer) {
            Application::GetInst().GetRenderManager()->PushFramebuffer(m_Framebuffer);
        }
        else {
            CIAO_CORE_ERROR("Invalid framebuffer Command!");
        }
    }

    void PopFramebuffer::Execute()
    {
        Application::GetInst().GetRenderManager()->PopFramebuffer();
    }

    void FlushCommandQueue::Execute()
    {
        Application::GetInst().GetRenderManager()->Flush();
    }
}
