
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
