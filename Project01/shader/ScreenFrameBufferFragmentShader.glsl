#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform bool isMosaic;

vec2 imageSize = vec2(1920, 108.0);
vec2 mosaicSize = vec2(1.0, 1.0);

void main()
{
    if (!isMosaic)
	{
		FragColor = texture(screenTexture, TexCoords);
		return;
	}
    vec2 pos = vec2(TexCoords.x*imageSize.x , TexCoords.y * imageSize.y);
    vec2 mosaicPos = vec2(floor(pos.x / mosaicSize.x) * mosaicSize.x, floor(pos.y / mosaicSize.y) * mosaicSize.y);
    vec2 texPos = vec2(mosaicPos.x / imageSize.x, mosaicPos.y / imageSize.y);
    vec3 col = texture(screenTexture, texPos).rgb;
    FragColor = vec4(col, 1.0);
} 