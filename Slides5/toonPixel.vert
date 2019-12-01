varying vec3 vPosition;
varying vec3 vNormal;

void main()
{
	vNormal = gl_Normal;
	vPosition = vec3(gl_Vertex.x, gl_Vertex.y, gl_Vertex.z);

	gl_Position = ftransform();
}
