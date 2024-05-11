#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

struct Material 
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
uniform Material material;

struct MaterialWithoutTexture 
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light 
{
    vec3 position;
    vec3 color;
};

in vec3 FragPos;
in vec3 Normal;

uniform samplerCube skybox;
uniform MaterialWithoutTexture materialWithoutTexture;
uniform Light light;
uniform vec3 viewPos;
uniform bool hasTexture;
uniform int mapType;

void main() 
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float diff = max(dot(norm, lightDir), 0.0);
    float spec;

    if(mapType == 1)
    {
        vec3 I = normalize(FragPos - viewPos);
        vec3 R = reflect(I, normalize(Normal));
        FragColor = vec4(texture(skybox, R).rgb, 1.0);
        return;
    }
//    else if(mayType == 3)
//    {
//        float ratio = 1.00 / 1.52;
//        vec3 I = normalize(FragPos - viewPos);
//        vec3 R = refract(I, normalize(Normal), ratio);
//        FragColor = vec4(texture(skybox, R).rgb, 1.0);
//        return;
//    }
    float ambientStrength = 0.5;
    float diffuseStrength = 5.0;
    float specularStrength = 0.5;
    if(!hasTexture) 
    {   
        
        ambient = ambientStrength* materialWithoutTexture.ambient * light.color;
        diffuse = diffuseStrength *diff * materialWithoutTexture.diffuse * light.color;
        spec = pow(max(dot(viewDir, reflectDir), 0.0), materialWithoutTexture.shininess);
        specular = specularStrength* spec * light.color ;
    } else 
    {
        ambient = ambientStrength * vec3(texture(material.diffuse, TexCoords));
        diffuse = diffuseStrength * diff * vec3(texture(material.diffuse, TexCoords));
        specular = specularStrength * spec * vec3(texture(material.specular, TexCoords));
    }

    vec3 result = ambient + diffuse + specular;

    float intensity = dot(normalize(lightDir),Normal);
    
    if(mapType == 4)
	{
		if(intensity > 0.85)
		{
			result *= vec3(0.7,0.7,0.7);
		}
		else if(intensity > 0.5)
		{
			result *= vec3(0.5,0.5,0.3);
		}
		else if(intensity > 0.2)
		{
			result *= vec3(0.2, 0.2, 0.2);
		}
		else
		{
			result = vec3(0, 0, 0);
		}
		
        FragColor = vec4(result, 1.0);
        return ;
	}

    FragColor = vec4(result, 1.0);
}