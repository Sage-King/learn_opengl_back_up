#ifndef PHYSICS_CIRCLE_H_GUARD
#define PHYSICS_CIRCLE_H_GUARD

#include "Circle.h"
#include <glm/glm.hpp>
namespace Sage
{
	class Physics_Circle
	{
	public:
		Physics_Circle(const glm::vec4& in_color = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f),
			const glm::vec2& in_pos = glm::vec2(0.0f, 0.0f),
			const float& in_radius = 1.0f,
			const float& in_num_of_points = 100.0f,
			const glm::vec2& in_vel = glm::vec2(0.0f),
			const glm::vec2& in_acc = glm::vec2(0.0f),
			const glm::vec2& in_force = glm::vec2(0.0f),
			const float& in_mass = 1.0f,
			const glm::vec2& in_jerk = glm::vec2(0.0f)
			);
		void update(float delta_time);
		void draw();
		float distance_to(Sage::Physics_Circle in_circle);
		void set_radius_clamp_to_window(float in_radius);
		void set_pos_clamp_to_window(glm::vec2 in_pos);
	public:
		Circle circle;
		glm::vec2 jerk, acc, vel, force;
		float mass;
		bool rate_limited = false;
		float last_rate_limit = 0.0f;
	};
}

#endif