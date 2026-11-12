#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

struct VerticeEscena {
    glm::vec3 posicion, normal;
    glm::vec2 uv;
    glm::vec3 color = glm::vec3(1);
    float emision = 0;
};
struct Geometria { std::vector<VerticeEscena> vertices; };
struct Pieza {
    int malla = 0, textura = 0;
    glm::mat4 matriz = glm::mat4(1);
    glm::vec3 color = glm::vec3(1);
    glm::vec2 repeticion = glm::vec2(1);
    float emision = 0;
};
struct LuzPuntual { glm::vec3 posicion, color; float intensidad = 1; };
enum Forma { CUBO, ESFERA, CILINDRO, CONO, OCTAGONO, TECHO, PLANO, FORMAS_BASE };
enum Textura { LISO, PIEDRA, MADERA, COBRE, HOJAS, TELA, MURO, ADOQUIN, TEXTURAS_BASE };

// Mallas y texturas propias: las piezas se agrupan en lotes estaticos al cargar.
class Render {
public:
    Render();
    ~Render();
    Render(const Render&) = delete;
    Render& operator=(const Render&) = delete;
    int textura(const std::string& archivo);
    int importar(const std::string& archivo);
    void preparar(const std::vector<Pieza>& piezas);
    void dibujar(const glm::mat4& vista, const glm::mat4& proyeccion,
                 const glm::vec3& ojo, const std::vector<LuzPuntual>& luces,
                 bool noche, bool lamparas, int ancho, int alto);
    size_t triangulos() const { return numeroTriangulos; }
    size_t lotes() const { return grupos.size(); }
    void captura(const std::string& ruta, int ancho, int alto) const;
    void exportarEscenario(const std::vector<Pieza>& piezas,const std::string& ruta) const;
private:
    struct Grupo { GLuint vao = 0, vbo = 0; int cantidad = 0, textura = 0; };
    std::vector<Geometria> mallas;
    std::vector<GLuint> texturas;
    std::vector<std::string> archivosTextura;
    std::vector<Grupo> grupos;
    GLuint programa = 0, profundidadPrograma = 0, cieloPrograma = 0, quad = 0;
    GLuint sombraFbo = 0, sombra = 0;
    glm::mat4 matrizLuz = glm::mat4(1);
    size_t numeroTriangulos = 0;
};
