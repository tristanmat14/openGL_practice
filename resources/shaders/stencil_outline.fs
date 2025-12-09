#version 330

out vec4 FragColor;

uniform vec4 outlineColor;

void main() {
    FragColor = vec4(outlineColor);
}
