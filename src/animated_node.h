#pragma once

#include "animation.h"

#include "CG/scene.h"
#include "CG/math.h"

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

#include <string>


// Scene graph node with a fixed placement relative to its parent and an optional rotation about its
// own (local) y axis that is driven by the shared animation clock.
//
// Why not use SceneObject::tf? Transform stores Euler angles with a fixed order (Rz * Ry * Rx).
// The ride needs other combinations, e.g. the gondola star first has to undo the tilt of its arm and
// then spin about the (now vertical) axis: Rx(-tilt) * Ry(spin). Building the local matrix explicitly
// makes the order of the operations visible.
class AnimatedNode : public CG::SceneObject {
public:
    glm::mat4 placement = glm::mat4(1.0f); // Fixed transform relative to the parent node
    float revolutions_per_period = 0.0f;   // Spin about the local y axis (negative = clockwise seen from above)

    AnimatedNode(const std::string& name, const AnimationClock& clock)
        : CG::SceneObject(name), clock(clock) {}

    // Current spin angle in radians, derived from the global animation phase.
    float spin_angle() const {
        return float(TAU * revolutions_per_period * clock.phase());
    }

    // The animation is realized purely by transformation matrices.
    // Matrices are applied right to left to the vertices: first the object spins about its own
    // y axis (while it is still at the origin of its local frame), then it is moved to its place.
    // Swapping the factors would make the object orbit around the parent's origin instead.
    glm::mat4 local_matrix() const override {
        return placement * glm::rotate(spin_angle(), glm::vec3(0.0f, 1.0f, 0.0f));
    }

protected:
    const AnimationClock& clock;
};
