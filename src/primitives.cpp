#include "primitives.h"

#include "CG/math.h"

#include "GL/glew.h"
#include "glm/gtc/type_ptr.hpp"
#include "imgui/imgui.h"

#include <cstdio>


// ===================================================================================================
// Geometry generation
// ===================================================================================================
namespace // anonymous namespace for helper functions, avoiding name clashes with other translation units
{
    // Point on the unit circle in the xz plane. The angle grows counter-clockwise when seen from
    // above (+y). In a right-handed coordinate system with y up this means z = -sin(phi).
    glm::vec3 circle_point(float phi)
    {
        return glm::vec3(glm::cos(phi), 0.0f, -glm::sin(phi));
    }

    void add_triangle(CG::MeshBuilder& b, unsigned int i0, unsigned int i1, unsigned int i2)
    {
        b.indices.push_back(i0);
        b.indices.push_back(i1);
        b.indices.push_back(i2);
    }

    // Disk at height y with the given radius, facing up (+y) or down (-y). Used for the frustum caps.
    // Texture coordinates map the disk onto the inscribed circle of the texture.
    void add_cap(CG::MeshBuilder& b, float radius, float y, int slices, bool facing_up)
    {
        const glm::vec3 normal(0.0f, facing_up ? 1.0f : -1.0f, 0.0f);
        const unsigned int center = (unsigned int)b.positions.size();
        b.positions.push_back(glm::vec3(0.0f, y, 0.0f));
        b.normals.push_back(normal);
        b.texcoords.push_back(glm::vec2(0.5f));

        // The rim needs its own vertices (and does not share the ones of the side surface),
        // because a vertex can only have one normal and the edge between side and cap is sharp.
        for (int i = 0; i <= slices; i++) {
            float phi = float(TAU) * float(i) / float(slices);
            glm::vec3 c = circle_point(phi);
            b.positions.push_back(radius * c + glm::vec3(0.0f, y, 0.0f));
            b.normals.push_back(normal);
            b.texcoords.push_back(glm::vec2(0.5f + 0.5f * c.x, 0.5f - 0.5f * c.z));
        }
        for (int i = 0; i < slices; i++) {
            unsigned int rim_i = center + 1 + i;
            // Counter-clockwise seen from the side the normal points to
            if (facing_up)
                add_triangle(b, center, rim_i, rim_i + 1);
            else
                add_triangle(b, center, rim_i + 1, rim_i);
        }
    }
}

CG::MeshBuilder Cube::generate(int /*slices*/) const
{
    CG::MeshBuilder b;
    float half = 0.5f * size;
    // 6 faces, each with 4 vertices (no sharing, because the normals are different)
    for (int f = 0; f < 6; f++) {
        glm::vec3 normal(0.0f);
        normal[f / 2] = (f % 2 == 0) ? -1.0f : 1.0f; // -x, +x, -y, +y, -z, +z
        glm::vec3 u_axis(0.0f), v_axis(0.0f);
        u_axis[(f / 2 + 1) % 3] = 1.0f; // next axis in x,y,z order
        v_axis[(f / 2 + 2) % 3] = (f % 2 == 0) ? -1.0f : 1.0f; // last axis in x,y,z order, flipped for the back faces
        unsigned int base_index = (unsigned int)b.positions.size();
        for (int j = -1; j <= 1; j += 2) {
            for (int i = -1; i <= 1; i += 2) {
                glm::vec3 pos = half * normal + half * float(i) * u_axis + half * float(j) * v_axis;
                b.positions.push_back(pos);
                b.normals.push_back(normal);
                b.texcoords.push_back(glm::vec2(0.5f * float(i + 1), 0.5f * float(j + 1)));
            }
        }
        add_triangle(b, base_index, base_index + 1, base_index + 3);
        add_triangle(b, base_index, base_index + 3, base_index + 2);
    }
    return b;
}

// ===================================================================================================
// Primitive scene object
// ===================================================================================================

void Primitive::set_resolution(int new_slices)
{
    new_slices = std::max(new_slices, primitives::min_slices);
    if (new_slices == slices)
        return;
    slices = new_slices;
    if (mesh) // before init() the mesh is created there
        rebuild_mesh();
}

void Primitive::init()
{
    rebuild_mesh();
}

void Primitive::rebuild_mesh()
{
    CG::MeshBuilder data = generate(slices);
    triangles = int(data.indices.size() / 3);
    // MeshBuilder::build() uploads positions, normals and texture coordinates into separate VBOs
    // with the attribute locations 0, 1, 2 (matching phong.vert.glsl) and creates the index buffer.
    // The previous mesh is released automatically when the shared_ptr is replaced (RAII).
    mesh = data.build();
}

void Primitive::render(const CG::CameraData& cam)
{
    if (!mesh || context.phong_shader.handle == 0)
        return;

    const GLuint program = context.phong_shader;
    glUseProgram(program);

    // Transformation chain: clip = projection * view * model * vertex.
    // Normals must be transformed with the inverse transpose of the model matrix. For rotations and
    // uniform scaling this equals the model matrix itself, but with non-uniform scaling the normals
    // would otherwise no longer be perpendicular to the surface.
    glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(world_matrix)));
    glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(world_matrix));
    glUniformMatrix3fv(glGetUniformLocation(program, "normal_matrix"), 1, GL_FALSE, glm::value_ptr(normal_matrix));
    glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(cam.view));
    glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_FALSE, glm::value_ptr(cam.projection));
    glUniform3fv(glGetUniformLocation(program, "camera_position"), 1, glm::value_ptr(cam.position));

    // Material
    glUniform3fv(glGetUniformLocation(program, "material.diffuse"), 1, glm::value_ptr(material.diffuse));
    glUniform3fv(glGetUniformLocation(program, "material.specular"), 1, glm::value_ptr(material.specular));
    glUniform1f(glGetUniformLocation(program, "material.shininess"), material.shininess);
    glUniform1i(glGetUniformLocation(program, "material.emissive"), material.emissive ? 1 : 0);

    // Textures: each sampler reads from a texture unit. The unit number is set as sampler uniform,
    // the texture is bound to that unit with glActiveTexture + glBindTexture.
    const bool has_texture = material.texture && material.texture->tex_id != 0;
    const bool has_overlay = material.overlay && material.overlay->tex_id != 0;
    glUniform1i(glGetUniformLocation(program, "has_texture"), has_texture ? 1 : 0);
    glUniform1i(glGetUniformLocation(program, "has_overlay"), has_overlay ? 1 : 0);
    glUniform1i(glGetUniformLocation(program, "base_texture"), 0);
    glUniform1i(glGetUniformLocation(program, "overlay_texture"), 1);
    if (has_texture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, material.texture->tex_id);
    }
    if (has_overlay) {
        // Shift the overlay texture coordinates along u depending on the animation time.
        // fract() keeps the offset small; the texture repeats in u (GL_REPEAT), so the result is the same.
        float offset = float(glm::fract(material.overlay_revolutions * clock.phase()));
        glUniform1f(glGetUniformLocation(program, "overlay_offset"), offset);
        glUniform2fv(glGetUniformLocation(program, "overlay_scale"), 1, glm::value_ptr(material.overlay_scale));
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, material.overlay->tex_id);
    }

    // Light sources
    context.lighting.set_uniforms(program);

    glBindVertexArray(mesh->vao);
    glDrawElements(mesh->draw_mode, mesh->index_count, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}

void Primitive::inspector_gui()
{
    // Resolution of this single object
    int value = slices;
    ImGui::SetNextItemWidth(150);
    if (ImGui::SliderInt("Slices", &value, primitives::min_slices, 128))
        set_resolution(value);
    ImGui::SameLine();
    ImGui::Text("%d triangles", triangles);
    ImGui::SetNextItemWidth(150);
    ImGui::ColorEdit3("Diffuse", glm::value_ptr(material.diffuse), ImGuiColorEditFlags_NoInputs);
}
