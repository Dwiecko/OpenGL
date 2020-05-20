#version 330

layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec2 inUV;
layout( location = 2 ) in vec3 inNormal;
layout( location = 3 ) in mat4 localMatrix;


uniform mat4 matProj;
uniform mat4 matView;
uniform mat4 matModel;
uniform int Frame;
out vec4 inoutPos;
out vec2 inoutUV;


void main()
{
    vec4 newPos = inPosition;
   // newPos.z += Frame*0.1f;


    float newX = newPos.x * 0.86602540378 - newPos.y * 0.5f;
    float newY = newPos.x * 0.5f + newPos.y * 0.86602540378f;
    newPos.x += (newX);
    newPos.y -= (Frame * 0.1f);



    gl_Position = matProj * matView * localMatrix * newPos;

    inoutPos = inPosition;
    inoutUV = inUV;
}
