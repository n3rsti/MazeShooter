#version 330

uniform sampler2D textureMap0; // diffuse
uniform sampler2D textureMap1; // normal map
uniform sampler2D textureMap2; // height map

uniform vec3 cameraPos;        // player position in world space

in vec4 ic;
in vec4 n;
in vec4 l;
in vec4 v;
in vec2 iTexCoord0;
in vec3 fragWorldPos;          // interpolated world position

out vec4 pixelColor;

vec2 parallaxTexCoords(vec4 viewDir, vec2 texCoords, float heightScale, float numLayers) {
    vec2 P = viewDir.xy / numLayers;
    float currentLayerDepth = 0.0;
    float layerDepth = 1.0 / numLayers;
    vec2 currentTexCoords = texCoords;
    float currentDepthMapValue = texture(textureMap2, currentTexCoords).r * heightScale;

    if (viewDir.z <= 0) discard;

    // Simple parallax loop (optional, can be commented if too heavy)
    // while (currentLayerDepth < currentDepthMapValue) {
    //     currentTexCoords -= P;
    //     if (currentTexCoords.x < 0.0 || currentTexCoords.x > 1.0 || currentTexCoords.y < 0.0 || currentTexCoords.y > 1.0) discard;
    //     currentDepthMapValue = texture(textureMap2, currentTexCoords).r * heightScale;
    //     currentLayerDepth += layerDepth;
    // }

    return currentTexCoords;
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
    float rv = pow(clamp(dot(mr, mv), 0.0, 1.0), 25.0);

    // Distance attenuation for light radius around player
    float dist = length(cameraPos - fragWorldPos);
    float radius = 5.0; // Light radius in world units
    float attenuation = clamp(1.0 - dist / radius, 0.0, 1.0);

    pixelColor = vec4(kd.rgb * nl * attenuation, kd.a) + vec4(ks.rgb * rv * attenuation, 0.0);
}
