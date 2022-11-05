uniform sampler2D quadTexture;
uniform float alpha;

void main(void)
{
	float gray = dot(texture2D(quadTexture, gl_TexCoord[0].xy).rgb, vec3(0.299, 0.587, 0.114));
	gl_FragColor = vec4(gray, gray, gray, alpha);
}