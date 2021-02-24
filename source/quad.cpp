#include "quad.h"
#include <algorithm>

Quad::Quad(glm::vec3 color, float width, float height, float in_x = 0.0f, float in_y = 0.0f) 
   :width(width),
	height(height),
	color(color),
	x(in_x),
	y(in_y)
{
	shader = Shader("shaders\\quad.vs", "shaders\\quad.fs");

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	float verts[] =
	{
		-1.0f, 1.0f,
		 1.0f, 1.0f,
		 1.0f,-1.0f,

		-1.0f, 1.0f,
		 1.0f,-1.0f,
		-1.0f,-1.0f
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDeleteBuffers(1, &vbo);
}

void Quad::draw()
{
	shader.use();

	glm::mat4 scale = glm::mat4(1.0f);
	scale = glm::scale(scale, glm::vec3(width, height, 0.0f));
	shader.setMat4("scale", scale);

	glm::mat4 translate = glm::mat4(1.0f);
	x = std::clamp(x, (width/2), 1.0f - (width/2));
	float temp_x = (x * 2) - 1;
	float temp_y = (y * 2) - 1;
	translate = glm::translate(translate, glm::vec3(temp_x, -temp_y, 0.0f));
	shader.setMat4("translate", translate);

	shader.setVec3("color", color);

	glBindVertexArray(vao);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

bool Quad::isIntersecting(Quad in_quad)
{
	float x_edge = x + (width / 2);
	float neg_x_edge = x - (width / 2);
	float y_edge = y + (height / 2);
	float neg_y_edge = y - (height / 2);

	float in_x_edge = in_quad.x + (in_quad.width / 2);
	float in_neg_x_edge = in_quad.x - (in_quad.width / 2);
	float in_y_edge = in_quad.y + (in_quad.height / 2);
	float in_neg_y_edge = in_quad.y - (in_quad.height / 2);

	return (neg_x_edge < in_x_edge && x_edge > in_neg_x_edge && 
			in_neg_y_edge < y_edge && in_y_edge > neg_y_edge);
}

float Quad::getWidth()
{
	return width;
}

void Quad::setWidth(float in_width)
{
	width = in_width;
}

float Quad::getHeight()
{
	return height;
}

void Quad::setHeight(float in_height)
{
	height = in_height;
}

glm::vec3 Quad::getColor()
{
	return color;
}

void Quad::setColor(glm::vec3 in_color)
{
	color = in_color;
}