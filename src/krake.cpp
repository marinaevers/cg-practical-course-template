#include "krake.h"

#include "CG/math.h"
#include "CG/texture.h"

#include "glm/gtx/transform.hpp"

#include <array>
#include <memory>


namespace
{
    // Dimensions of the ride (all lengths in scene units, radii as in the task description).
    constexpr float plaza_radius = 30.0f;

    constexpr float podest_bottom_radius = 7.0f;
    constexpr float podest_top_radius = 6.5f;
    constexpr float podest_height = 0.4f;

    constexpr float mast_radius = 0.4f;
    constexpr float mast_height = 5.0f;

    constexpr float roof_radius = 2.5f;
    constexpr float roof_height = 1.5f;
    constexpr float garland_inner_radius = 2.3f;
    constexpr float garland_outer_radius = 2.6f;
    constexpr float lamp_radius = 0.2f;

    constexpr float rotor_radius = 0.6f;
    constexpr float rotor_height = 4.0f;       // on the mast, measured from its base

    constexpr int arm_count = 5;
    constexpr float arm_length = 4.5f;
    constexpr float arm_base_radius = 0.25f;
    constexpr float arm_tip_radius = 0.12f;
    constexpr float arm_tilt_deg = 25.0f;      // downwards from the horizontal

    constexpr float holder_outer_radius = 1.1f;

    constexpr int gondolas_per_star = 3;
    constexpr float gondola_radius = 0.45f;
    constexpr float gondola_distance = 1.0f;   // from the center of the star
    constexpr float gondola_hang = 0.65f;      // gondola centers hang below the star plane
    constexpr float gondola_roof_radius = 0.5f;
    constexpr float gondola_roof_height = 0.35f;
    constexpr float gondola_roof_offset = 0.3f; // height of the roof base above the gondola center

    constexpr float searchlight_base_radius = 0.08f;
    constexpr float searchlight_front_radius = 0.15f;
    constexpr float searchlight_length = 0.3f;
    constexpr float searchlight_mount = 0.38f;          // distance of the mount from the gondola center
    constexpr float searchlight_pitch_deg = 135.0f;     // 90 deg = horizontal, 135 deg = 45 deg downwards

    constexpr float rotor_speed = 1.0f;
    constexpr float star_speed = -3.0f;
    constexpr float gondola_speed = 5.0f;

    const glm::vec3 x_axis(1.0f, 0.0f, 0.0f);
    const glm::vec3 y_axis(0.0f, 1.0f, 0.0f);
    const glm::vec3 z_axis(0.0f, 0.0f, 1.0f);

    // Creates an object, adds it as child of parent and returns a typed pointer to it.
    // Note on the syntax: the "..." allows to add a variable number of parameters
    // See https://en.wikipedia.org/wiki/Variadic_template for more details
    template<class T, class... Args>
    T* attach(CG::SceneObject& parent, Args&&... args)
    {
        return static_cast<T*>(parent.add_child(std::make_unique<T>(std::forward<Args>(args)...)));
    }

    Material make_material(glm::vec3 diffuse, glm::vec3 specular = glm::vec3(0.2f), float shininess = 16.0f)
    {
        Material m;
        m.diffuse = diffuse;
        m.specular = specular;
        m.shininess = shininess;
        return m;
    }
}


KrakeParts build_krake(CG::SceneHierarchy& scene, RenderContext& context)
{
    KrakeParts parts;

    // Resolution groups in the order they appear in the GUI, with their initial number of slices
    enum Group { Podest, Mast, Roof, Garland, Lamp, Rotor, Arms, Holders, Gondolas, GondolaRoofs, Searchlight, GroupCount };
    parts.resolution_groups = {
        { "Podest", {}, 64 }, { "Mast", {}, 24 }, { "Roof", {}, 64 },
        { "Garland", {}, 64 }, { "Lamp", {}, 12 }, { "Rotor", {}, 20 }, { "Arms", {}, 16 },
        { "Gondola holders", {}, 32 }, { "Gondolas", {}, 20 }, { "Gondola roofs", {}, 16 }, { "Searchlight", {}, 12 },
    };
    auto group = [&](Group g, Primitive* p) {
        p->set_resolution(parts.resolution_groups[g].slices);
        parts.resolution_groups[g].members.push_back(p);
    };

    // TODO: Remove the cube for the final scene and replace it by the primitives you want to use.
	auto cube = scene.add_child(std::make_unique<Cube>("Cube", context, 3.0f));
	cube->placement = glm::translate(glm::vec3(0.0f, 5.0f, 0.0f));
	cube->revolutions_per_period = 0.5f;
    cube->material = make_material(glm::vec3(0.8f, 0.8f, 0.85f), glm::vec3(0.2f), 16.0f);
    // This is just example usage because the cube cannot change resolution.
	group(Podest, cube);

    return parts;
}
