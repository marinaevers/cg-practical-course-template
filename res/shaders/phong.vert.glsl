// Vertex shader for all primitives of the ride.
// Core profile only (no fixed-function pipeline): "#version 410 core" is prepended by
// CG::ShaderPreprocessor, so the files contain no #version line.

// Attribute locations match CG::BufferIndex (positions 0, normals 1, texture coordinates 2),
// which are used by CG::MeshBuilder::build() in glVertexAttribPointer.
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;

uniform mat4 model;          // object -> world
uniform mat3 normal_matrix;  // inverse transpose of the upper 3x3 part of model
uniform mat4 view;           // world -> camera
uniform mat4 proj;           // camera -> clip space

out vec3 world_position;
out vec3 world_normal;
out vec2 texcoord;

void main(void)
{
    vec4 world = model * vec4(in_position, 1.0);
    world_position = world.xyz;
    world_normal = normal_matrix * in_normal; // normalized again in the fragment shader after interpolation
    texcoord = in_texcoord;
    gl_Position = proj * view * world;
}
