#include "template_app.h"

#include "GL/glew.h"
#include "glm/gtc/type_ptr.hpp"
#include "imgui/imgui.h"

#include <cstdio>


int main()
{
    auto app = TemplateApp("Praktikum Computergraphik - Krake", 1280, 720);
    CG::run(app);
}


TemplateApp::TemplateApp(const char* title, int width, int height)
    : Application(title, width, height)
{
    // Shaders are loaded from res/shaders at runtime. CG::Shader and CG::ShaderProgram check the
    // compile and link status and print the info log; a failed program has handle 0.
    context.phong_shader = CG::ShaderProgram(
        CG::Shader(GL_VERTEX_SHADER, "shaders/phong.vert.glsl"),
        CG::Shader(GL_FRAGMENT_SHADER, "shaders/phong.frag.glsl"),
        "phong");
    if (context.phong_shader.handle == 0)
        printf("Error: shader compilation failed, see messages above\n");

    // TODO: Initialize the skybox

    // Build the scene graph, then create all meshes and buffers
    parts = build_krake(scene, context);
    scene.init();
    scene.update(0.0);

    // TODO: The global paths never change, so they are computed once here
}

void TemplateApp::update(double dt)
{
    // Only use the mouse for the camera if it is not over a GUI window
    camera.update(*this, !ImGui::GetIO().WantCaptureMouse);

    // Advance the animation (scaled by the speed factor) and recompute all world matrices
    context.clock.advance(dt);
    scene.update(dt);
}

void TemplateApp::draw_gui()
{
    ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("%.1f FPS (%.2f ms)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
    ImGui::TextDisabled("Left mouse button: rotate camera, scroll wheel: zoom");

    if (ImGui::CollapsingHeader("Animation", ImGuiTreeNodeFlags_DefaultOpen)) {
        // 0 stops the animation, small values help to verify the paths
        ImGui::SliderFloat("Speed", &context.clock.speed, 0.0f, 5.0f, "%.2fx");
        ImGui::SameLine();
        if (ImGui::Button(context.clock.speed == 0.0f ? "Play" : "Stop"))
            context.clock.speed = context.clock.speed == 0.0f ? 1.0f : 0.0f;
        ImGui::ProgressBar(float(context.clock.phase()), ImVec2(-1.0f, 0.0f), "Period");
    }

    if (ImGui::CollapsingHeader("Rendering", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Checkbox("Wireframe", &wireframe); 
        int mode = int(context.path_display);
        ImGui::Combo("Paths", &mode, "None\0Local (relative to parent)\0Global (relative to plaza)\0");
        context.path_display = RenderContext::PathDisplay(mode);
    }

    if (ImGui::CollapsingHeader("Lighting", ImGuiTreeNodeFlags_DefaultOpen)) {
        SceneLighting& lights = context.lighting;
        // TODO: Color and cutoff angle of the spotlight
    }

    if (ImGui::CollapsingHeader("Resolution")) {
        // Number of segments around the circumference per type of object
        int total = 0;
        for (size_t i = 0; i < parts.resolution_groups.size(); i++) {
            ResolutionGroup& g = parts.resolution_groups[i];
            ImGui::PushID(int(i));
            ImGui::SetNextItemWidth(160);
            if (ImGui::SliderInt(g.label.c_str(), &g.slices, primitives::min_slices, 128))
                g.apply();
            ImGui::SameLine(330);
            ImGui::Text("%2d x %5d tris", int(g.members.size()), g.members.empty() ? 0 : g.members[0]->triangle_count());
            ImGui::PopID();
            total += g.triangle_count();
        }
        ImGui::Text("Total: %d triangles", total);
    }

    if (ImGui::CollapsingHeader("Scene hierarchy"))
        scene.inspector_gui();

    ImGui::End();
}

void TemplateApp::render()
{
    // The viewport follows the framebuffer size, so resizing the window works
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, viewport.x, viewport.y);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    // No face culling: rings and the roof are open surfaces that must be visible from both sides.
    glDisable(GL_CULL_FACE);

    CG::CameraData cam;
    cam.view = camera.get_view();
    cam.projection = camera.get_projection();
    cam.position = camera.get_position();
    cam.viewport = glm::vec2(viewport);

    // Wireframe mode for the models
    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
    scene.render(cam); // also draws the local paths, which are part of the hierarchy
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    draw_gui();
}
