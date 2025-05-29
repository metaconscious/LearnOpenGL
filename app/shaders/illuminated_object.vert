#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 Normal;
out vec3 FragPos;

uniform mat3 normalMatrix;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    vec3 worldPos = vec3(model * vec4(aPos, 1.0));
    vec3 viewPos = vec3(view * vec4(worldPos, 1.0));
    FragPos = viewPos;
    Normal = normalMatrix * aNormal; // Expensive. Do this on CPU time

    gl_Position = projection * vec4(FragPos, 1.0);
}
