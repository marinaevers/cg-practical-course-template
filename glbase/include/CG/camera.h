#pragma once

#include "CG/application.h"
#include "CG/transform.h"
#include "CG/math.h"



namespace CG
{
	// Perspective camera with free movement in 3D using keyboard and mouse.
	class FreeCam3D {
	public:
        Transform tf;                      // Camera position and orientation in world space
		float fovy = 60;                   // Vertical field of view in degrees
		float near_z = 0.1f;               // Near clipping plane distance. Values closer to zero reduce depth buffer precision, causing increased z-fighting and other depth-related artifacts.
		float far_z = 200.0f;              // Far clipping plane distance. Everything further away is clipped.
		glm::vec3 up = glm::vec3(0, 1, 0); // Up vector for the camera

		float sensitivity = 0.003f;        // Mouse sensitivity in radians per pixel of mouse movement
        float move_speed = 5;              // Base movement speed in units per second
        float shift_multiplier = 2;        // Movement speed multiplier when holding down shift
		bool move_axis_aligned = true;     // If true, WASD movement is constrained to the XZ plane, and vertical movement to the Y axis. If false, movement is applied relative to the camera orientation.

	protected:
		float aspect = 1;

	public:
		FreeCam3D() = default;

		glm::mat4 get_projection()
		{
			// Standard OpenGL perspective projection (right-handed, depth range [-1, 1]).
			// Use with glClearDepth(1) and glDepthFunc(GL_LESS) (the OpenGL defaults).
			return glm::perspective(glm::radians(fovy), aspect, near_z, far_z);
		}

		glm::mat4 get_view()
		{
			glm::vec3 view_dir = tf.get_forward_vec();
			return glm::lookAt(tf.translation, tf.translation + view_dir, up);
		}

		// Reads mouse and keyboard input of the current frame and moves the camera.
		// Mouse movement rotates the camera only if read_mouse_movement is true (e.g. while a mouse button is held).
		// Controls: WASD to move, Space/C to move up/down, Shift to move faster.
		void update(float dt, Application* app, bool read_mouse_movement = true)
		{
			if (app->viewport.y > 0)
				aspect = (float)app->viewport.x / app->viewport.y;

			if (read_mouse_movement) {
				// mouse movement is already a per-frame quantity, so it is NOT multiplied by dt
				glm::vec2 mouse_move = sensitivity * app->input.mouse_delta;
				tf.rotation.x = glm::clamp<float>(tf.rotation.x - mouse_move.y, -0.499f * PI, 0.499f * PI);
				tf.rotation.y = angle_clamp(tf.rotation.y - mouse_move.x, 2.f * PI);
			}
			float pitch = tf.rotation.x;
			float yaw = tf.rotation.y;

			glm::vec3 move_fwd, move_horz, move_vert;
			if (move_axis_aligned) {
				move_fwd = glm::vec3(-sin(yaw), 0, -cos(yaw));
				move_horz = glm::vec3(cos(yaw), 0, -sin(yaw));
				move_vert = glm::vec3(0, 1, 0);
			}
			else {
				move_fwd = glm::vec3(
					-sin(yaw) * cos(pitch),
					sin(pitch),
					-cos(yaw) * cos(pitch)
				);
				move_horz = glm::normalize(glm::cross(move_fwd, glm::vec3(0, 1, 0)));
				move_vert = glm::cross(move_horz, move_fwd);
			}

			const bool* pressed = app->input.key_down;
			if (pressed[GLFW_KEY_LEFT_SHIFT])
				dt *= shift_multiplier;
			if (pressed[GLFW_KEY_W])
				tf.translation += move_fwd * move_speed * dt;
			else if (pressed[GLFW_KEY_S])
				tf.translation -= move_fwd * move_speed * dt;
			if (pressed[GLFW_KEY_A])
				tf.translation -= move_horz * move_speed * dt;
			else if (pressed[GLFW_KEY_D])
				tf.translation += move_horz * move_speed * dt;
			if (pressed[GLFW_KEY_C] || pressed[GLFW_KEY_LEFT_CONTROL])
				tf.translation -= move_vert * move_speed * dt;
			else if (pressed[GLFW_KEY_SPACE])
				tf.translation += move_vert * move_speed * dt;
		}
	};

	// Orthogonal camera with panning and zooming in the XY plane.
	class FreeCam2D {
	public:
		Transform tf;               // Camera position in world space (x, y) only
		float fovy = 1;             // Readonly. Current vertical field of view in world units (affected by zoom level)
        float near_z = 1;           // Near clipping plane position (z coordinate)
        float far_z = -1;           // Far clipping plane position (z coordinate)

		float move_speed;           // Readonly. Current movement speed in units per second (affected by zoom level)
		float shift_multiplier = 2; // Movement speed multiplier when holding down shift
        bool scroll_enabled = true; // If true, mouse scrolling zooms in and out
		int zoom_level = 0;         // Current zoom level

	protected:
		int min_zoom_level;
		int max_zoom_level;
		float aspect = 1;
		float speed_factor;

	public:
		FreeCam2D()
		{
			set_zoom_behavior(true);
		}

		glm::mat4 get_projection()
		{
			float hy = 0.5f * fovy;
			float hx = hy * aspect;
			return glm::ortho(-hx, hx, -hy, hy, near_z, far_z);
		}

		glm::mat4 get_view()
		{
			return glm::translate(glm::vec3(-tf.translation.x, -tf.translation.y, near_z));
		}

		void update(float dt, Application* app)
		{
			if (app->viewport.y > 0)
				aspect = (float)app->viewport.x / app->viewport.y;

			int scroll_ticks = (int)glm::round(app->input.mouse_scroll);
			if (scroll_enabled && scroll_ticks != 0) {
				zoom_level = glm::clamp(zoom_level - scroll_ticks, min_zoom_level, max_zoom_level);
			}
			fovy = calc_fovy(zoom_level);
			move_speed = fovy * speed_factor;

			const bool* pressed = app->input.key_down;
			if (pressed[GLFW_KEY_LEFT_SHIFT])
				dt *= shift_multiplier;
			if (pressed[GLFW_KEY_W])
				tf.translation.y += move_speed * dt;
			else if (pressed[GLFW_KEY_S])
				tf.translation.y -= move_speed * dt;
			if (pressed[GLFW_KEY_A])
				tf.translation.x -= move_speed * dt;
			else if (pressed[GLFW_KEY_D])
				tf.translation.x += move_speed * dt;
		}

		void set_zoom_behavior(bool enable_zoom, float base_move_speed = 20, int min_zoom_level = -10, int max_zoom_level = 10)
		{
			scroll_enabled = enable_zoom;
			zoom_level = min_zoom_level + (max_zoom_level - min_zoom_level) / 2;
			this->min_zoom_level = min_zoom_level;
			this->max_zoom_level = max_zoom_level;
			speed_factor = base_move_speed / calc_fovy(zoom_level);
			fovy = calc_fovy(zoom_level); // update zoom now
			move_speed = fovy * speed_factor;
		}

	protected:
		float calc_fovy(float zoom)
		{
			return glm::exp(0.1f * zoom + 2.718f);
		}
	};

}