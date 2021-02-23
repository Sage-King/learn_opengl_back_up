#version 330 core

layout(location = 0) in vec3 in_pos;

layout (std140) uniform matrices
{
	mat4 projection;
	mat4 view;
};

uniform mat4 model;

void main()
{
	gl_Position = projection * view * model * vec4(in_pos, 1.0);
}