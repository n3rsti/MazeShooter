#version 330
uniform sampler2D textureMap0;
uniform sampler2D textureMap1;
uniform vec3 cameraPos;

out vec4 pixelColor; //Output variable. Almost final pixel color.

//Varying variables
in vec4 ic;
in vec4 n;
in vec4 l;
in vec4 l2;
in vec4 v;

in vec2 iTexCoord0;
in vec3 frag;

void main(void) {
    vec4 mv = normalize(v);

    vec4 texColor=texture(textureMap0,iTexCoord0);
	//Normalized, interpolated vectors
	vec4 ml = normalize(l);
	vec4 ml2 = normalize(l2);
	vec4 mn = normalize(n);
	
    vec4 texColor1=texture(textureMap1,(mn.xy+1)/2);
	
	//Reflected vector
	vec4 mr = reflect(-ml, mn);
	vec4 mr2 = reflect(-ml2, mn);

	//Surface parameters
	vec4 kd = mix(texColor,texColor1,0.5);
	vec4 ks = vec4(1,0.7,0.3,1);

	//Lighting model computation
	float nl = clamp(dot(mn, ml), 0, 1);
	float rv = pow(clamp(dot(mr, mv), 0, 1), 1);
	
	float nl2 = clamp(dot(mn, ml2), 0, 1);
	float rv2 = pow(clamp(dot(mr2, mv), 0, 1), 10);
	
	
	float dist = length(cameraPos - frag);
    float radius = 10.0; // Light radius in world units
    float attenuation = clamp(1.0 - dist / radius, 0.0, 1.0);

    float dist2 = length(vec3(0.0, 10.0, 0.0) - frag);
    float radius2 = 10.0; // Light radius for second light, can tweak as needed
    float attenuation2 = clamp(1.0 - dist2 / radius2, 0.0, 1.0);

    // Sum both lights
    vec3 diffuse = kd.rgb * (nl * attenuation + nl2 * attenuation2);
    float spec = rv * attenuation + rv2 * attenuation2;

    pixelColor = vec4(diffuse, kd.a) + vec4(ks.rgb * spec, 0.0);

}
