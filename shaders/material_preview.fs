#version 330

in vec3 fragPos;
in vec3 fragNormal;

uniform vec3 lightDir;
uniform vec3 baseColor;

out vec4 finalColor;

void main() {
    vec3 norm = normalize(fragNormal);
    vec3 light = normalize(-lightDir);  // inverse because light comes *from* direction
    float diff = max(dot(norm, light), 0.0);

    vec3 color = baseColor * diff;
    finalColor = vec4(color, 1.0);
}
