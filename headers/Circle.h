#ifndef CIRCLE_H_GUARD
#define CIRCLE_H_GUARD

#include <glm/glm.hpp>

#include "Shader.h"
constexpr double PI = 3.14159265358979;
/// <summary>
/// Circle class, but also general shape class if num_of_points is low.
/// Uses geometry shader to render circle.
/// 
/// Maybe TODO(?):
/// Add rotate and scale to vertex shader for low num_of_points.
/// </summary>
namespace Sage  
{
	class Circle 
	{
	public:
		Circle(
			const glm::vec4& in_color = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f),
			const glm::vec2& in_pos = glm::vec2(0.0f, 0.0f),
			const float& in_radius = 1.0f,
			const float& in_num_of_points = 100.0f);
		void draw();
		bool is_intersecting(const Circle& in_circle);
		void set_color(const glm::vec4& in_color);
		void set_radius(const float& in_radius);
		void set_radius_clamp_to_window(const float& in_radius);
		void set_pos(const glm::vec2& in_pos);
		void set_pos_clamp_to_window(const glm::vec2& in_pos);
		void set_num_of_points(const float& in_num);
		glm::vec2 get_pos();
		float get_radius();
	private:
		float num_of_points;
		unsigned int vao;
		Shader shader;
		glm::vec4 color;
		glm::vec2 pos;
		float radius;
	};
}
#endif