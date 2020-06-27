#version 330 core

layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec3 inNormal;
layout( location = 2 ) in vec2 inTextureCoords;

out vec4 ourPosition;
out vec3 ourNormal;
out vec2 ourTextureCoords;

uniform mat4 Matrix_proj_mv;

void main()
{
    gl_Position = Matrix_proj_mv * inPosition;

    ourPosition = inPosition;
    ourNormal = inNormal;
    ourTextureCoords = inTextureCoords;
}
