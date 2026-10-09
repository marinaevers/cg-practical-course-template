#pragma once

#include "CG/input.h"

#include "glm/vec2.hpp"


namespace CG
{
	// Generic windowed application class.
	// Window, OpenGL context and user input are handled with GLFW, the GUI with Dear ImGui.
	// Subclass this and override update() and render() to add your own application logic.
	class Application {
	public:
		bool quit = false;            // Set to true to end the main loop
		GLFWwindow* window = nullptr; // GLFW window handle (owns the OpenGL context)
		Input input;                  // User input of the current frame
		glm::ivec2 viewport;          // Framebuffer size in pixels

	public:
		Application(const char* title = "CG Template", int width = 1280, int height = 720);
		virtual ~Application();

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		friend void run(Application& app);

		// Called once per frame. dt is the time in seconds since the previous frame.
		virtual void update(double /*dt*/) {}

		// Called once per frame after update(). Draw the scene and describe the GUI here.
		virtual void render() {}

		// Returns the time in seconds since application initialisation.
		double get_time_s() const {
			return glfwGetTime();
		}

		bool is_mouse_locked() const {
			return mouse_locked;
		}
		// Hides the cursor and keeps it inside the window (useful for first-person camera controls).
		void lock_mouse();
		void unlock_mouse();

	private:
		void init(const char* win_title, int win_width, int win_height);

		bool new_frame();

		// GLFW event callbacks, forwarding to the Application instance stored as the window user pointer.
		static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
		static void cursor_pos_callback(GLFWwindow* window, double x, double y);
		static void scroll_callback(GLFWwindow* window, double x_offset, double y_offset);
		static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

		bool initialized = false;
		double last_frame_time = 0;
		bool mouse_locked = false;
		glm::vec2 last_mouse_pos{ 0, 0 };
	};

	// Application entry point. This will block until the application is quit.
	void run(Application& app);
}
