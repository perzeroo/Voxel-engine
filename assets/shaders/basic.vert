#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

out vec3 vertexColor;

uniform mat4 u_ViewProjection;

void main() {
    gl_Position = u_ViewProjection * vec4(aPos, 1.0);

    vertexColor = aNormal * 0.5 + 0.5;
}

