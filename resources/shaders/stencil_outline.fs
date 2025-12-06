#version 330

out vec4 FragColor;

uniform vec3 outlineColor;

void main() {
    FragColor = vec4(outlineColor, 0.2);
}
