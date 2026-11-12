#include "Colisiones.h"
#include <algorithm>
#include <cmath>
bool Colisiones::libre(glm::vec3 p,float radio) const {
    if(std::abs(p.x)>27.3f-radio || p.z< -22.5f+radio || p.z>25.5f-radio)return false;
    if(p.z>21.8f && std::abs(p.x)>2.5f-radio)return false;
    for(const auto& o:obstaculos) {
        float x=std::max(std::abs(p.x-o.centro.x)-o.mitad.x,0.0f);
        float z=std::max(std::abs(p.z-o.centro.y)-o.mitad.y,0.0f);
        if(x*x+z*z<radio*radio)return false;
    }
    return true;
}
float Colisiones::altura(glm::vec3 p) const {
    float r=std::sqrt(p.x*p.x+p.z*p.z);
    if(r<4.16f)return 1.0f;
    if(std::abs(p.x)<1.25f && p.z>=4.0f && p.z<6.4f) return std::clamp((6.4f-p.z)/2.25f,0.0f,1.0f);
    return 0;
}
glm::vec3 Colisiones::mover(glm::vec3 p,glm::vec3 delta) const {
    delta.y=0;
    int pasos=std::max(1,static_cast<int>(std::ceil(glm::length(delta)/.09f)));
    delta/=float(pasos);
    for(int i=0;i<pasos;i++) for(int eje:{0,2}) {
        auto siguiente=p;siguiente[eje]+=delta[eje];
        if(libre(siguiente) && altura(siguiente)-altura(p)<.26f)p=siguiente;
    }
    p.y=1.7f+altura(p);return p;
}
