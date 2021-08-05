#version 330 core
in vec3 vertexPosition;
layout (location = 1) in vec2 vertexTexCoord;
layout (location = 2) in vec3 vertexNormal;

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    TexCoords = vertexTexCoord;
    WorldPos = vec3(model * vec4(vertexPosition, 1.0));
    Normal = mat3(model) * vertexNormal;

    gl_Position =  projection * view * vec4(WorldPos, 1.0);
}