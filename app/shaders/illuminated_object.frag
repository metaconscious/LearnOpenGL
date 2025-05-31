#version 330 core

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light
{
    vec3 position;
    vec3 direction;
    float cutoff;

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
uniform Light light;
uniform vec3 viewPos;
uniform vec3 objectColor;

void main() {
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

    float dist = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * dist * dist);

    vec3 lightDirection = normalize(light.position - FragPos);
    float theta = dot(lightDirection, normalize(-light.direction));

    vec3 result;

    if (theta > light.cutoff)
    {
        vec3 norm = normalize(Normal);
        float diff = max(dot(norm, lightDirection), 0.0);
        vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse, TexCoords)));

        vec3 viewDirection = normalize(viewPos - FragPos);
        vec3 reflectDirection = reflect(-lightDirection, norm);
        float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
        vec3 specular = light.specular * (spec * vec3(texture(material.specular, TexCoords)));

        result = attenuation * (ambient + diffuse + specular) * objectColor;
    }
    else
    {
        result = ambient * objectColor;
    }

    FragColor = vec4(result, 1.0);
}
