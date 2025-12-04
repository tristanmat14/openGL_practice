#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

void main() {

    // ambient 
    vec3 ambient = vec3(texture(material.diffuse, TexCoords)) * light.ambient;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diffuseCoeff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseCoeff * vec3(texture(material.diffuse, TexCoords)) * light.diffuse;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float specularCoeff = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = specularCoeff * vec3(texture(material.specular, TexCoords)) * light.specular;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}