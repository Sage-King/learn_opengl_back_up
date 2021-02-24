#ifndef QUAD_H_GUARD
#define QUAD_H_GUARD

#include "glm/glm.hpp"
#include "Shader.h"

class Quad
{
public:
	Quad(glm::vec3 color, float width, float height, float in_x, float in_y);
	Quad() :x(0.0f), y(0.0f), width(0.0f), height(0.0f), color(glm::vec3(0.0f)), vao(0) {};

	void draw();
	bool isIntersecting(Quad in_quad);

	float getWidth();
	void setWidth(float in_width);
	float getHeight();
	void setHeight(float in_height);
	glm::vec3 getColor();
	void setColor(glm::vec3 in_color);
	float x;
	float y;

private:
	glm::vec3 color;	
	float width;
	float height;
	Shader shader;
	unsigned int vao;
};
#endif