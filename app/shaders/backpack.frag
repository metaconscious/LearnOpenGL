#version 330 core

struct Material
{
    sampler2D diffuses[1];
    sampler2D speculars[1];
    sampler2D normals[1];
    sampler2D heights[1];
    float shininess;
};

struct DirectionalLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;
    float cutoff;
    float outerCutoff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform Material material;
uniform DirectionalLight directionalLight;
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform vec3 viewPos;

vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection)
{
    vec3 lightDirection = normalize(-light.direction);

    vec3 ambient = light.ambient * vec3(texture(material.diffuses[0], TexCoords));

    float diff = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuses[0], TexCoords)));

    vec3 reflectDirection = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * vec3(texture(material.speculars[0], TexCoords)));

    return ambient + diffuse + specular;
}

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDirection)
{
    vec3 lightDirection = normalize(light.position - fragPos);

    float dist = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * dist * dist);

    vec3 ambient = light.ambient * vec3(texture(material.diffuses[0], TexCoords));

    float diff = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuses[0], TexCoords)));

    vec3 reflectDirection = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * vec3(texture(material.speculars[0], TexCoords)));

    return attenuation * (ambient + diffuse + specular);
}

vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDirection)
{
    float dist = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * dist * dist);

    vec3 lightDirection = normalize(light.position - fragPos);
    float theta = dot(lightDirection, normalize(-light.direction));
    float epsilon = light.cutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);

    vec3 ambient = light.ambient * vec3(texture(material.diffuses[0], TexCoords));

    float diff = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuses[0], TexCoords)));

    vec3 reflectDirection = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * vec3(texture(material.speculars[0], TexCoords)));

    return attenuation * (ambient + intensity * (diffuse + specular));
}

void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDirection = normalize(viewPos - FragPos);

    vec3 result = calculateDirectionalLight(directionalLight, norm, viewDirection);

    for (int i = 0; i < NR_POINT_LIGHTS; i++)
    {
        result += calculatePointLight(pointLights[i], norm, FragPos, viewDirection);
    }

    result += calculateSpotLight(spotLight, norm, FragPos, viewDirection);

    FragColor = vec4(result, 1.0);

    //    FragColor = texture(material.diffuses[0], TexCoords);
}
