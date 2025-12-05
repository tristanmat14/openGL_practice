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
    vec3 direction;
    float innerCutOff;
    float outerCutOff;

    int type;
    // 1 => point source
    // 2 => directional source
    // 3 => spotlight source

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    // attenuation coefficients
    float constant;
    float linear;
    float quadratic;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

float calculateAttenuation(float distance) {
    return 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
}

void main() {
    vec3 lightDir;
    float attenuation = 1.0; // no attenuation
    float intensity = 1.0;

    // point source
    if (light.type == 1) {
        lightDir = light.position - FragPos;
        attenuation = calculateAttenuation(length(lightDir));
        lightDir = normalize(lightDir);
    }

    // directional source
    if (light.type == 2) {
        lightDir = normalize(-light.direction);
    }

    // spotlight source
    if (light.type == 3) {
        lightDir = light.position - FragPos;
        attenuation = calculateAttenuation(length(lightDir));
        lightDir = normalize(lightDir);
        
        float theta = dot(lightDir, normalize(-light.direction));
        float epsilon = light.innerCutOff - light.outerCutOff;
        intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    }

    // ambient 
    vec3 ambient = vec3(texture(material.diffuse, TexCoords)) * light.ambient;

    // diffuse
    vec3 norm = normalize(Normal);
    float diffuseCoeff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseCoeff * vec3(texture(material.diffuse, TexCoords)) * light.diffuse;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float specularCoeff = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = specularCoeff * vec3(texture(material.specular, TexCoords)) * light.specular;

    vec3 result;
    
    if (light.type == 3) {
        result = ambient + ((diffuse + specular) * attenuation * intensity);
    } else {
        result = (ambient + diffuse + specular) * attenuation;
    }

    FragColor = vec4(result, 1.0);
}