#version 460 core

layout(location = 0) in vec2 in_pos;

uniform mat4 translate;

void main()
{
	gl_Position = translate * vec4(in_pos,0.0f,1.0f);
}