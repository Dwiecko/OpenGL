#version 330

layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec3 inNormal;

uniform mat4 matProj;
uniform mat4 matView;
uniform mat4 matModel;

uniform mat4 lightProj;
uniform mat4 lightView;


out vec4 ourPos;
out vec4 ourPosLight;
out vec3 ourNormal;
out vec2 ourUV;



void main()
{
	gl_Position = matProj * matView * matModel * inPosition;


	ourPos = inPosition;
	ourPosLight = lightProj * lightView * inPosition;
	ourNormal = inNormal;

}
