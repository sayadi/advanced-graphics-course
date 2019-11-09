varying float intensity;
uniform vec3 lPos;

void main()
{
	vec3 lDir = gl_Position - lPos;

	lDir = -normalize(lDir);

	intensity = dot(lDir, gl_Normal);
	
	gl_Position = ftransform();
}
