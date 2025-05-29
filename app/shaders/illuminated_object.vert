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
    vec4 worldPos = model * vec4(aPos, 1.0);
    vec4 viewPos = view * worldPos;
    FragPos = viewPos.xyz;
    Normal = normalMatrix * aNormal; // Expensive. Do this on CPU time

    gl_Position = projection * viewPos;
}
