#pragma once
#include "Render.h"
#include "Colisiones.h"
#include <string>
struct PuntoInteres {
    glm::vec3 posicion;
    std::wstring titulo, categoria, descripcion, tecnica;
    int obra = -1;
};
class Escenario {
public:
    explicit Escenario(Render& render);
    std::vector<Pieza> piezas;
    std::vector<LuzPuntual> luces;
    std::vector<PuntoInteres> puntos;
    Colisiones colisiones;
private:
    void pieza(int forma,glm::vec3 p,glm::vec3 s,glm::vec3 color,int textura=LISO,
               const glm::mat4& padre=glm::mat4(1),float giro=0,float emision=0,glm::vec2 uv=glm::vec2(1));
    void barra(glm::vec3 a,glm::vec3 b,float radio,glm::vec3 color,const glm::mat4& padre=glm::mat4(1),int textura=LISO);
    void obstaculo(glm::vec3 p,glm::vec2 mitad,const glm::mat4& padre=glm::mat4(1));
    void kiosco();
    void puesto(Render& render,int indice,glm::vec3 p,float angulo,int cartel,int modelo);
    void persona(glm::vec3 p,float giro,glm::vec3 ropa,bool artesano=false);
    void arbol(glm::vec3 p,float escala);
    void farol(glm::vec3 p);
};
