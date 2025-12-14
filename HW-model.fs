#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTex;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}


#shader fragment
#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

// ========= STRUCTS =========
struct PointLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    sampler2D texture_diffuse1;
    // sampler2D texture_specular1; // You can add this later
    float shininess;
};

// ========= UNIFORMS =========
uniform vec3 viewPos;
uniform PointLight pointLights[1];
uniform Material material;

// ========= FUNCTION PROTOTYPES =========
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

// ========= MAIN =========
void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = CalcPointLight(pointLights[0], norm, FragPos, viewDir);
    FragColor = vec4(result, 1.0);
}

// ========= FUNCTION IMPLEMENTATIONS =========
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    // Combine results
    vec3 ambient = light.ambient * texture(material.texture_diffuse1, TexCoord).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, TexCoord).rgb;
    vec3 specular = light.specular * spec; // Specular color usually comes from the light source

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}
