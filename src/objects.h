#pragma once

#include "CG/scene.h"
#include "CG/shader.h"
#include "CG/mesh.h"
#include "CG/texture.h"

#include "glm/vec4.hpp"

#include <memory>
#include <string>


// Example scene object that draws a mesh with its own shader program.
// The mesh is drawn either with per-vertex colors, a texture, or a uniform tint color.
// Use this class as a reference for how to set up shaders, uniforms and draw calls.
class MeshObject : public CG::SceneObject {
public:
    std::shared_ptr<CG::Mesh> mesh;
    std::shared_ptr<CG::Texture> texture;
    glm::vec4 tint_color = glm::vec4(0); // rgb = tint color, a = blend factor (0 = no tint, 1 = only tint color)

protected:
    std::shared_ptr<CG::ShaderProgram> shader;
    int debug_mode = 0; // 0: none, 1: normals, 2: texture coordinates

public:
    MeshObject(const std::string& name = "Unnamed")
        : CG::SceneObject(name) {}

    // Compiles the shader program. Called once by SceneHierarchy::init() after the OpenGL context exists.
    void init() override;

    // Draws the mesh with the world matrix computed by the scene hierarchy.
    void render(const CG::CameraData& cam) override;

    // GUI controls shown for this object in the scene inspector.
    void inspector_gui() override;
};