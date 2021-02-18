#include "Window.h"
#include "stb_image.h"
#include <filesystem>
#include <iostream>

void Window::draw(Shader shader)
{
	shader.use();
	shader.setInt("inTexture", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

Window::Window()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//float windowVertices[] = {
	//	// positions(3f)// texture coords(2f)
	//	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
	//	0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	//	0.5f, 0.5f, -0.5f,  1.0f, 1.0f,
	//	0.5f, 0.5f, -0.5f,  1.0f, 1.0f,
	//	-0.5f, 0.5f, -0.5f,  0.0f, 1.0f,
	//	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	//	-0.5f, -0.5f, 0.5f,  0.0f, 0.0f,
	//	0.5f, -0.5f, 0.5f,  1.0f, 0.0f,
	//	0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
	//	0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
	//	-0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
	//	-0.5f, -0.5f, 0.5f,  0.0f, 0.0f,
	//	-0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
	//	-0.5f, 0.5f, -0.5f,  1.0f, 1.0f,
	//	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	//	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	//	-0.5f, -0.5f, 0.5f,  0.0f, 0.0f,
	//	-0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
	//	0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
	//	0.5f, 0.5f, -0.5f,  1.0f, 1.0f,
	//	0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	//	0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	//	0.5f, -0.5f, 0.5f,  0.0f, 0.0f,
	//	0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
	//	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	//	0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	//	0.5f, -0.5f, 0.5f,  1.0f, 0.0f,
	//	0.5f, -0.5f, 0.5f,  1.0f, 0.0f,
	//	-0.5f, -0.5f, 0.5f,  0.0f, 0.0f,
	//	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	//	-0.5f, 0.5f, -0.5f,  0.0f, 1.0f,
	//	0.5f, 0.5f, -0.5f,  1.0f, 1.0f,
	//	0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
	//	0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
	//	-0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
	//	-0.5f, 0.5f, -0.5f, 0.0f, 1.0f
	//};
	float plane[] =
	{
		-0.5f, 0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
		 0.5f,-0.5f, 0.0f, 1.0f, 1.0f,

		-0.5f, 0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f,-0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f,-0.5f, 0.0f, 0.0f, 1.0f,
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(plane), plane, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	std::string filepath = std::filesystem::current_path().string() + "\\resources\\window2.png";
	int width, height, nrChannels;
	unsigned char* data;
	stbi_set_flip_vertically_on_load(false);
	data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture:: path = " << filepath << std::endl;
	}
	stbi_image_free(data);
}