#version 330 core
layout (location = 0) in vec4 inPosition;

uniform mat4 lightProj;
uniform mat4 lightView;

void main()
{
    gl_Position = lightProj * lightView * inPosition;
}
