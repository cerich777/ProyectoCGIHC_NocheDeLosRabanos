#version 330 core
layout(location=0) in vec3 posicion;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 uv;
layout(location=3) in vec3 color;
layout(location=4) in float emision;
uniform mat4 vista;
uniform mat4 proyeccion;
uniform mat4 matrizLuz;
out vec3 mundo;
out vec3 N;
out vec2 UV;
out vec3 tinte;
out float brillo;
out vec4 coordLuz;
void main() {
    mundo=posicion; N=normal; UV=uv; tinte=color; brillo=emision;
    coordLuz=matrizLuz*vec4(posicion,1);
    gl_Position=proyeccion*vista*vec4(posicion,1);
}
