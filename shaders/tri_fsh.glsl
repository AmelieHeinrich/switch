#version 460

layout (location = 0) in vec2 fTexCoords;
layout (location = 1) in vec3 fNormals;

layout (location = 0) out vec4 outColor;

layout (binding = 0) uniform sampler2D texture0;

void main()
{
    outColor = texture(texture0, vec2(fTexCoords.x, 1.0 - fTexCoords.y));
}