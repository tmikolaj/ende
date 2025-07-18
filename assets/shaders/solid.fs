#version 330

in vec3 fragNormal;

uniform vec3 lightDir;
uniform vec3 baseColor;

out vec4 fragColor;

void main() {
    vec3 normal = normalize(fragNormal);
    float lighting = max(dot(normal, normalize(lightDir)), 0.0);

    float ambient = 0.2;
    vec3 shadedColor = baseColor * (ambient + (1.0 - ambient) * lighting);

    fragColor = vec4(shadedColor, 1.0);
}
