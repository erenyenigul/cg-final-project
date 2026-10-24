#include "material.h"
#include "texture.h"
#include <framework/disable_all_warnings.h>
DISABLE_WARNINGS_PUSH()
#include <fmt/format.h>
DISABLE_WARNINGS_POP()
#include <framework/image.h>

#include <iostream>

PBRMaterial::PBRMaterial(const std::filesystem::path& filePath, float metallic)
    : m_color(filePath / "color.jpg", true)
    , m_displacement(filePath / "displacement.png")
    , m_roughness(filePath / "roughness.jpg")
    , m_metallic(metallic)
{}

void PBRMaterial::bind(const Shader& shader)
{
    m_color.bind(GL_TEXTURE0);
    m_displacement.bind(GL_TEXTURE1);
    m_roughness.bind(GL_TEXTURE2);

    glUniform1f(shader.getUniformLocation("metallic"), m_metallic);
}
