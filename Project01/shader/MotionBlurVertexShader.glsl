#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;
out vec2 PrevTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 prevModel;


void main()
{
	gl_Position  =projection * view * model * vec4(aPos, 1.0);
	TexCoords = aTexCoords;

	vec4 prevPos = prevModel * vec4(aPos, 1.0);
	PrevTexCoords = prevPos.xy / prevPos.w*0.5 + 0.5;
}