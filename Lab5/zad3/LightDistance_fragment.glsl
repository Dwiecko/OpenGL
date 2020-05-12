#version 150 core

in vec4 ourPosition;
in vec3 ourNormal;

out vec4 outColor;

#define MAX_LIGHTS 3

// Parametry oswietlenia
uniform vec3 Light_Ambient;
uniform vec3 Light_Diffuse;
uniform vec3 Light_Position;


void main()
{
	vec3 Light_Positions[3];
	Light_Positions[0] = vec3(0.0, 0.0, 0.0);
	Light_Positions[1] = vec3(7.0, 0.0, -2.0);
	Light_Positions[2] = vec3(-7.0, 0.0, -8.0);

	vec3 objectColor = vec3(0.2, 0.8, 0.2);

	vec3 pointLights = vec3(0.0);
	for (int i=0; i<MAX_LIGHTS; i++)
	{	
		vec3 lightDir = normalize(Light_Positions[i] - vec3(ourPosition));
		
		float distance = length(Light_Positions[i] - vec3(ourPosition));
		
		float resultDiffuse = max(dot(ourNormal, lightDir), 0.0);
		resultDiffuse *= (1.0 / (1.0 + (0.25 * pow(distance, 2))));

		pointLights += resultDiffuse;
	}
	   
	vec3 result = (Light_Ambient + pointLights) * objectColor;

	outColor = vec4(result, 1.0);
}
