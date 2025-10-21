#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

out vec2 vUV;

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;

void main() {
    gl_Position = u_ViewProjection * u_Model * vec4(aPos, 1.0);
    vUV = aUV;
}

