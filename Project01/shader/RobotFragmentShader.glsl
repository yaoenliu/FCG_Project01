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
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
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

    if(!hasTexture) 
    {
        ambient = light.ambient * materialWithoutTexture.ambient;
        diffuse = light.diffuse * (diff * materialWithoutTexture.diffuse);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), materialWithoutTexture.shininess);
        specular = light.specular * (spec * materialWithoutTexture.specular);
    } else 
    {
        ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
        diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
        specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    }

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}