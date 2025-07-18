#version 330

layout(location = 0) in vec3 vertexPosition;
layout(location = 2) in vec3 vertexNormal;

uniform mat4 mvp;
uniform mat4 matModel;

out vec3 fragNormal;

void main() {
    gl_Position = mvp * vec4(vertexPosition, 1.0);

    fragNormal = normalize(mat3(matModel) * vertexNormal);
}
