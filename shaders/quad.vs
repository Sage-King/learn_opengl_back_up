#version 460 core

layout(location = 0) in vec2 in_pos;

uniform mat4 scale;
uniform mat4 translate;

void main()
{
	gl_Position = translate * scale * vec4(in_pos.x, in_pos.y, 0, 1.0), -1.0f, 1.0f; //what are these two -1.0f, 1.0f what; it works though?
}