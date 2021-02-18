#version 330 core

layout(location = 0) in vec3 in_pos;

out vec3 interpolated_vertex_pos;

uniform mat4 MVP_transform;

void main()
{
	vec4 temp_position = MVP_transform * vec4(in_pos,1.0);
	gl_Position = temp_position.xyww;
	interpolated_vertex_pos = in_pos;
}