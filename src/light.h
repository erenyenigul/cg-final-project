#pragma once
#include <glm/glm.hpp>
#include <framework/shader.h>
#include <glm/gtc/type_ptr.hpp>

class DirectionalLight {

    public:
        DirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity);
        
        glm::vec3 direction() const { return m_direction; }
        glm::vec3 color() const { return m_color; }
        float intensity() const { return m_intensity; }

        void bind(const Shader& shader) const;

    private:
        glm::vec3 m_direction;
        glm::vec3 m_color;
        float m_intensity;
};