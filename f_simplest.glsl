#version 330

uniform sampler2D textureMap0;
uniform sampler2D textureMap1;

out vec4 pixelColor; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela

in vec4 ic;
in vec4 n;
in vec4 l;
in vec4 v;
in vec2 iTexCoord0;

void main(void) {
	// Sun-like lighting parameters
	vec4 sunColor = vec4(1.0, 0.98, 0.85, 1.0);  // Slightly yellow sun color
	float ambientStrength = 0.6;                 // Ambient light strength
	float specularStrength = 0.4;                // Specular reflection strength
	float shininess = 70.0;                      // Shininess factor (higher = smaller highlight)

	//Znormalizowane interpolowane wektory
	vec4 ml = normalize(l);
	vec4 mn = normalize(n);
	vec4 mv = normalize(v);
	//Wektor odbity
	vec4 mr = reflect(-ml, mn);

	//Parametry powierzchni
	vec4 kd = texture(textureMap0, iTexCoord0);
	vec4 ks = texture(textureMap1, iTexCoord0);

	// Ambient component
	vec4 ambient = vec4(kd.rgb * ambientStrength, kd.a);

	// Diffuse component (directional sunlight)
	float diff = max(dot(mn, ml), 0.0);
	vec4 diffuse = vec4(kd.rgb * diff * sunColor.rgb, kd.a);

	// Specular component
	float spec = pow(max(dot(mv, mr), 0.0), shininess);
	vec4 specular = vec4(ks.rgb * spec * specularStrength * sunColor.rgb, 0.0);

	// Combined lighting
	pixelColor = ambient + diffuse + specular;
}
