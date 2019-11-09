uniform int myColor;

void main()
{
	vec4 oColor = vec4(0, 1, 0, 1);	

	if(myColor == 2)
	{
		oColor = vec4(1, 0, 0, 1);
	}
	else if(myColor == 3)
	{
		oColor = vec4(0, 0, 1, 1);
	}

	gl_FragColor = oColor;
}
