#version 330 core
out vec4 frag_color;

in vec3 normal;
in vec3 frag_position_world_space;

uniform vec3 camera_pos;
uniform samplerCube skybox;

void main()
{
	//air/glass
	float ratio = 1.0/1.52;
	vec3 incoming = normalize(frag_position_world_space - camera_pos);
	vec3 reflected = reflect(incoming, normalize(normal));
	vec3 refracted = refract(incoming, normalize(normal), ratio);
	frag_color = vec4(texture(skybox, refracted).rgb,1.0);
}