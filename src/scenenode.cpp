#include "scenenode.h"
#include "texture.h"
#include "mesh.h"
#include "material.h"
#include <framework/disable_all_warnings.h>
DISABLE_WARNINGS_PUSH()
#include <fmt/format.h>
DISABLE_WARNINGS_POP()
#include <framework/image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <iostream>

SceneNode::SceneNode(const std::filesystem::path& objPath)
    : m_meshes(GPUMesh::loadMeshGPU(objPath))
{}

SceneNode::SceneNode(const std::filesystem::path& objPath, const glm::mat4& transform)
    : m_meshes(GPUMesh::loadMeshGPU(objPath)),
    transform(transform)
{}

SceneNode::SceneNode(const std::vector<std::shared_ptr<SceneNode>> children)
    : m_children(children)
{}

SceneNode::SceneNode(const std::filesystem::path& objPath, 
                     const std::vector<std::shared_ptr<SceneNode>> children)
    : m_children(children),
    m_meshes(GPUMesh::loadMeshGPU(objPath))
{}

SceneNode::SceneNode(const std::filesystem::path& objPath, 
                     const glm::mat4& transform,
                     const std::vector<std::shared_ptr<SceneNode>> children)
    : m_children(children),
    m_meshes(GPUMesh::loadMeshGPU(objPath)),
    transform(transform)
{}

void SceneNode::draw(const Shader& drawingShader, const glm::mat4& viewProjection, const glm::mat4& parentWorld)
{
    const glm::mat4 world = parentWorld * transform;
    const glm::mat4 mvpMatrix = viewProjection * world;
    // Normals should be transformed differently than positions (ignoring translations + dealing with scaling):
    // https://paroj.github.io/gltut/Illumination/Tut09%20Normal%20Transformation.html
    const glm::mat3 normalModelMatrix = glm::inverseTranspose(glm::mat3(world));

    drawingShader.bind();
    for (GPUMesh& mesh : m_meshes) {
        glUniformMatrix4fv(drawingShader.getUniformLocation("mvpMatrix"), 1, GL_FALSE, glm::value_ptr(mvpMatrix));
        glUniformMatrix4fv(drawingShader.getUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(world));
        glUniformMatrix3fv(drawingShader.getUniformLocation("normalModelMatrix"), 1, GL_FALSE, glm::value_ptr(normalModelMatrix));
        if (mesh.hasTextureCoords()) {
            glUniform1i(drawingShader.getUniformLocation("colorMap"), 0);
            glUniform1i(drawingShader.getUniformLocation("hasTexCoords"), GL_TRUE);
            glUniform1i(drawingShader.getUniformLocation("useMaterial"), GL_FALSE);
        } else {
            glUniform1i(drawingShader.getUniformLocation("hasTexCoords"), GL_FALSE);
            glUniform1i(drawingShader.getUniformLocation("useMaterial"), true);
        }
        mesh.draw(drawingShader);
    }

    for (const std::shared_ptr<SceneNode>& child : m_children) {
        child->draw(drawingShader, viewProjection, world);
    }
}