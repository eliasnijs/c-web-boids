#version 300 es
precision mediump float;

in vec2 vTexCoord;
out vec4 FragColor;

uniform float u_size;

void main() {
    float edgeThickness = 0.5 / u_size; // Adjust this value to change line thickness

    if (vTexCoord.x > edgeThickness &&
        vTexCoord.x < 1.0 - edgeThickness &&
        vTexCoord.y > edgeThickness &&
        vTexCoord.y < 1.0 - edgeThickness) {
        discard;
    }

    FragColor = vec4(0.5, 0.0, 0.0, 1.0);
}
