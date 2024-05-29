#version 330 core

in vec2 position;

out vec4 clipSpace;
out vec2 textureCoords;
out vec3 toCameraVector;
out vec3 fromLightVector;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3  cameraPos;
uniform vec3 lightPosition;


const float tiling = 5.0;

void main()
{
	vec4 wordPosition =  model * vec4(position.x,0,position.y,1.0);
	clipSpace =  projection * view * wordPosition;
	gl_Position = clipSpace;
	textureCoords = vec2(position.x/2.0 + 0.5, position.y/2.0 + 0.5);
	toCameraVector = cameraPos - wordPosition.xyz;
	fromLightVector = wordPosition.xyz - lightPosition;
}