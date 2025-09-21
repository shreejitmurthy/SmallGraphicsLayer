#version 410

// SGL's AttributeBuilder output's one "colour" Vector4.
in vec4 colour;

// We must output a Vector4 for the colour, naming doesn't matter.
out vec4 FragColor;

// uniform vec4 tint;

void main() {
    FragColor = vec4(1.0, 0.5, 0.0, 1.0);
}