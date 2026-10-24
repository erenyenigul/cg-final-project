#pragma once
#include "texture.h"
#include <framework/disable_all_warnings.h>
DISABLE_WARNINGS_PUSH()
#include <glm/vec3.hpp>
DISABLE_WARNINGS_POP()
#include <exception>
#include <filesystem>
#include <framework/opengl_includes.h>
#include <framework/shader.h>

class PBRMaterial {
public:
    PBRMaterial(const std::filesystem::path& filePath, float metallic = 0.0f);
    void bind(const Shader& shader);
private:
    Texture m_color;
    Texture m_displacement;
    Texture m_roughness;
    float m_metallic { 0.0f };
};
