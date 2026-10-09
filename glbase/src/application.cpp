#include "CG/application.h"

#include "GL/glew.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <exception>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>

using namespace CG;


static void GLAPIENTRY gl_message_callback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    if (type != GL_DEBUG_TYPE_ERROR) return;
    // Set a debug breakpoint here to quickly find the source of the error (will be the previous OpenGL call)
    printf("[OpenGL] Error 0x%04X: %s\n", type, message);
}

static void glfw_error_callback(int error, const char* description)
{
    printf("[GLFW] Error %d: %s\n", error, description);
}


CG::Application::Application(const char* title, int width, int height)
    : viewport(width, height)
{
    init(title, width, height);
}


void CG::Application::init(const char* win_title, int win_width, int win_height)
{
    if (initialized) return;

    std::set_terminate([]() {
        try {
            std::exception_ptr eptr{ std::current_exception() };
            if (eptr) {
                std::rethrow_exception(eptr);
            }
            else {
                printf("Exiting without exception\n");
            }
        }
        catch (const std::exception& e) {
            printf("Uncaught exception: %s\n", e.what());
        }
        exit(EXIT_FAILURE);
        });

    // Setup GLFW
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
    {
        printf("Error: glfwInit() failed\n");
        std::terminate();
    }

    // OpenGL 4.1 core profile + GLSL 410 (highest version available on macOS)
    const char* glsl_version = "#version 410 core";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // required on macOS
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // shown after positioning

    // Create window with graphics context (scaled for HiDPI monitors, returns 1 on macOS where the OS scales itself)
    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
    window = glfwCreateWindow((int)(win_width * main_scale), (int)(win_height * main_scale), win_title, nullptr, nullptr);
    if (window == nullptr)
    {
        printf("Error: glfwCreateWindow() failed. Does your graphics driver support OpenGL 4.1?\n");
        std::terminate();
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Center the window on the primary monitor
    if (const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor())) {
        int w, h;
        glfwGetWindowSize(window, &w, &h);
        glfwSetWindowPos(window, (mode->width - w) / 2, (mode->height - h) / 2);
    }
    glfwShowWindow(window);

    // Framebuffer size can differ from the requested window size (HiDPI, window manager)
    glfwGetFramebufferSize(window, &viewport.x, &viewport.y);

    // Register input callbacks. They must be installed before ImGui, which chains them.
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    double mx, my;
    glfwGetCursorPos(window, &mx, &my);
    last_mouse_pos = input.mouse_pos = glm::vec2(mx, my);

    // Setup GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        printf("Error: Failed to setup GLEW\n");
        std::terminate();
    }

    const GLubyte* versionstr = glGetString(GL_VERSION);
    const GLubyte* vendorstr = glGetString(GL_VENDOR);
    if (versionstr && vendorstr) {
        printf("Running OpenGL %s on %s hardware\n", versionstr, vendorstr);
    }
    else {
        printf("Failed to get OpenGL version or vendor\n");
    }

    // OpenGL error logging (only available with OpenGL 4.3 / KHR_debug, i.e. not on macOS)
    if (GLEW_KHR_debug && glDebugMessageCallback) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(gl_message_callback, nullptr);
    }
    else {
        printf("OpenGL debug output not available, use glGetError() for error checking\n");
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.IniFilename = NULL;                                    // Disable saving layout to ini file

    // Setup Dear ImGui style and scaling
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale); // Bake a fixed style scale
    style.FontScaleDpi = main_scale; // Set initial font scale

    // Setup Platform/Renderer backends (install_callbacks = true chains our callbacks registered above)
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    io.Fonts->AddFontDefaultVector();

    initialized = true;
}


CG::Application::~Application()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}


// ---- GLFW callbacks --------------------------------------------------------------------------------

void CG::Application::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (!app || key < 0 || key >= Input::key_count) return;
    if (action == GLFW_PRESS) {
        app->input.key_down[key] = true;
        app->input.key_pressed[key] = true;
    }
    else if (action == GLFW_RELEASE) {
        app->input.key_down[key] = false;
    }
}

void CG::Application::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    auto* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (!app || button < 0 || button >= Input::button_count) return;
    if (action == GLFW_PRESS) {
        app->input.mouse_down[button] = true;
        app->input.mouse_clicked[button] = true;
    }
    else if (action == GLFW_RELEASE) {
        app->input.mouse_down[button] = false;
    }
}

void CG::Application::cursor_pos_callback(GLFWwindow* window, double x, double y)
{
    auto* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (!app) return;
    glm::vec2 pos(x, y);
    app->input.mouse_delta += pos - app->last_mouse_pos;
    app->last_mouse_pos = pos;
}

void CG::Application::scroll_callback(GLFWwindow* window, double x_offset, double y_offset)
{
    auto* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (!app) return;
    app->input.mouse_scroll += (float)y_offset;
}

void CG::Application::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    auto* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (!app) return;
    app->viewport = glm::ivec2(width, height);
}

// -----------------------------------------------------------------------------------------------------


bool CG::Application::new_frame()
{
    // Reset per-frame input, then poll events (callbacks above fill in the input of this frame).
    // ImGui receives the same events. Query ImGui::GetIO().WantCaptureMouse / WantCaptureKeyboard
    // to find out whether ImGui wants to use the input exclusively (e.g. the mouse is over a GUI window).
    input.clear();
    glfwPollEvents();
    input.mouse_pos = last_mouse_pos;

    if (glfwWindowShouldClose(window))
        quit = true;

    // Don't render while minimized
    if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0 || viewport.x <= 0 || viewport.y <= 0)
    {
        ImGui_ImplGlfw_Sleep(10);
        return false;
    }

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    return !quit;
}

void CG::Application::lock_mouse()
{
    if (mouse_locked)
        return;
    mouse_locked = true;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
}

void CG::Application::unlock_mouse()
{
    if (!mouse_locked)
        return;
    mouse_locked = false;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}


void CG::run(Application& app)
{
    if (!app.initialized)
        throw std::logic_error("Application was not initialized before calling run()");

    app.last_frame_time = glfwGetTime();

    while (!app.quit)
    {
        double current_time = glfwGetTime();
        double dt = current_time - app.last_frame_time;
        app.last_frame_time = current_time;

        if (!app.new_frame())
            continue;
        app.update(dt);
        app.render();

        ImGui::Render();
        glViewport(0, 0, app.viewport.x, app.viewport.y);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(app.window);
    }
}
