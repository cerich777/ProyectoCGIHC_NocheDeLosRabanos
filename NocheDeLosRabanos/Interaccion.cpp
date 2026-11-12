#include "Interaccion.h"
#include <algorithm>
void Interaccion::actualizar(const std::vector<PuntoInteres>& puntos,glm::vec3 posicion,glm::vec3 frente) {
    cercano=-1;float mejor=5.2f;
    for(size_t i=0;i<puntos.size();i++) {
        auto delta=puntos[i].posicion-posicion;float distancia=glm::length(delta);
        if(distancia<mejor && (distancia<1.5f || glm::dot(glm::normalize(delta),frente)>.2f)) {mejor=distancia;cercano=static_cast<int>(i);}
    }
}
void Interaccion::activar(const std::vector<PuntoInteres>& puntos) {
    if(abierto>=0){abierto=-1;return;}
    abierto=cercano;
    if(abierto>=0 && puntos[abierto].obra>=0)visitadas.at(puntos[abierto].obra)=true;
}
int Interaccion::vistas() const{return static_cast<int>(std::count(visitadas.begin(),visitadas.end(),true));}
