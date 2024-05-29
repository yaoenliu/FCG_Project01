#version 330 core

out vec4 FragColor;
in vec2 TexCoords;
in vec2 PrevTexCoords;

uniform sampler2D scene;
uniform float blurAmount;

void main()
{
	vec4 color = texture(scene, TexCoords);
	vec4 prevColor = texture(scene, PrevTexCoords);

	vec2 motionVector = TexCoords - PrevTexCoords;

	vec4 blurredColor = vec4(0.0);
	for(int i = -4 ; i<=4 ;i++)
	{
		blurredColor += texture(scene, TexCoords + motionVector * i * blurAmount);
	}
	blurredColor /= 9.0;

	FragColor = mix(color, blurredColor, 0.5);
}