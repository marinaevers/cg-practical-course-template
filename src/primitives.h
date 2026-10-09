#pragma once

#include "animated_node.h"
#include "render_context.h"

#include "CG/mesh.h"
#include "CG/texture.h"

#include "glm/glm.hpp"

#include <algorithm>
#include <memory>
#include <string>


// ---------------------------------------------------------------------------------------------------
// Geometry generation for the three primitive types.
// The functions only produce vertex data (no OpenGL calls), so they can be tested independently.
// All primitives are centered on the y axis. Triangles are ordered counter-clockwise when seen from
// outside, i.e. from the side the normals point to (important for gl_FrontFacing in the shader).
// ---------------------------------------------------------------------------------------------------
namespace primitives
{
    constexpr int min_slices = 3; // a circle needs at least three segments
}


// Surface properties of a primitive.
struct Material {
    glm::vec3 diffuse = glm::vec3(0.8f);   // base color (multiplied with the texture, if any)
    glm::vec3 specular = glm::vec3(0.3f);
    float shininess = 32.0f;               // Blinn-Phong exponent
    bool emissive = false;                 // draw unlit in the diffuse color (for the lamp)

    std::shared_ptr<CG::Texture> texture;  // optional base texture (tent canvas)
    std::shared_ptr<CG::Texture> overlay;  // optional second texture with alpha (light bulbs)
    glm::vec2 overlay_scale = glm::vec2(1.0f); // repetitions of the overlay texture in u and v
    float overlay_revolutions = 0.0f;          // how often the overlay moves once around (u) per animation period
};


// Common base class of the primitives: owns the mesh, the material and the resolution,
// and draws itself with the shared Blinn-Phong shader. Subclasses only provide the geometry.
class Primitive : public AnimatedNode {
public:
    Material material;

    Primitive(const std::string& name, RenderContext& context, int resolution)
        : AnimatedNode(name, context.clock), context(context), slices(resolution) {}

    // Changes the number of segments around the circumference and rebuilds the mesh.
    void set_resolution(int new_slices);
    int resolution() const { return slices; }
    int triangle_count() const { return triangles; }

    void init() override;
    void render(const CG::CameraData& cam) override;
    void inspector_gui() override;

protected:
    // Creates the vertex data for the given resolution (number of slices).
    virtual CG::MeshBuilder generate(int slices) const = 0;

private:
    void rebuild_mesh();

    RenderContext& context;
    std::shared_ptr<CG::Mesh> mesh;
    int slices;
    int triangles = 0;
};

class Cube : public Primitive {
public:
	float size;

	Cube(const std::string& name, RenderContext& context, float size)
		: Primitive(name, context, 1), size(size) {
	}

protected:
    // Slices not used in example but can be used to set the resolution for the other primitives
    CG::MeshBuilder generate(int slices) const override;
};