#include "orbit_camera.h"

#include "CG/math.h"

#include "glm/gtc/matrix_transform.hpp"

#include <cmath>


void OrbitCamera::update(const CG::Application& app, bool read_input)
{
    // The aspect ratio is recomputed every frame, so resizing the window does not distort the image.
    if (app.viewport.x > 0 && app.viewport.y > 0)
        aspect = float(app.viewport.x) / float(app.viewport.y);

    if (!read_input)
        return;

    // One mouse axis per angle while the left button is held.
    // Mouse movement is already a per-frame quantity and must not be scaled with dt.
    if (app.input.mouse_down[GLFW_MOUSE_BUTTON_LEFT]) {
		// TODO: Update azimuth and elevation based on mouse_delta and rotate_sensitivity. Clamp elevation to [min_elevation, max_elevation].
    }

    // Scroll wheel: multiplicative zoom feels uniform at small and large distances.
    if (app.input.mouse_scroll != 0.0f) {
        distance *= std::pow(zoom_factor, app.input.mouse_scroll);
        distance = glm::clamp(distance, min_distance, max_distance);
    }
}

glm::vec3 OrbitCamera::get_position() const
{
    // Spherical -> Cartesian: the horizontal radius is distance * cos(elevation).
    float horizontal = distance * std::cos(elevation);
    return target + glm::vec3(horizontal * std::sin(azimuth),
                              distance * std::sin(elevation),
                              horizontal * std::cos(azimuth));
}

glm::mat4 OrbitCamera::get_view() const
{
    return glm::lookAt(get_position(), target, glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 OrbitCamera::get_projection() const
{
    return glm::perspective(glm::radians(fovy_deg), aspect, near_z, far_z);
}
