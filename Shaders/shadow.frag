uniform sampler2D texture;

void main(void)
{
	gl_FragColor.a = texture2D(texture, gl_TexCoord[0].xy).r;
	if (gl_FragColor.a == 1.0) {
		discard;
	}
}