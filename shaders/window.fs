#version 330 core

in vec2 textureCoordinates;
 
uniform sampler2D inTexture;

out vec4 fragColor;

void main()
{
	fragColor = texture(inTexture, textureCoordinates);
}