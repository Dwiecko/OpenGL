#version 330

layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec2 inUV;

uniform mat4 screenMatProj;
uniform mat4 screenMatView;
uniform mat4 screenMatModel;

out vec4 inoutPos;
out vec2 inoutUV;


void main()
{
	gl_Position = screenMatProj * screenMatView * screenMatModel * inPosition;

	inoutPos = inPosition;
	inoutUV = inUV;
}
