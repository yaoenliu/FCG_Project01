#version 330 core
layout (location = 0) in vec3 aPos;
layout(location = 1) in mat4 models;

out vec4 ParticleColor;

uniform mat4 projection;
uniform vec3 offset;
uniform vec4 color;
uniform mat4 view;
uniform mat4 model;

void main()
{
	gl_Position = projection * view * models*model * vec4(aPos + offset, 1.0);
	ParticleColor = color;
}
