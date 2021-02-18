#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTextureCoordinates;

out vec2 textureCoordinates;

uniform mat4 MVPtransform;

void main()
{
	gl_Position = MVPtransform * vec4(inPosition, 1.0);
	textureCoordinates = inTextureCoordinates;
}