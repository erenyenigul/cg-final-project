#include "material.h"
#include "texture.h"
#include <framework/disable_all_warnings.h>
DISABLE_WARNINGS_PUSH()
#include <fmt/format.h>
DISABLE_WARNINGS_POP()
#include <framework/image.h>

#include <iostream>

Materialx::Materialx(const std::filesystem::path& filePath)
    : m_color(filePath / "color.jpg")
    , m_displacement(filePath / "displacement.jpg")
    , m_normaldx(filePath / "normaldx.jpg")
    , m_normalgl(filePath / "normalgl.jpg")
    , m_roughness(filePath / "roughness.jpg")
{}

void Materialx::bind()
{
    m_color.bind(GL_TEXTURE0);
    m_displacement.bind(GL_TEXTURE1);
    m_normaldx.bind(GL_TEXTURE2);
    m_normalgl.bind(GL_TEXTURE3);
    m_roughness.bind(GL_TEXTURE4);
}
