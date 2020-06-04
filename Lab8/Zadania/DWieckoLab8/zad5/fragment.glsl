#version 150

in vec4 ourPos;
in vec4 ourPosLight;
in vec3 ourNormal;

// Potrzebne do shadowMap i oswietlenia kierunkowego
uniform sampler2D shadowMap;
uniform vec3 Camera_Position;
uniform vec3 Light_Direction;


out vec4 outColor;


struct LightParameters
{
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
};


// ---------------------------------------------------------------------------
vec3 Calculate_DirectionalLight(vec3 lightDiffuse, vec3 lightSpecular, vec3 LightDirection)
{
	// diffuse
	vec3  lightDirection = normalize(LightDirection);
	float diffuseCoeff = max(dot(ourNormal, -lightDirection), 0.0);
	vec3  resultDiffuse = diffuseCoeff * vec3(lightDiffuse);
	vec3  diffusePart = resultDiffuse;

	// specular
	vec3  viewDir = normalize(Camera_Position - vec3(ourPos));
	vec3  reflectDir = reflect(lightDirection, ourNormal);
	float specularCoeff = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3  specularPart = specularCoeff * vec3(lightSpecular);

	// result
	return (diffusePart + specularPart);
}

// ---------------------------------------------------------------------------
float Calculate_Shadow(vec4 fragPosLight, vec3 Light_Direction)
{

	// perform perspective divide
    vec3 projCoords = fragPosLight.xyz / fragPosLight.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0)
        return 0.0;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

	// calculate bias according to the angle between normal vector and light vector
    float bias = max(0.004 * (1.0 - dot(ourNormal, Light_Direction)), 0.001);
	//bias = 0.0f;

    // Simple filtering
    // check whether current frag pos is in shadow
	// float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
	// return shadow;

	// Smooth filtering
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(float x = -1; x <= 1; x+=1.0)
	{
		for(float y = -1; y <= 1; y+=1.0)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;

	return shadow;
}




// ---------------------------------------------------------------------------
void main()
{

	LightParameters myLight;

	// Parametry swiatla
	myLight.Ambient = vec3(0.2, 0.2, 0.2);
	myLight.Diffuse = vec3(0.8, 0.8, 0.8);
	myLight.Specular = vec3(0.5, 0.5, 0.5);

	// Kolor wyjsciowy
	vec3  myColor = vec3(0.1, 1.0, 0.1);

	// Skladowe
	vec3  lightPart = Calculate_DirectionalLight(myLight.Diffuse, myLight.Specular, Light_Direction);

	float shadowPart = Calculate_Shadow(ourPosLight, Light_Direction);

	// Kolor finalny
	vec3  finalColor = (myLight.Ambient + (1.0f - shadowPart) *lightPart) * myColor;

	outColor = vec4(finalColor , 1.0);

}
