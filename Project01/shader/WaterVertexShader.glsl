#version 330 core

in vec2 position;

out vec4 clipSpace;
out vec2 textureCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

const float tiling = 5.0;

void main()
{
	clipSpace =  projection * view * model * vec4(position.x,0,position.y,1.0);
	gl_Position = clipSpace;
	textureCoords = vec2(position.x/2.0 + 0.5, position.y/2.0 + 0.5);
}