#pragma once

#include "CG/application.h"

#include "glm/glm.hpp"


// Camera that moves on a sphere around a target point (the mast) and always looks at it.
// The position is stored in spherical coordinates: azimuth (angle around the y axis),
// elevation (angle above the horizontal plane) and distance to the target.
// Same interface as CG::FreeCam3D: update(), get_view(), get_projection().
class OrbitCamera {
public:
    glm::vec3 target = glm::vec3(0.0f, 3.0f, 0.0f); // point the camera looks at (on the mast)
    float azimuth = glm::radians(30.0f);             // radians
    float elevation = glm::radians(20.0f);           // radians
    float distance = 18.0f;

    float fovy_deg = 50.0f;
    float near_z = 0.1f;
    float far_z = 200.0f; // large enough for the plaza (radius 30) seen from the maximum distance

    float rotate_sensitivity = 0.005f; // radians per pixel of mouse movement
    float zoom_factor = 0.9f;          // distance is multiplied by this per scroll wheel tick

    // Limits: the elevation stays below +-90 degrees because lookAt() is undefined when the view
    // direction is parallel to the up vector. Negative values are limited so the camera stays above ground.
    float min_elevation = glm::radians(-10.0f);
    float max_elevation = glm::radians(85.0f);
    float min_distance = 3.0f;
    float max_distance = 60.0f;

    // Reads the mouse input of this frame. Dragging with the left mouse button rotates the camera,
    // the scroll wheel changes the distance. read_input = false ignores the mouse (e.g. over the GUI).
    void update(const CG::Application& app, bool read_input);

    // Camera position in world coordinates, converted from the spherical coordinates.
    glm::vec3 get_position() const;

    glm::mat4 get_view() const;
    glm::mat4 get_projection() const;

private:
    float aspect = 1.0f;
};
