#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	//this is from Learn OpenGL(book)
	//things in snake case are things I added
	//the program ID
	unsigned int ID;
	//constructor reads and builds the shader
	Shader(const char* vertexPath, const char* fragmentPath);
	Shader(const char* vertexPath, const char* geometry_shader, const char* fragmentPath);
	Shader() {};
	//use/activate/bind the shader
	void use();
	//utility uniform functions
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec3(const std::string& name, float val1, float val2, float val3) const;
	void setVec3(const std::string& name, const glm::vec3& value) const;
	void setVec4(const std::string& name, const glm::vec4& value) const;
	void setMat4(const std::string& name, const glm::mat4& value);
	void setMat3(const std::string& name, const glm::mat3& value);
};

#endif 