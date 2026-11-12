#pragma once
#include <memory>
#include "Interaccion.h"
struct EstadoVista {
    bool noche=true,lamparas=true,ayuda=true,mapa=false,general=false,cursor=false;
};
class Interfaz {
public:
    Interfaz();
    ~Interfaz();
    void dibujar(const Escenario& escena,const Interaccion& interaccion,const EstadoVista& estado,
                 glm::vec3 posicion,glm::vec3 frente,int ancho,int alto);
private:
    struct Datos;
    std::unique_ptr<Datos> datos;
};
