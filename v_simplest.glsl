#version 330

// Uniforms
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec3 cameraPos;  // Player position in world space

// Attributes
in vec4 vertex;    // model space vertex position
in vec4 color;
in vec4 normal;
in vec2 texCoord0;
in vec4 c1;        // tangent
in vec4 c2;        // bitangent
in vec4 c3;        // normal (TBN basis vectors)

// Outputs to fragment shader
out vec4 ic;
out vec4 l;        // light direction (tangent space, from camera/player)
out vec4 l2;       // light direction (tangent space, from (0,10,0))
out vec4 n;        // normal (tangent space)
out vec4 v;        // view direction (tangent space)
out vec2 iTexCoord0;
out vec3 fragWorldPos;  // world position of the vertex

void main(void) {
    mat4 invTBN = mat4(c1, c2, c3, vec4(0, 0, 0, 1));

    vec4 worldPos = M * vertex;
    fragWorldPos = vec3(worldPos);

    // Light direction vector (world space) from camera/player
    vec4 lightDirWorld = vec4(cameraPos, 1.0) - worldPos;
    l = normalize(invTBN * lightDirWorld); // light dir in tangent space

    // Second light direction vector (world space) from (0,10,0)
    vec4 light2Pos = vec4(0.0, 10.0, 0.0, 1.0);
    vec4 light2DirWorld = light2Pos - worldPos;
    l2 = normalize(invTBN * light2DirWorld); // second light dir in tangent space

    // View direction vector (world space)
    vec4 viewPosWorld = inverse(V * M) * vec4(0, 0, 0, 1);
    v = normalize(invTBN * viewPosWorld - invTBN * vertex);

    n = vec4(0, 0, 1, 0); // normal vector in tangent space (Z axis)

    iTexCoord0 = texCoord0;
    ic = color;

    gl_Position = P * V * M * vertex;
}
