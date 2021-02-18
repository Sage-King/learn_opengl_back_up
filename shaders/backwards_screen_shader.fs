#version 330 core

out vec4 frag_color;

in vec2 tex_coords;

uniform sampler2D backwards_screen_texture;

void main()
{
	frag_color = vec4(vec3(texture(backwards_screen_texture, tex_coords)),1.0);
}
