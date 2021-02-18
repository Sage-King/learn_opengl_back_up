#version 330 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_tex_coordinates;

uniform mat4 mvp_transform;
uniform mat4 model_transform;

out vec3 normal;
out vec3 frag_position_world_space;

void main()
{
	gl_Position = mvp_transform * vec4(in_position,1.0);
	normal = mat3(transpose(inverse(model_transform))) * in_normal;
	frag_position_world_space = vec3(model_transform * vec4(in_position, 1.0)); 
}