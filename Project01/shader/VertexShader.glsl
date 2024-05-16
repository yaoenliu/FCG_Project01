#version 330 core
layout(location = 0) in vec3 aPos;
uniform mat4 rotation;
uniform mat4 view;
uniform mat4 proj;
void main() 
{
	gl_Position = proj * view * rotation * vec4(aPos, 1.0);
}