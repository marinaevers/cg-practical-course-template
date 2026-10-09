#pragma once

#include "glm/vec2.hpp"

// GLEW must be included before any other OpenGL related header (including GLFW)
#include "GL/glew.h"
#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE // OpenGL headers come from GLEW, not from GLFW
#endif
#include "GLFW/glfw3.h"


namespace CG
{
	// Gathers the user input of the current frame into one place, so that it can be queried at any time during
	// update()/render(), instead of only inside event callbacks.
	// Keys are indexed by GLFW key codes (e.g. input.key_down[GLFW_KEY_W]),
	// mouse buttons by GLFW button codes (e.g. input.mouse_down[GLFW_MOUSE_BUTTON_LEFT]).
	// Note: mouse coordinates are given in window (screen) coordinates, which can differ from
	// framebuffer pixels (Application::viewport) on HiDPI displays.
	class Input {
	public:
		static constexpr int key_count = GLFW_KEY_LAST + 1;
		static constexpr int button_count = GLFW_MOUSE_BUTTON_LAST + 1;

		glm::vec2 mouse_pos{ 0, 0 };                  // Mouse position this frame (window coordinates, origin top left)
		glm::vec2 mouse_delta{ 0, 0 };                // Mouse movement since last frame
		float mouse_scroll = 0;                       // Vertical scroll wheel offset this frame (one "tick" = 1, touchpads may give fractions)
		bool mouse_down[button_count] = { false };    // Mouse buttons currently held down
		bool mouse_clicked[button_count] = { false }; // Mouse buttons newly pressed this frame
		bool key_down[key_count] = { false };         // Keys currently held down
		bool key_pressed[key_count] = { false };      // Keys newly pressed this frame

		// Resets all per-frame values. Called by the Application at the start of each frame.
		void clear() {
			mouse_delta = { 0, 0 };
			mouse_scroll = 0;
			for (bool& b : mouse_clicked) b = false;
			for (bool& b : key_pressed) b = false;
		}
	};
}
