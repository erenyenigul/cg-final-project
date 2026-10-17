#include "material.h"
#include "texture.h"
#include <framework/disable_all_warnings.h>
DISABLE_WARNINGS_PUSH()
#include <fmt/format.h>
DISABLE_WARNINGS_POP()
#include <framework/image.h>

#include <iostream>

PBRMaterial::PBRMaterial(const std::filesystem::path& filePath)
    : m_color(filePath / "color.jpg")
    , m_displacement(filePath / "displacement.png")
    , m_roughness(filePath / "roughness.jpg")
{}

void PBRMaterial::bind()
{
    m_color.bind(GL_TEXTURE0);
    m_displacement.bind(GL_TEXTURE1);
    m_roughness.bind(GL_TEXTURE2);
}
