#version 330 core

out vec4 fragColor;  

uniform vec3 lightColor;

//float near = 0.1;
//float far = 100.0;
//float LinearizeDepth(float depth)
//{
//float z = depth * 2.0 - 1.0; // back to NDC
//return (2.0 * near * far) / (far + near - z * (far - near));
//}
//void main()
//{
//	float depth = LinearizeDepth(gl_FragCoord.z) / far; // / far /for /demo
//	FragColor = vec4(vec3(depth), 1.0);
//}

void main()
{
    fragColor = vec4(lightColor,1.0);
}