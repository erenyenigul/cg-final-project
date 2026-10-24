#pragma once

#include <framework/shader.h>
#include <filesystem>

class Cubemap {
public:
    explicit Cubemap(const std::filesystem::path& directory);
    ~Cubemap();

    // safety first
    Cubemap(const Cubemap&) = delete;

    void bind(GLenum textureUnit) const;

private:
    GLuint m_texture = 0;
};