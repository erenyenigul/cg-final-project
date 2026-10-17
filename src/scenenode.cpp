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

SceneNode::SceneNode(const std::filesystem::path& objPath,
                     const glm::mat4& transform,
                     std::vector<std::shared_ptr<SceneNode>> children,
                     std::shared_ptr<PBRMaterial> pbrMaterial)
    : m_children(std::move(children)),
    m_meshes(GPUMesh::loadMeshGPU(objPath, false, std::move(pbrMaterial))),
    transform(transform)
{}

SceneNode::SceneNode(std::vector<std::shared_ptr<SceneNode>> children,
                     const glm::mat4& transform)
    : m_children(std::move(children)),
    transform(transform)
{}

void SceneNode::draw(const Shader& drawingShader, const glm::mat4& viewProjection, const glm::mat4& parentWorld, DisplayMode displayMode)
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

        mesh.draw(drawingShader, displayMode);
    }

    for (const std::shared_ptr<SceneNode>& child : m_children) {
        child->draw(drawingShader, viewProjection, world, displayMode);
    }
}