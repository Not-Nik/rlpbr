#version 330 core
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;

uniform mat4 matView;
uniform mat4 matProjection;
uniform mat4 matModel;

void main() {
    TexCoords = vertexTexCoord;
    WorldPos = vec3(matModel * vec4(vertexPosition, 1.0));
    Normal = mat3(matModel) * vertexNormal;

    gl_Position =  matProjection * matView * vec4(WorldPos, 1.0);
}