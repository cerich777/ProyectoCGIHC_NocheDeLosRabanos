#pragma once
#include "Escenario.h"
#include <array>
class Interaccion {
public:
    int cercano=-1,abierto=-1;
    std::array<bool,5> visitadas{};
    void actualizar(const std::vector<PuntoInteres>& puntos,glm::vec3 posicion,glm::vec3 frente);
    void activar(const std::vector<PuntoInteres>& puntos);
    int vistas() const;
};
