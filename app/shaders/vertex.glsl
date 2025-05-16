#version 330

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;// Unused

uniform vec3 aOffset;

out vec3 ourColor;

void main() {
    gl_Position = vec4(aPos.x + aOffset.x, -aPos.y + aOffset.y, aPos.z + aOffset.z, 1.0);
    ourColor = gl_Position.xyz;
}
