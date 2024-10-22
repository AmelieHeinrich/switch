#version 460

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vTexCoords;

layout (location = 0) out vec3 fTexCoords;

layout (std140, binding = 0) uniform TestConstant {
    mat4 Projection;
    mat4 View;
} u;

void main()
{
    gl_Position = u.Projection * u.View * vec4(vPosition, 1.0);
    fTexCoords = vTexCoords;
}