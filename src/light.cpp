#include "light.h"
#include <glm/glm.hpp>
#include <framework/shader.h>
#include <glm/gtc/type_ptr.hpp>

DirectionalLight::DirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity)
    : m_direction(direction), m_color(color), m_intensity(intensity)
{}

void DirectionalLight::bind(const Shader& shader) const
{
    glUniform3fv(shader.getUniformLocation("lightDirection"), 1, glm::value_ptr(m_direction));
    glUniform3fv(shader.getUniformLocation("lightColor"), 1, glm::value_ptr(m_color));
    glUniform1f(shader.getUniformLocation("lightIntensity"), m_intensity);
}