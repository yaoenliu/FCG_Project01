#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in mat4 models;

out vec2 TexCoords;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform bool isDepth;
uniform mat4 lightSpaceMatrix;

void main() 
{
    if(isDepth)
	{
		gl_Position = lightSpaceMatrix * models * model * vec4(aPos, 1.0);
		return;
	}
    TexCoords = aTexCoords;
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = projection * view * models * model * vec4(aPos, 1.0);
}