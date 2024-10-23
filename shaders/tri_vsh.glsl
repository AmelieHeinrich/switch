#version 460

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec2 vTexCoords;
layout (location = 2) in vec3 vNormals;

layout (location = 0) out vec2 fTexCoords;
layout (location = 1) out vec3 fNormals;

layout (std140, binding = 0) uniform Camera {
    mat4 Projection;
    mat4 View;
} u;

layout (std140, binding = 1) uniform Model {
    mat4 Model;
} m;

void main()
{
    gl_Position = u.Projection * u.View * m.Model * vec4(vPosition, 1.0);
    fTexCoords = vTexCoords;
    fNormals = vNormals;
}