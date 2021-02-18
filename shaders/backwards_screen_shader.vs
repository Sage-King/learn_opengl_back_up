#version 330 core

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inTex;

out vec2 tex_coords;

uniform mat4 transform;

void main()
{
	gl_Position = transform * vec4(inPos.x, inPos.y, 0.0, 1.0);
	tex_coords = inTex;
}