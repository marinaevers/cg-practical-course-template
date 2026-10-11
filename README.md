# Practical Course Computer Graphics – Template

Code template for the **Practical Course Computer Graphics** (Praktikum Computergraphik) of the
Computer Graphics and Multimedia Systems group, University of Siegen.

---

## Contents

- [Requirements](#requirements)
- [Getting the code](#getting-the-code)
- [Building](#building)
- [Running](#running)
- [Project structure](#project-structure)
- [Working with the template](#working-with-the-template)
- [Troubleshooting](#troubleshooting)
- [Submission](#submission)

---

## Requirements

| | Minimum |
|---|---|
| CMake | 3.24 |
| C++ compiler | with C++20 support: Visual Studio 2022, GCC 11+, Clang 14+ / Apple Clang (Xcode 14+) |
| GPU / driver | OpenGL **4.1** core profile |
| Git | any recent version |

All libraries (GLFW, GLEW, GLM, Dear ImGui, stb_image, tinyobjloader) are included in
`glbase/vendor` and built together with the project. You do **not** need to install them separately.

### Platform-specific setup

**Windows**
Install Visual Studio 2022 with the workload *Desktop development with C++* (this includes CMake).
Alternatively, any other C++20 compiler together with a separate CMake installation works.

**Linux (Ubuntu/Debian)**
GLFW needs the OpenGL and X11 development headers:

```bash
sudo apt install build-essential cmake git \
    libgl1-mesa-dev libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
```

On other distributions, install the equivalent packages. GLFW is built for X11 only by default; this
also works under Wayland via XWayland (see [Troubleshooting](#troubleshooting)).

**macOS**
Install the Xcode Command Line Tools (`xcode-select --install`) and CMake (e.g. `brew install cmake`).
macOS supports OpenGL only up to version 4.1. OpenGL is marked as deprecated on macOS, so you will 
see deprecation warnings, they can be ignored.

---

## Getting the code

You work in pairs and submit via your own repository. **Keep your repository private.** A public
copy of your solution makes plagiarism by others possible, and you are responsible for your code.

Note that a GitHub *fork* of a public repository is always public. Instead, create a private copy:

```bash
# 1. Create a new, empty, PRIVATE repository on GitHub (without README or .gitignore)

# 2. Clone the template and point it to your own repository
git clone https://github.com/marinaevers/cg-practical-course-template.git cg-praktikum
cd cg-praktikum
git remote rename origin template
git remote add origin https://github.com/<your-user>/<your-repo>.git
git push -u origin main

# 3. Add your partner and the course supervisor as collaborators
```

Keeping the template as a second remote (`template`) lets you fetch bug fixes later:

```bash
git fetch template
git merge template/main
```

Other submission options (a repository created by us, or our SVN server) are described in the
introduction slides in Moodle. If we create a repository for you, you can still follow the steps 
outlined above.

---

## Building

The project uses CMake. Always build **out of source**, i.e. in a separate `build` directory, which is
already excluded by `.gitignore`.

You can follow the same process for building as in the lecture *Computergraphik*. If you need help with
building a CMake project, please contact the tutors (as early as possible).

---

## Running

The executable is called **`CG-Praktikum`** and is located in the build directory.

```bash
./build/CG-Praktikum
```

You should see a window titled *Praktikum Computergraphik – Krake* with a rotating, lit cube and a
*Settings* window. The console prints the OpenGL version and GPU in use. If you get there, your setup works.

Controls:

| Input | Action |
|---|---|
| Left mouse button + drag | Rotate the camera *(to be implemented in task 4)* |
| Mouse wheel | Zoom (should already work in the template) |
| *Settings* window | Animation speed, wireframe mode, paths, lighting, resolution, scene inspector |

Shaders and textures are loaded at runtime from the `res` folder. Its absolute path is compiled into
the program, so you can start the executable from any working directory.

---

## Project structure

```
├── CMakeLists.txt        Build configuration of your application (add new files here!)
├── src/                  Your code
│   ├── template_app.*    Application class: main loop (update/render) and GUI (draw_gui)
│   ├── krake.*           Builds the scene graph of the ride; all dimensions and speeds are defined here
│   ├── primitives.*      Base class Primitive (mesh, material, resolution) and an example Cube
│   ├── animated_node.h   Scene node with fixed placement + rotation driven by the animation clock
│   ├── animation.h       AnimationClock: shared animation time, one period = 12 s
│   ├── orbit_camera.*    Camera on a sphere around the mast
│   ├── lighting.h        Light parameters and their upload to the shader
│   ├── render_context.h  Data shared by all objects (shader, lights, clock, path display mode)
│   └── objects.*         MeshObject: reference example for an object with its own shader
├── res/
│   ├── shaders/          GLSL shaders, loaded and compiled at runtime
│   └── textures/         Skybox, tent canvas, light bulbs, UV checker (see res/textures/README.md)
└── glbase/               Framework library – you normally do not need to change anything here
    ├── include/CG/       Application, scene graph, shaders, meshes, textures, cameras, input
    ├── src/
    └── vendor/           Third-party libraries (GLFW, GLEW, GLM, Dear ImGui, stb, ...)
```

Places where you are expected to add code are marked with `// TODO` comments (some of them, not all!). You will also need to
create new classes (e.g. for the primitives and the skybox).

---

## Working with the template

### Adding source files

New files are **not** picked up automatically. List every new file in the top-level
`CMakeLists.txt`.

Then re-run CMake.
If you forget a `.cpp` file, you get a **linker** error such as *unresolved external symbol*
(MSVC) or *undefined reference* (GCC/Clang).

### Shaders

- Shaders live in `res/shaders` and are compiled when the program starts. After changing a shader,
  restarting the program is enough; no recompilation of the C++ code is needed.
- Do **not** write a `#version` line. The framework prepends `#version 410 core` automatically
  (see `CG::ShaderPreprocessor` in `glbase/include/CG/shader.h`). It also resolves
  `#include "file"` directives.
- Compile and link errors are printed to the console. A program that failed to link has handle `0`.
- Add new shader files to `CG_SHADERS` in `CMakeLists.txt` so they appear in your IDE.

### GUI

The user interface uses [Dear ImGui](https://github.com/ocornut/imgui) in immediate mode: the whole
GUI is described every frame in `TemplateApp::draw_gui()`. To browse all available widgets, call
`ImGui::ShowDemoWindow()` once in `draw_gui()` and look at the corresponding code in
`glbase/vendor/imgui/imgui_demo.cpp`.

### Useful documentation

- OpenGL reference pages: <https://registry.khronos.org/OpenGL-Refpages/gl4/>
- GLSL specification: <https://registry.khronos.org/OpenGL/index_gl.php>
- GLM: <https://github.com/g-truc/glm>
- GLFW: <https://www.glfw.org/docs/latest/>
- Learn OpenGL (tutorial): <https://learnopengl.com>

---

## Troubleshooting

The template is new this semester and has only been tested on a limited set of hardware. If you
think you found a bug in the template, please report it, ideally with your operating system,
compiler, GPU and the console output.