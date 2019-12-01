uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;

void main()
{
	vec4 source = texture2D(tex1,gl_TexCoord[0].st);
	vec4 mask = texture2D(tex2, gl_TexCoord[0].st);
	vec4 target = texture2D(tex0, gl_TexCoord[0].st);

	vec4 final = source;

	if(mask.x == 0.0)
	{
		final = target;
	}
	
	gl_FragColor = final;
}
