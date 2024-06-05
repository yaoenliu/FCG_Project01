#version 330 core

in vec4 currPos;
in vec4 prevPos;
in float delta;

out vec4 oVelocity;

void main()
{
	vec2 a = (currPos.xy / currPos.w);
	vec2 b = (prevPos.xy / prevPos.w);
	oVelocity = vec4(vec2(a - b) * delta , 0 , 0);
}
