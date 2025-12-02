#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    float diffuseStrength = 1.0;    
    float diffuseCoeff = max(dot(norm, lightDir), 0.0) * diffuseStrength;
    vec3 diffuse = diffuseCoeff * lightColor;

    float specularStrength = 0.5;
    float shininess = 32;
    float specularCoeff = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * specularCoeff * lightColor;

    FragColor = vec4((ambient + diffuse + specular) * objectColor, 1.0);
}