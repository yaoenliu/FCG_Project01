#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D velocityTexture;
uniform float blurAmount;

void main()
{
	vec4 color = texture(screenTexture, TexCoords);
	vec2 velocity = texture(velocityTexture, TexCoords).xy;

	vec4 blurredColor = color;
	for(int i = 1 ; i<= 16 ;i++)
	{
		blurredColor += texture(screenTexture, TexCoords + velocity * i * blurAmount);
	}
	blurredColor /= 16.0f;

	FragColor = blurredColor;
	//FragColor = color;
}