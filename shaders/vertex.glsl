#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

uniform mat4 model;
uniform mat3 normalMatrix;
uniform mat4 viewProjection;

void main() {
    vec4 worldPos = model * vec4(aPos, 1.0);
    gl_Position = viewProjection * worldPos;

    Normal = normalMatrix * aNormal;
    FragPos = vec3(worldPos);
    TexCoords = aTexCoords;
}

