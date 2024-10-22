#version 460

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vColor;

layout (location = 0) out vec3 fColor;

layout (std140, binding = 0) uniform TestConstant {
    mat4 ProjectionView;
} u;

void main()
{
    gl_Position = vec4(vPosition, 1.0) * u.ProjectionView;
    fColor = vColor;
}