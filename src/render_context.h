#pragma once

#include "animation.h"
#include "lighting.h"

#include "CG/shader.h"

// Data shared by all drawable objects of the ride. Owned by the application, every object only keeps
// a reference to it. Sharing one shader program between all objects avoids compiling the same
// shader dozens of times.
struct RenderContext {
    // Which gondola paths are shown
    enum class PathDisplay : int { None = 0, Local = 1, Global = 2 };

    AnimationClock clock;            // Shared animation time
    SceneLighting lighting;          // Light sources, updated every frame
    PathDisplay path_display = PathDisplay::None;

    CG::ShaderProgram phong_shader;  // Blinn-Phong shader used by all primitives (res/shaders/phong.*.glsl)
};