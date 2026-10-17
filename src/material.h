#pragma once
#include "texture.h"
#include <framework/disable_all_warnings.h>
DISABLE_WARNINGS_PUSH()
#include <glm/vec3.hpp>
DISABLE_WARNINGS_POP()
#include <exception>
#include <filesystem>
#include <framework/opengl_includes.h>

class PBRMaterial {
public:
    PBRMaterial(const std::filesystem::path& filePath);
    void bind();
private:
    Texture m_color;
    Texture m_displacement;
    Texture m_roughness;
};
