#pragma once

#include "krake.h"
#include "orbit_camera.h"

#include "CG/application.h"
#include "CG/scene.h"


// Template of exercise sheet 1: the "Krake" ride at a fairground at night.
class TemplateApp : public CG::Application {
public:
	// Declaration order matters: members are destroyed in reverse order, and the scene objects keep
	// references to the render context, so the context must outlive the scene.
	RenderContext context;    // shared shaders, lights and animation clock
	CG::SceneHierarchy scene; // scene graph of the ride
	KrakeParts parts;         // pointers into the scene graph
	OrbitCamera camera;       

	TemplateApp(const char* title = "CG Template", int width = 1280, int height = 720);

	// Advances camera, animation and lights. dt is the time since the last frame in seconds.
	void update(double dt) override;

	// Draws skybox, scene, paths and GUI.
	void render() override;

	// Describes the ImGui user interface (called every frame from render()).
	void draw_gui();

private:
	static constexpr int global_path_samples = 720; // samples per period, enough for smooth curves

	bool wireframe = false;
};
