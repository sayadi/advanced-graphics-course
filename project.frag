uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;

void main()
{
	vec4 Earth = texture2D(tex0, gl_TexCoord[0].st);
	vec4 Mars = texture2D(tex1, gl_TexCoord[0].st);
	vec4 flag =  texture2D(tex2, gl_TexCoord[0].st);
	vec4 black =  texture2D(tex3, gl_TexCoord[0].st);

	vec4 final = Earth;

	gl_FragColor = final;
}
