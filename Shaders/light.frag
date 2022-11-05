uniform sampler2D diffuseTexture;
uniform sampler2D lightMapTexture;

void main(void)
{
	float ambientIntensity = 0.9;
	vec4 ambientColor = vec4(1.0, 1.0, 1.0, 1.0) * ambientIntensity;
	
	float lightIntensity = 1.0 - ambientIntensity;
	vec4 lightMapColor = texture2D(lightMapTexture, gl_TexCoord[0].xy) * lightIntensity;

	vec4 baseColor = texture2D(diffuseTexture, gl_TexCoord[0].xy);

	gl_FragColor = (ambientColor * baseColor) + (lightMapColor * baseColor);
}