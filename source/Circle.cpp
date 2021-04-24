#include "Circle.h"
#include <algorithm>
#include <GLFW/glfw3.h>
namespace Sage
{
	Circle::Circle(
		const glm::vec4& in_color,
		const glm::vec2& in_pos, 
		const float& in_radius,
		const float& in_num_of_points)
		:
		color(in_color),
		pos(in_pos),
		radius(in_radius),
		num_of_points(in_num_of_points)
	{
		shader = Shader("shaders\\circle.vs", "shaders\\circle.gs", "shaders\\circle.fs");
		set_color(color);
		set_pos(pos);
		set_radius(radius);
		set_num_of_points(num_of_points);
		
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		unsigned int vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		float verts[] =
		{
			0.0f
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)0);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &vbo);
	}

	void Circle::draw()
	{
		shader.use();
		shader.setFloat("time", (float)glfwGetTime());
		glBindVertexArray(vao);
		glDrawArrays(GL_POINTS, 0, 1);
		glBindVertexArray(0);
	}

	bool Circle::is_intersecting(const Circle& in_circle)
	{
		return fabs(((this->pos.x - in_circle.pos.x) * (this->pos.x - in_circle.pos.x)) + ((this->pos.y - in_circle.pos.y) * (this->pos.y - in_circle.pos.y)) <= ((this->radius + in_circle.radius) * (this->radius + in_circle.radius)));
	}

	void Circle::set_color(const glm::vec4& in_color)
	{
		color = in_color;
		shader.use();
		shader.setVec4("color", color);
	}
	
	void Circle::set_radius(const float& in_radius)
	{
		radius = in_radius;
		radius = std::max(0.0f, radius);
		shader.use();
		shader.setFloat("radius", radius);
	}

	void Circle::set_pos(const glm::vec2& in_pos)
	{
		pos = in_pos;
		shader.use();
		float temp_x = (pos.x * 2) - 1;
		float temp_y = (pos.y * 2) - 1;
		glm::mat4 translate(1.0f);
		translate = glm::translate(glm::mat4(1.0f), glm::vec3(temp_x, temp_y, 0.0f));
		shader.setMat4("translate", translate);
	}

	void Circle::set_num_of_points(const float& in_num)
	{
		num_of_points = in_num;
		num_of_points = std::floor(in_num);  

		shader.use();
		shader.setFloat("num_of_points", num_of_points);
	}

	glm::vec2 Circle::get_pos()
	{
		return pos;
	}
	float Circle::get_radius()
	{
		return radius;
	}
}