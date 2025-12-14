#shader vertex
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = projection * view * vec4(FragPos, 1.0); // Optimization: use FragPos
}

#shader fragment
#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos;
uniform vec3 lightDir;
uniform vec3 viewPos; // Note: This uniform is not used in your calculations, but kept it
uniform vec3 color;
uniform float cutOff;
uniform float outerCutOff;

void main(){
    // Ambient light
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * color;

    // Diffuse light
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(lightPos - FragPos);

    float diff = max(dot(norm, lightDirection), 0.0);

    // Spotlight effect
    float theta = dot(lightDirection, normalize(-lightDir));
    float epsilon = cutOff - outerCutOff;
    float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);

    // Combine results
    vec3 diffuse = intensity * diff * color;
    vec3 result = ambient + diffuse;

    FragColor = vec4(result, 1.0);
}
