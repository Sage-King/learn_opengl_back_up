#version 460 core

layout(points) in;
layout(triangle_strip, max_vertices = 256) out;

uniform float radius;
uniform float num_of_points;

void main()
{
	float radius_times_2 = radius * 2; //i don't really understand why this shader uses the diameter and not the radius
	float num = num_of_points - 1;
	float fraction_of_circle = (2 * 3.141592)/num;
	for (float i = 0; i < num_of_points; i += 1)
	{
		float frac_time_i = fraction_of_circle * i;

		gl_Position = gl_in[0].gl_Position;
	    EmitVertex();
		gl_Position = gl_in[0].gl_Position + vec4(cos(frac_time_i) * radius_times_2,sin(frac_time_i) * radius_times_2,0.0,0.0);
		EmitVertex();
	}
	EndPrimitive();
}