#version 330 core

in vec3 interpolated_vertex_pos;

out vec4 frag_color;

uniform samplerCube skybox_texture;

void main()
{
	frag_color = texture(skybox_texture, interpolated_vertex_pos);
}