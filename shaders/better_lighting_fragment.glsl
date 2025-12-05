#version 330 core

#define NR_POINT_LIGHTS 4

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirectionalLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
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

struct SpotLight {
    vec3 position;
    vec3 direction;
    float innerCutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform DirectionalLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform vec3 viewPosition;


float calcAttenuation(float constant, float linear, float quadratic, float distance);
float calcIntensity(float theta, float innerCutOff, float outerCutOff);

vec3 calcAmbient(vec3 lightAmbient);
vec3 calcDiffuse(vec3 lightDiffuse, vec3 normal, vec3 lightDir);
vec3 calcSpecular(vec3 lightSpecular, vec3 normal, vec3 lightDir, vec3 viewDir);

vec3 calcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() {
    vec3 normal = normalize(Normal);
    vec3 viewDirection = normalize(viewPosition - FragPos);

    vec3 result = vec3(0.0);

    result += calcDirectionalLight(dirLight, normal, viewDirection);

    for (int i = 0; i < NR_POINT_LIGHTS; i++) {
        result += calcPointLight(pointLights[i], normal, FragPos, viewDirection);
    }

    result += calcSpotLight(spotLight, normal, FragPos, viewDirection);

    FragColor = vec4(result, 1.0);
}

float calcAttenuation(float constant, float linear, float quadratic, float distance) {
    float denom = constant + linear * distance + quadratic * distance * distance;
    return 1 / denom;
}

float calcIntensity(float theta, float innerCutOff, float outerCutOff) {
    float epsilon = innerCutOff - outerCutOff;
    return clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);
}

vec3 calcAmbient(vec3 lightAmbient) {
    return lightAmbient * vec3(texture(material.diffuse, TexCoords));
}

vec3 calcDiffuse(vec3 lightDiffuse, vec3 normal, vec3 lightDir) {
    float diff = max(dot(normal, lightDir), 0.0);
    return lightDiffuse * diff * vec3(texture(material.diffuse, TexCoords));
}

vec3 calcSpecular(vec3 lightSpecular, vec3 normal, vec3 lightDir, vec3 viewDir) {
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    return lightSpecular * spec * vec3(texture(material.specular, TexCoords));
}

vec3 calcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);

    vec3 ambient = calcAmbient(light.ambient); 
    vec3 diffuse = calcDiffuse(light.diffuse, normal, lightDir);
    vec3 specular = calcSpecular(light.specular, normal, lightDir, viewDir); 
    return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);

    float attenuation = calcAttenuation(
        light.constant, light.linear, light.quadratic, distance
    );

    vec3 ambient = calcAmbient(light.ambient); 
    vec3 diffuse = calcDiffuse(light.diffuse, normal, lightDir);
    vec3 specular = calcSpecular(light.specular, normal, lightDir, viewDir); 
    return (ambient + diffuse + specular) * attenuation;
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);

    float attenuation = calcAttenuation(
        light.constant, light.linear, light.quadratic, distance
    );

    float theta = dot(lightDir, normalize(-light.direction));
    float intensity = calcIntensity(theta, light.innerCutOff, light.outerCutOff);

    vec3 ambient = calcAmbient(light.ambient); 
    vec3 diffuse = calcDiffuse(light.diffuse, normal, lightDir);
    vec3 specular = calcSpecular(light.specular, normal, lightDir, viewDir); 
    return ambient + ((diffuse + specular) * attenuation * intensity);
}
