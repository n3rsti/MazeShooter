#version 330

uniform sampler2D textureMap0; // diffuse
uniform sampler2D textureMap1; // normal map
uniform sampler2D textureMap2; // height map

uniform vec3 cameraPos;        // player position in world space

in vec4 ic;
in vec4 n;
in vec4 l;        // from camera/player
in vec4 l2;       // from (0,10,0)
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
    vec4 ml2 = normalize(l2);
    vec4 mn = normalize(vec4(texture(textureMap1, tc).rgb * 2.0 - 1.0, 0.0));
    vec4 mr = reflect(-ml, mn);
    vec4 mr2 = reflect(-ml2, mn);

    vec4 kd = texture(textureMap0, tc);
    vec4 ks = vec4(1.0);

    // First light (camera/player)
    float nl = clamp(dot(mn, ml), 0.0, 1.0);
    float rv = pow(clamp(dot(mr, mv), 0.0, 1.0), 25.0);

    // Second light ((0,10,0))
    float nl2 = clamp(dot(mn, ml2), 0.0, 1.0);
    float rv2 = pow(clamp(dot(mr2, mv), 0.0, 1.0), 25.0);

    // Distance attenuation for each light
    float dist = length(cameraPos - fragWorldPos);
    float radius = 5.0; // Light radius in world units
    float attenuation = clamp(1.0 - dist / radius, 0.0, 1.0);

    float dist2 = length(vec3(0.0, 10.0, 0.0) - fragWorldPos);
    float radius2 = 10.0; // Light radius for second light, can tweak as needed
    float attenuation2 = clamp(1.0 - dist2 / radius2, 0.0, 1.0);

    // Sum both lights
    vec3 diffuse = kd.rgb * (nl * attenuation + nl2 * attenuation2);
    float spec = rv * attenuation + rv2 * attenuation2;

    pixelColor = vec4(diffuse, kd.a) + vec4(ks.rgb * spec, 0.0);
}
