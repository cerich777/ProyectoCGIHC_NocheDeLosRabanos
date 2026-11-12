#version 330 core
layout(location=0) in vec3 posicion;
uniform mat4 matrizLuz;
void main(){gl_Position=matrizLuz*vec4(posicion,1);}
