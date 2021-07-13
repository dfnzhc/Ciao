#include "pch.h"
#include "VertexBufferObject.h"

VertexBufferObject::VertexBufferObject()
{
    m_isDataUploaded = false;
}

VertexBufferObject::~VertexBufferObject()
{
    
}

void VertexBufferObject::Create()
{
    glGenBuffers(1, &m_VBO);
}

void VertexBufferObject::Bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
}

void VertexBufferObject::Release()
{
    glDeleteBuffers(1, &m_VBO);
    m_isDataUploaded = false;
    m_Data.clear();
}

/// 往 VBO 缓冲区中添加数据
void VertexBufferObject::AddData(void* ptrData, GLuint uiDataSize)
{
    m_Data.insert(m_Data.end(), (GLbyte*)ptrData, (GLbyte*)ptrData + uiDataSize);
}

/// 将缓冲区中的数据上传到 GPU，之后缓冲区中的数据能够被清除
void VertexBufferObject::UploadDataToGPU(int iUsageHint)
{
    glBufferData(GL_ARRAY_BUFFER, m_Data.size(), &m_Data[0], iUsageHint);
    m_isDataUploaded = true;
    m_Data.clear();
}

VertexBufferObjectIndexed::VertexBufferObjectIndexed()
{
    m_isDataUploaded = false;
}

VertexBufferObjectIndexed::~VertexBufferObjectIndexed()
{
}

void VertexBufferObjectIndexed::Create()
{
    glGenBuffers(1, &m_vboVertices);
    glGenBuffers(1, &m_vboIndices);
}

void VertexBufferObjectIndexed::Bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vboVertices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIndices);
}

void VertexBufferObjectIndexed::Release()
{
    glDeleteBuffers(1, &m_vboVertices);
    glDeleteBuffers(1, &m_vboIndices);
    m_isDataUploaded = false;
    m_vertexData.clear();
    m_indexData.clear();
}

void VertexBufferObjectIndexed::AddVertexData(void* pVertexData, GLuint vertexDataSize)
{
    m_vertexData.insert(m_vertexData.end(), (GLbyte*)pVertexData, (GLbyte*)pVertexData+vertexDataSize);
}

void VertexBufferObjectIndexed::AddIndexData(void* pIndexData, GLuint indexDataSize)
{
    m_indexData.insert(m_indexData.end(), (GLbyte*)pIndexData, (GLbyte*)pIndexData+indexDataSize);
}

void VertexBufferObjectIndexed::UploadDataToGPU(int iUsageHint)
{
    glBufferData(GL_ARRAY_BUFFER, m_vertexData.size(), &m_vertexData[0], iUsageHint);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexData.size(), &m_indexData[0], iUsageHint);
    m_isDataUploaded = true;
    m_vertexData.clear();
    m_indexData.clear();
}



