#include "cubemap.h"
#include "framework/opengl_includes.h"
#include "texture.h"
#include <cstddef>
#include <framework/image.h>

#include <array>
#include <string>
#include <stdexcept>


Cubemap::Cubemap(const std::filesystem::path& directory) 
{
    const std::array<std::string, 6> faces = {
        "posx.jpg",
        "negx.jpg",
        "posy.jpg",
        "negy.jpg",
        "posz.jpg",
        "negz.jpg"
    };

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);

    for (size_t i = 0; i < faces.size(); ++i) {
        auto path = directory / faces[i];

        int width, height, channels;

        auto* data = stbi_load(
            path.string().c_str(),
            &width,
            &height,
            &channels,
            3
        );

        if (!data) {
            glDeleteTextures(1, &m_texture);
            m_texture = 0;
            throw std::runtime_error(
                "Failed to load cubemap face: " + path.string()
            );
        }

        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<GLenum>(i),
            0,
            GL_RGB,
            width,
            height,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            data
        );

        stbi_image_free(data);
    }

    // filter
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // wrap
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

}


void Cubemap::bind(GLenum textureUnit) const
{
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
}


Cubemap::~Cubemap()
{
    glDeleteTextures(1, &m_texture);
}
