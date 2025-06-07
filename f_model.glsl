#version 330 core

uniform sampler2D textureMap0; // diffuse map
uniform sampler2D textureMap1; // normal map (packed in RGB)
uniform sampler2D textureMap2; // height map (for parallax)

in vec4 ic;
in vec3 lightDirTS;
in vec3 viewDirTS;
in vec2 iTexCoord0;

out vec4 pixelColor;

vec2 parallaxTexCoords(vec3 viewDir, vec2 texCoords, float heightScale, float numLayers) {
    // Approximate parallax mapping using linear search
    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;

    vec2 P = viewDir.xy * heightScale;
    vec2 deltaTexCoords = P / numLayers;

    vec2 currentTexCoords = texCoords;
    float currentDepthMapValue = texture(textureMap2, currentTexCoords).r;

    while (currentLayerDepth < currentDepthMapValue) {
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = texture(textureMap2, currentTexCoords).r;
        currentLayerDepth += layerDepth;

        // Optional: clamp texcoords inside [0,1]
        if (currentTexCoords.x < 0.0 || currentTexCoords.x > 1.0 || currentTexCoords.y < 0.0 || currentTexCoords.y > 1.0)
            break;
    }

    return currentTexCoords;
}

void main(void) {
    vec3 viewDir = normalize(viewDirTS);

    // Parallax offset the texture coordinates
    vec2 parallaxUV = parallaxTexCoords(viewDir, iTexCoord0, 0.1, 20.0);

    // Sample normal map and transform to [-1,1]
    vec3 normalSample = texture(textureMap1, parallaxUV).rgb;
    vec3 normalTS = normalize(normalSample * 2.0 - 1.0);

    vec3 lightDir = normalize(lightDirTS);
    vec3 reflectDir = reflect(-lightDir, normalTS);

    // Sample diffuse color
    vec4 kd = texture(textureMap0, parallaxUV);
    vec4 ks = vec4(1.0); // Specular color hardcoded to white

    // Lighting
    float NdotL = max(dot(normalTS, lightDir), 0.0);
    float RdotV = pow(max(dot(reflectDir, viewDir), 0.0), 25.0);

    vec4 diffuse = vec4(kd.rgb * NdotL, kd.a);
    vec4 specular = vec4(ks.rgb * RdotV, 0.0);

    pixelColor = kd;
}
