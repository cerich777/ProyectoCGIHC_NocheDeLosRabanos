#pragma once
#include <glm/glm.hpp>
#include <vector>
struct Obstaculo { glm::vec2 centro, mitad; };
class Colisiones {
public:
    std::vector<Obstaculo> obstaculos;
    bool libre(glm::vec3 posicion, float radio = 0.28f) const;
    glm::vec3 mover(glm::vec3 inicio, glm::vec3 desplazamiento) const;
    float altura(glm::vec3 posicion) const;
};
