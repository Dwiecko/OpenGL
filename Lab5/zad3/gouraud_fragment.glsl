#version 150 core

in vec4 ourPosition;
in vec3 ourNormal;

out vec4 outColor;


// Parametry oswietlenia
uniform vec3 Light_Ambient;
uniform vec3 Light_Diffuse;
uniform vec3 Light_Position;


void main()
{
	vec3 objectColor = vec3(0.2, 0.8, 0.2);

	float distance = length(Light_Position - vec3(ourPosition));
	vec3 lightVector = normalize(Light_Position - vec3(ourPosition));

	float diffuse = max(dot(ourNormal, lightVector), 0.1);
	diffuse = diffuse * (1.0 / (1.0 + (0.25 * pow(distance, 2))));

	//objectColor = objectColor * 0.25;
	vec3 result = (Light_Ambient + diffuse) * objectColor;

	outColor = vec4(result, 1.0);
}
