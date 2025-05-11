#version 330

//Uniform variables
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

//Attributes
in vec4 vertex; //Vertex coordinates in model space
out vec4 iC;
in vec4 color;

void main(void) {
    float d = distance(V*M*vertex,vec4(0,0,0,1));
    d=(d-3.3)/4;
    iC=color;
    gl_Position=P*V*M*vertex;
}
