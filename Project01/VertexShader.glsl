#version 430

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Color;

uniform mat4 modelMatrix;
uniform mat4 lookMatrix;
uniform mat4 projectMatrix;

out vec3 ex_Color;

void main()
{
	gl_Position =  projectMatrix * lookMatrix * modelMatrix * vec4(in_Position.x, in_Position.y, in_Position.z, 1.0);
	ex_Color = in_Color ;
}