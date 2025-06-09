#version 330

//Uniform variables
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec3 cameraPos;

//Attributes
in vec4 vertex; //Vertex coordinates in model space
in vec4 normal; //Vertex normal in model space

in vec2 texCoord0;
out vec2 iTexCoord0;

//Varying variables
out vec4 l;
out vec4 l2;
out vec4 n;
out vec4 v;
out vec3 frag;

void main(void) {
    vec4 worldPos=M*vertex;
    frag = vec3(worldPos);
    
    iTexCoord0=texCoord0;
    vec4 lp = vec4(cameraPos, 1.0); //light position, world space
    l = normalize(V * lp - V * M * vertex); //vector towards the light in eye space
    
    vec4 lp2 = vec4(0,10,0,1);
    l2 = normalize(V * lp - V * M * vertex); //vector towards the light in eye space
    
    vec4 a = vec4(cameraPos,1);
    v = normalize(vec4(0, 0, 0, 1) - V * M * vertex); //vector towards the viewer in eye space
    n = normalize(V * M * normal); //normal vector in eye space

    gl_Position = P * V * M * vertex;
}
