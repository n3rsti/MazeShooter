#version 330 core

uniform sampler2D textureMap0; // diffuse map
uniform sampler2D textureMap1; // normal map
uniform sampler2D textureMap2; // height map
uniform sampler2D textureMap3; // ambient occlussion
uniform sampler2D textureMap4; // emissive

in vec4 ic;
in vec4 n;
in vec4 l;
in vec4 v;
in vec2 iTexCoord0;

out vec4 pixelColor;

vec2 parallaxTexCoords(vec4 v, vec2 t, float h, float s) {
    vec2 ti = v.xy / s;
    float hi = v.z / s;
    vec2 tc = t;
    float hc = h;
    float ht = texture(textureMap2, tc).r * h;

    if (v.z <= 0.0) discard;

    return tc; // Note: You can insert full parallax loop here if needed
}

void main(void) {
    vec4 mv = normalize(v);
    vec2 tc = parallaxTexCoords(mv, iTexCoord0, 0.1, 1000.0);

    vec4 ml = normalize(l);
    vec4 mn = normalize(vec4(texture(textureMap1, tc).rgb * 2.0 - 1.0, 0.0));
    vec4 mr = reflect(-ml, mn);

    vec4 kd = texture(textureMap0, tc); 
    vec4 ks = vec4(1.0);

    float nl = clamp(dot(mn, ml), 0.0, 1.0);
    nl = mix(0.4, 1.0, pow(nl, 2.0)); // softened light, as in earlier camera shader

    float rv = pow(clamp(dot(mr, mv), 0.0, 1.0), 25.0);

    float dist = length(v.xyz);
    float att = 1.0 / (0.2 + 0.15 * dist + 0.05 * dist * dist);

    float ao = texture(textureMap3, tc).r;

    vec3 ambient = vec3(0.08);
    vec3 lightColor = vec3(0.7, 0.5, 0.2);

    vec3 emissive = texture(textureMap4, tc).rgb;

    vec3 lighting = (nl * lightColor + ambient + ks.rgb * rv) * att;
    vec3 finalColor = kd.rgb * lighting;

    pixelColor = vec4(finalColor, 1.0);
}
