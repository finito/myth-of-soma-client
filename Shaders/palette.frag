uniform sampler2D texture;
uniform sampler2D paletteTexture;

void main(void)
{
	gl_FragColor = texture2D(paletteTexture, vec2(texture2D(texture, gl_TexCoord[0].xy).r, 1));
	if (gl_FragColor.rgb == vec3(1.0, 0.0, 1.0)) {
		discard;
	}
	// } else {
	
	gl_FragColor.a = gl_Color.a;
	if (gl_Color.rgb == vec3(0.0, 0.0, 0.0)) {
		float gray = dot(gl_FragColor.rgb, vec3(0.299, 0.587, 0.114));
		gl_FragColor.rgb = vec3(gray, gray, gray);
	} else if (gl_Color.rgb != vec3(1.0, 1.0, 1.0)) {
		float alpha = gl_Color.a / 1.75;
		gl_FragColor = vec4((gl_FragColor.rgb * (1.0 - alpha)) + (gl_Color.rgb * alpha), 1.0);			
	}
	//}
}