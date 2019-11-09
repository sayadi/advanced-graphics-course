uniform vec3 lPos;
varying vec3 vPosition;
varying vec3 vNormal;


void main()
{
	vec3 normal = normalize(vNormal);
	
	vec3 lDir = vPosition - lPos;
	lDir = -normalize(lDir);

	float intensity = dot(lDir, normal);

	vec4 color;
	if (intensity > 0.95)
		color = vec4(1.0,0.5,0.5,1.0);
	else if (intensity > 0.5)
		color = vec4(0.6,0.3,0.3,1.0);
	else if (intensity > 0.25)
		color = vec4(0.4,0.2,0.2,1.0);
	else
		color = vec4(0.2,0.1,0.1,1.0);
	gl_FragColor = color;
}
