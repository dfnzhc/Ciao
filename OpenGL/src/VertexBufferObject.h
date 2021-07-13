#pragma once

class VertexBufferObject
{
public:
    VertexBufferObject();
    ~VertexBufferObject();

    void Create();						                    // Creates a VBO
    void Bind();	                                        // Binds the VBO
    void Release();									        // Releases the VBO

    void AddData(void* ptrData, GLuint uiDataSize);	        // Add data to the VBO
    void UploadDataToGPU(int iUsageHint);			        // Upload the VBO to the GPU

private:
    GLuint m_VBO;
    std::vector<GLbyte> m_Data;

    bool m_isDataUploaded;
};

class VertexBufferObjectIndexed
{
public:
    VertexBufferObjectIndexed();
    ~VertexBufferObjectIndexed();

    void Create();									// Creates a VBO
    void Bind();									// Binds the VBO
    void Release();									// Releases the VBO

    void AddVertexData(void* pVertexData, GLuint vertexDataSize);	// Adds vertex data
    void AddIndexData(void* pIndexData, GLuint indexDataSize);	    // Adds index data
    void UploadDataToGPU(int iUsageHint);			// Upload the VBO to the GPU


private:
    GLuint m_vboVertices;		                // VBO id for vertices
    GLuint m_vboIndices;		                // VBO id for indices

    std::vector<GLbyte> m_vertexData;	        // Vertex data to be uploaded
    std::vector<GLbyte> m_indexData;	        // Index data to be uploaded

    bool m_isDataUploaded;		                // Flag indicating if data is uploaded to the GPU
};