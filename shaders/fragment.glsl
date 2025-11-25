#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform sampler2D texture1;
uniform sampler2D texture2;

void main() {
    FragColor = vec4(lightColor * objectColor, 1.0);
}

