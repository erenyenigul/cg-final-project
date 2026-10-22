#include "mesh.h"
#include "texture.h"
#include "enum.h"
#include <framework/disable_all_warnings.h>
DISABLE_WARNINGS_PUSH()
#include <fmt/format.h>
DISABLE_WARNINGS_POP()
#include <iostream>
#include <vector>
#include <glm/geometric.hpp>
#include <cmath>

GPUMaterial::GPUMaterial(const Material& material) :
    kd(material.kd),
    ks(material.ks),
    shininess(material.shininess),
    transparency(material.transparency)
{}

GPUMesh::GPUMesh(const Mesh& cpuMesh)
    : GPUMesh(cpuMesh, nullptr)
{}

GPUMesh::GPUMesh(const Mesh& cpuMesh, std::shared_ptr<PBRMaterial> pbrMaterial)
    : m_pbrMaterial(pbrMaterial)
{
    // Create uniform buffer to store mesh material (https://learnopengl.com/Advanced-OpenGL/Advanced-GLSL)
    GPUMaterial gpuMaterial(cpuMesh.material);
    glGenBuffers(1, &m_uboMaterial);
    glBindBuffer(GL_UNIFORM_BUFFER, m_uboMaterial);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(GPUMaterial), &gpuMaterial, GL_STATIC_READ);

    // Figure out if this mesh has texture coordinates
    m_hasTextureCoords = static_cast<bool>(cpuMesh.material.kdTexture);
    
    if (m_hasTextureCoords) {
        m_texture = std::make_shared<Texture>(*cpuMesh.material.kdTexture);
    }

    
    std::vector<glm::vec3> tangentSum(cpuMesh.vertices.size(), glm::vec3(0.0f));
    std::vector<glm::vec3> bitangentSum(cpuMesh.vertices.size(), glm::vec3(0.0f));

    // maybe better way to do
    for (const auto& triangle : cpuMesh.triangles) {

        const auto i0 = triangle[0];
        const auto i1 = triangle[1];
        const auto i2 = triangle[2];

        const Vertex& v0 = cpuMesh.vertices[i0];
        const Vertex& v1 = cpuMesh.vertices[i1];
        const Vertex& v2 = cpuMesh.vertices[i2];
        
        // edges of triangle in model space
        glm::vec3 e1 = v1.position - v0.position;
        glm::vec3 e2 = v2.position - v0.position;

        // same edges in uv space
        glm::vec2 duv1 = v1.texCoord - v0.texCoord;
        glm::vec2 duv2 = v2.texCoord - v0.texCoord;
        
        // calculate determinate  of m
        float det = duv1.x * duv2.y - duv1.y * duv2.x;

        // small det case
        if (std::abs(det) < 1e-8f)
            continue;

        float f = 1.0f / det;
        
        // solve t and b
        glm::vec3 T = f * (duv2.y * e1 - duv1.y * e2);
        glm::vec3 B = f * (-duv2.x * e1 + duv1.x * e2);

        tangentSum[i0] += T;
        tangentSum[i1] += T;
        tangentSum[i2] += T;

        bitangentSum[i0] += B;
        bitangentSum[i1] += B;
        bitangentSum[i2] += B;
    }

    std::vector<glm::vec4> tangents(cpuMesh.vertices.size());

    for (size_t i = 0; i < cpuMesh.vertices.size(); ++i) {
        glm::vec3 N = glm::normalize(cpuMesh.vertices[i].normal);
        glm::vec3 T = tangentSum[i];

        // Gram-Schmidt thank you sir
        T = glm::normalize(T - N * glm::dot(N, T));

        // Determine bitangent orientation
        float sign = glm::dot(glm::cross(N, T), bitangentSum[i]) < 0.0f
            ? -1.0f
            : 1.0f;

        tangents[i] = glm::vec4(T, sign);
    }

    // Create VAO and bind it so subsequent creations of VBO and IBO are bound to this VAO
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // Create vertex buffer object (VBO)
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(cpuMesh.vertices.size() * sizeof(decltype(cpuMesh.vertices)::value_type)), cpuMesh.vertices.data(), GL_STATIC_DRAW);

    // Create index buffer object (IBO)
    glGenBuffers(1, &m_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(cpuMesh.triangles.size() * sizeof(decltype(cpuMesh.triangles)::value_type)), cpuMesh.triangles.data(), GL_STATIC_DRAW);

    // Tell OpenGL that we will be using vertex attributes 0, 1 and 2.
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    // We tell OpenGL what each vertex looks like and how they are mapped to the shader (location = ...).
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

    glGenBuffers(1, &m_tangentVbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_tangentVbo);

    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(tangents.size() * sizeof(glm::vec4)),
        tangents.data(),
        GL_STATIC_DRAW
    );

    glEnableVertexAttribArray(3);

    glVertexAttribPointer(
        3,
        4,
        GL_FLOAT,
        GL_FALSE,
        sizeof(glm::vec4),
        nullptr
    );

    glVertexAttribDivisor(3, 0);

    // Reuse all attributes for each instance
    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 0);
    glVertexAttribDivisor(2, 0);

    // Each triangle has 3 vertices.
    m_numIndices = static_cast<GLsizei>(3 * cpuMesh.triangles.size());
}

GPUMesh::GPUMesh(GPUMesh&& other)
{
    moveInto(std::move(other));
}

GPUMesh::~GPUMesh()
{
    freeGpuMemory();
}

GPUMesh& GPUMesh::operator=(GPUMesh&& other)
{
    moveInto(std::move(other));
    return *this;
}

std::vector<GPUMesh> GPUMesh::loadMeshGPU(std::filesystem::path filePath, bool normalize,
                                          std::shared_ptr<PBRMaterial> pbrMaterial) {
    if (!std::filesystem::exists(filePath))
        throw MeshLoadingException(fmt::format("File {} does not exist", filePath.string().c_str()));

    // Generate GPU-side meshes for all sub-meshes
    std::vector<Mesh> subMeshes = loadMesh(filePath, { .normalizeVertexPositions = normalize });
    std::vector<GPUMesh> gpuMeshes;
    for (const Mesh& mesh : subMeshes) { gpuMeshes.emplace_back(mesh, pbrMaterial); }
    
    return gpuMeshes;
}

bool GPUMesh::hasTextureCoords() const
{
    return m_hasTextureCoords;
}

void GPUMesh::draw(const Shader& drawingShader, DisplayMode displayMode)
{
    // Bind material data uniform (we assume that the uniform buffer objects is always called 'Material')
    // Yes, we could define the binding inside the shader itself, but that would break on OpenGL versions below 4.2
    drawingShader.bindUniformBlock("Material", 0, m_uboMaterial);
    
    if (displayMode == DisplayMode::PBR && !m_pbrMaterial && m_texture)
        displayMode = DisplayMode::Texture;

    if (displayMode == DisplayMode::PBR && m_pbrMaterial) {
    m_pbrMaterial->bind();

        glUniform1i(
            drawingShader.getUniformLocation("normalMap"),
            3
        );
    }

        

    else if (displayMode == DisplayMode::Texture && m_texture)
        m_texture->bind(GL_TEXTURE0);

    glUniform1i(drawingShader.getUniformLocation("displayMode"), static_cast<int>(displayMode));
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, nullptr);
}

void GPUMesh::moveInto(GPUMesh&& other)
{
    freeGpuMemory();
    m_numIndices = other.m_numIndices;
    m_hasTextureCoords = other.m_hasTextureCoords;
    m_ibo = other.m_ibo;
    m_vbo = other.m_vbo;
    m_vao = other.m_vao;
    m_tangentVbo = other.m_tangentVbo;
    m_uboMaterial = other.m_uboMaterial;
    m_texture = std::move(other.m_texture);
    m_pbrMaterial = std::move(other.m_pbrMaterial);

    other.m_numIndices = 0;
    other.m_hasTextureCoords = other.m_hasTextureCoords;
    other.m_ibo = INVALID;
    other.m_vbo = INVALID;
    other.m_vao = INVALID;
    other.m_uboMaterial = INVALID;
    other.m_tangentVbo = INVALID;
}

void GPUMesh::freeGpuMemory()
{
    if (m_vao != INVALID)
        glDeleteVertexArrays(1, &m_vao);
    if (m_vbo != INVALID)
        glDeleteBuffers(1, &m_vbo);
    if (m_ibo != INVALID)
        glDeleteBuffers(1, &m_ibo);
    if (m_uboMaterial != INVALID)
        glDeleteBuffers(1, &m_uboMaterial);
    if (m_tangentVbo != INVALID)
        glDeleteBuffers(1, &m_tangentVbo);
}
