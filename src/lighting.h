#pragma once

#include "GL/glew.h"
#include "glm/glm.hpp"


// All light sources of the scene in world coordinates.
// The positions and directions are updated every frame from the scene objects the lights are attached to
// (lamp on the roof, searchlight on a gondola), the remaining values can be changed in the GUI.
struct SceneLighting {
    // Low ambient term because the scene takes place at night
    glm::vec3 ambient = glm::vec3(0.04f, 0.04f, 0.06f);

    // Point light emitted by the lamp on top of the roof
	// TODO: Move to correct position on the roof and allow changing the color in the GUI
    glm::vec3 point_position = glm::vec3(8.0f, 10.0f, 8.0f);
    glm::vec3 point_color = glm::vec3(1.0f, 0.88f, 0.7f);  // warm light bulb
    float point_intensity = 2.2f;
    float point_attenuation = 0.01f;                        // quadratic falloff: 1 / (1 + k * d^2)

    // Uploads all light parameters to the given (currently bound) shader program.
    void set_uniforms(GLuint program) const
    {
        glUniform3fv(glGetUniformLocation(program, "ambient_light"), 1, &ambient[0]);

        glm::vec3 point_radiance = point_color * point_intensity;
        glUniform3fv(glGetUniformLocation(program, "point_light.position"), 1, &point_position[0]);
        glUniform3fv(glGetUniformLocation(program, "point_light.color"), 1, &point_radiance[0]);
        glUniform1f(glGetUniformLocation(program, "point_light.attenuation"), point_attenuation);
    }
};
