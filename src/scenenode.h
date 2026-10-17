#pragma once
#include "texture.h"
#include <framework/disable_all_warnings.h>
DISABLE_WARNINGS_PUSH()
#include <glm/vec3.hpp>
DISABLE_WARNINGS_POP()
#include <exception>
#include <filesystem>
#include <framework/opengl_includes.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <glm/mat4x4.hpp>
#include "mesh.h"

class SceneNode {
public:
    SceneNode(const std::filesystem::path& objPath,
              const glm::mat4& transform = glm::mat4(1.0f),
              std::vector<std::shared_ptr<SceneNode>> children = {},
              std::shared_ptr<PBRMaterial> pbrMaterial = nullptr);

    // Group node without its own mesh.
    SceneNode(std::vector<std::shared_ptr<SceneNode>> children,
              const glm::mat4& transform = glm::mat4(1.0f));

    void draw(const Shader& drawingShader, const glm::mat4& viewProjection, const glm::mat4& parentWorld = glm::mat4(1.0f), DisplayMode displayMode = DisplayMode::Normal);
private:
    std::vector<std::shared_ptr<SceneNode>> m_children;
    std::vector<GPUMesh> m_meshes;
    glm::mat4 transform = glm::mat4(1.0f);
};
