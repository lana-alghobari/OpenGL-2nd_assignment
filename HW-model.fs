#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal  = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}

#shader fragment
#version 330 core

out vec4 FragColor;

struct Material {
    float shininess;
};

struct PointLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 1

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 viewPos;
uniform vec3 objectColor;
uniform sampler2D textureSample;
uniform Material material;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform vec3 sunPos;
uniform vec3 earthPos;
uniform vec3 moonPos;
uniform float moonRadius;

uniform bool isEmissive;
uniform vec3 emissiveColor;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 color)
{
    vec3 lightDir = normalize(light.position - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    vec3 ambient  = light.ambient  * color;
    vec3 diffuse  = light.diffuse  * diff * color;
    vec3 specular = light.specular * spec;

    return (ambient + diffuse + specular) * attenuation;
}
float simpleShadow(vec3 fragPos, vec3 lightPos, vec3 occluderPos, float occluderRadius)
{
    vec3 L = lightPos - fragPos;
    vec3 Ldir = normalize(L);
    float Ldist = length(L);

    vec3 O = occluderPos - fragPos;

    float t = dot(O, Ldir);

    if(t <= 0.0 || t >= Ldist)
        return 0.0;

    vec3 closestPoint = fragPos + Ldir * t;

    float d = length(occluderPos - closestPoint);

    if(d > occluderRadius)
        return 0.0;


    float softness = 1.0 - smoothstep(0.0, occluderRadius, d);
    return softness;
}
void main()
{

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 texColor = texture(textureSample, TexCoord).rgb;
    vec3 baseColor = texColor * objectColor;

    vec3 result = vec3(0.0);
        float shadow = 0.0;
    
    if(!isEmissive) {

float shadowEarth = simpleShadow(FragPos, sunPos, earthPos, 0.3);
float shadowMoon  = simpleShadow(FragPos, sunPos, moonPos, moonRadius);

shadow = max(shadowEarth, shadowMoon);  
    }
    
    if(!isEmissive) {
        for(int i=0; i<NR_POINT_LIGHTS; i++) {
            vec3 lit = CalcPointLight(pointLights[i], norm, FragPos, viewDir, baseColor);
            vec3 shadowed = pointLights[i].ambient * baseColor;
            result = mix(lit, shadowed, shadow);
        }
    }
if(isEmissive)
{
    result = texColor + emissiveColor;
}
    FragColor = vec4(result, 1.0);
}
