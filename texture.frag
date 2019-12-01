uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;

uniform bool showClouds;

uniform vec3 lPos;

varying vec4 myPos;

varying vec4 myNorm;

uniform bool bumpN;

uniform bool highlight;

uniform bool animateClouds;

uniform bool diffuseLight;

uniform bool textureEarth;

void main()
{
	vec4 Earth = texture2D(tex0, gl_TexCoord[0].st);
	vec4 Normal = texture2D(tex1,gl_TexCoord[0].st);
	vec4 Cloud = texture2D(tex2, gl_TexCoord[0].st);
	vec4 Water = texture2D(tex3, gl_TexCoord[0].st);

	vec3 vPos = vec3(myPos.x, myPos.y, myPos.z);

	vec3 norm = vec3(Normal.x, Normal.y, Normal.z);
	norm = normalize(norm);

	vec3 vDir = vec3(0, 0, 1);

	vec3 lDir = normalize(vPos - lPos);

	vec3 normN = vec3(myNorm.x, myNorm.y, myNorm.z);
	normN = normalize(normN);

	float intensity;
	
	intensity = clamp(dot(lDir, normN), 0.0, 1.0);

	if (bumpN)
	{
		// Calculate intensity using norm, instead of normN
		intensity = clamp(dot(lDir, norm), 0.0, 1.0);
	}
	
	vec3 Reflection = lDir - 2.0 * dot(lDir, normN) * normN;
	Reflection = -normalize(Reflection);

	vec4 final = gl_Color * intensity;

	if (textureEarth)
	{
		// Set the final value to Earth
		final = Earth;
	}

	if (diffuseLight)
	{
		// Set the final value to 0.1 * Earth + 0.9 * intensity * Earth
		final = (0.1 * Earth) + (0.9 * intensity) * Earth;
	}

	if (animateClouds)
	{
		Cloud = Cloud + vec4(0.1, 0.0, 0.0, 0.0);
	}

	if (showClouds)
	{
		// Add the cloud texture to final
		final+=Cloud;
	}

	float keTerm = pow(clamp(dot(Reflection, vDir), 0.0, 1.0), 50.0);
	vec4 ksTerm = vec4(0.5, 0.5, 0.5, 0) * keTerm;

	if (highlight)
	{
		// Add ksTerm multiplied by the Water texture to final
		final+=Water * ksTerm;
	}

	gl_FragColor = final;
}
