#pragma once

#include "primitives.h"
#include "render_context.h"

#include "CG/scene.h"

#include <string>
#include <vector>


// All primitives of one kind (e.g. all 15 gondolas) share a resolution that can be changed in the GUI.
struct ResolutionGroup {
    std::string label;
    std::vector<Primitive*> members;
    int slices = 16;

    void apply() {
        for (Primitive* p : members)
            p->set_resolution(slices);
    }
    int triangle_count() const {
        int sum = 0;
        for (const Primitive* p : members)
            sum += p->triangle_count();
        return sum;
    }
};


// Pointers to the parts of the ride the application needs after construction.
// The objects themselves are owned by the scene hierarchy.
// Hint: Consider adding information about the lights
struct KrakeParts {
    std::vector<const CG::SceneObject*> gondolas;    // for the global paths
    std::vector<ResolutionGroup> resolution_groups;  
};

// Builds the complete scene hierarchy of the "Krake" ride and adds it to the scene.
KrakeParts build_krake(CG::SceneHierarchy& scene, RenderContext& context);