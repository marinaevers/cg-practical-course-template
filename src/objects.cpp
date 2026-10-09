#include "objects.h"

#include "GL/glew.h"
#include "glm/gtc/type_ptr.hpp"
#include "imgui/imgui.h"

#include <cstdio>


void MeshObject::init()
{
    if (mesh && !texture && mesh->vbos[CG::BufferIndex_Colors] == 0 && tint_color.a == 0) {
        // Neither vertex colors nor a texture: without a tint the object would be invisible,
        // so assume the tint was forgotten and use it fully.
        tint_color.a = 1;
    }
    // Defines are inserted as "#define NAME value" after the "#version" line of the shader source,
    // so the same shader file can be compiled in different variants (see #ifdef in mesh.*.glsl).
    CG::ShaderDefines defines;
    if (mesh && mesh->vbos[CG::BufferIndex_Colors] != 0)
        defines["#VERTEX_COLOR"] = "";
    defines["#DEBUG"] = "";
    // Shader paths are relative to the resource directory (res/).
    shader = std::make_shared<CG::ShaderProgram>(
        CG::Shader(GL_VERTEX_SHADER, "shaders/mesh.vert.glsl", defines),
        CG::Shader(GL_FRAGMENT_SHADER, "shaders/mesh.frag.glsl", defines),
        "mesh");
}

void MeshObject::render(const CG::CameraData& cam)
{
    if (!mesh) {
        printf("MeshObject '%s' has no mesh set!\n", name.c_str());
        return;
    }
    if (!shader)
        return; // init() has not been called

    glBindVertexArray(mesh->vao);
    glUseProgram(*shader);

    glUniform1i(glGetUniformLocation(*shader, "has_texture"), texture ? 1 : 0);
    if (texture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(texture->target, texture->tex_id);
        glUniform1i(glGetUniformLocation(*shader, "tex"), 0);
    }

    glUniform4f(glGetUniformLocation(*shader, "color"), tint_color.r, tint_color.g, tint_color.b, tint_color.a);
    glUniform1i(glGetUniformLocation(*shader, "debug_mode"), debug_mode);

    glUniformMatrix4fv(glGetUniformLocation(*shader, "model"), 1, GL_FALSE, glm::value_ptr(world_matrix));
    glUniformMatrix4fv(glGetUniformLocation(*shader, "view"), 1, GL_FALSE, glm::value_ptr(cam.view));
    glUniformMatrix4fv(glGetUniformLocation(*shader, "proj"), 1, GL_FALSE, glm::value_ptr(cam.projection));

    // Indexed meshes are drawn with glDrawElements, meshes without index buffer with glDrawArrays.
    if (mesh->vbos[CG::BufferIndex_Indices] != 0)
        glDrawElements(mesh->draw_mode, mesh->index_count, GL_UNSIGNED_INT, nullptr);
    else
        glDrawArrays(mesh->draw_mode, 0, mesh->index_count);
}

void MeshObject::inspector_gui()
{
    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_FrameBgActive]);
    ImGui::Text("MeshObject");
    ImGui::PopStyleColor();

    ImGui::SetNextItemWidth(200);
    ImGui::ColorEdit4("Tint Color", glm::value_ptr(tint_color));
    ImGui::SetNextItemWidth(200);
    ImGui::Combo("Debug view", &debug_mode, "None\0Normals\0Texcoords\0");
}