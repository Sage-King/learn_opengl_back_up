#include "Physics_Circle.h"

#include <algorithm>

namespace Sage
{
		Physics_Circle::Physics_Circle(const glm::vec4& in_color,
			const glm::vec2& in_pos,
			const float& in_radius,
			const float& in_num_of_points,
			const glm::vec2& in_vel,
			const glm::vec2& in_acc,
			const glm::vec2& in_force,
			const float& in_mass,
			const glm::vec2& in_jerk
			)
			:
			vel(in_vel),
			acc(in_acc),
			force(in_force),
			mass(in_mass),
			jerk(in_jerk)
		{
			circle = Circle(in_color, in_pos, in_radius, in_num_of_points);
		}


		glm::vec2 operator*(glm::vec2 vec2, float factor)
		{
			vec2.x = vec2.x * factor;
			vec2.y = vec2.y * factor;
			return { vec2.x, vec2.y };
		}

		template<typename T>
		void print(T printable)
		{
			std::cout << std::to_string(printable) << '\n';
		}
		void print(std::string printable)
		{
			std::cout << printable << '\n';
		}
		void Physics_Circle::update(float delta_time)
		{
			glm::vec2 temp_pos = circle.get_pos();
			temp_pos += vel * delta_time;
			circle.set_pos(temp_pos);
		}

		void Physics_Circle::draw()
		{
			circle.draw();
		}

		float Physics_Circle::distance_to(Sage::Physics_Circle in_circle)
		{
			return sqrtf(((in_circle.circle.get_pos().y - circle.get_pos().y) * (in_circle.circle.get_pos().y - circle.get_pos().y)
		   +(in_circle.circle.get_pos().x - circle.get_pos().x) * (in_circle.circle.get_pos().x - circle.get_pos().x)));
		}

		void Physics_Circle::set_pos_clamp_to_window(glm::vec2 in_pos)
		{
			in_pos.x = std::clamp(in_pos.x, 0.0f + circle.get_radius(), 1.0f - circle.get_radius());
			in_pos.y = std::clamp(in_pos.y, 0.0f + circle.get_radius(), 1.0f - circle.get_radius());
			circle.set_pos(in_pos);
		}

		void Physics_Circle::set_radius_clamp_to_window(float in_radius)
		{
			in_radius = std::clamp(in_radius, 0.0f, 0.499999999f);
			circle.set_radius(in_radius);
		}


}