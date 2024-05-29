#version 330 core

in vec2 position;

out vec4 clipSpace;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	clipSpace =  projection * view * model * vec4(position.x,0,position.y,1.0);
	gl_Position = clipSpace;
}