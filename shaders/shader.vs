#version 330 core
layout (location = 0) in vec3 aPos;   
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 transform;
uniform mat4 modelToWorldTransform;
  
out vec2 texCoord;
out vec3 Normal;
out vec3 fragPos;

void main()
{
    gl_Position = transform * vec4(aPos, 1.0);
    fragPos = vec3(modelToWorldTransform * vec4(aPos, 1.0f));
    texCoord = aTexCoord;
    Normal = mat3(transpose(inverse(modelToWorldTransform))) * aNormal;
}   