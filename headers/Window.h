#ifndef WINDOW_H
#define WINDOW_H
#include "Shader.h"
#include "glm/glm.hpp"

class Window 
{
public:
	Window();
	void draw(Shader shader);
private:
	unsigned int texture;
	unsigned int VAO;
	unsigned int VBO;
};

#endif