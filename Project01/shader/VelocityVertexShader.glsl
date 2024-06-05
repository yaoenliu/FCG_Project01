#version 330 core
layout (location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in mat4 models;
out vec4 currPos;
out vec4 prevPos;
out float delta;

uniform mat4 currProj;
uniform mat4 currView;
uniform mat4 model;
uniform mat4 prevProj;
uniform mat4 prevView;
uniform mat4 preModel;
uniform float deltaTime;


void main()
{
	currPos = currProj * currView * models * model * vec4(aPos, 1.0);
	prevPos = prevProj * prevView * models * preModel * vec4(aPos, 1.0);
	delta = deltaTime;
	gl_Position = currPos;
}

