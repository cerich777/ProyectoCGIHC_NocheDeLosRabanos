#include "Escenario.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
namespace {
constexpr float PI=3.14159265f;
const glm::vec3 metal(.085f,.12f,.11f),oro(.72f,.46f,.16f),crema(.91f,.86f,.71f),madera(.8f,.72f,.65f);
glm::mat4 colocar(glm::vec3 p,float giro=0){return glm::rotate(glm::translate(glm::mat4(1),p),glm::radians(giro),glm::vec3(0,1,0));}
}
void Escenario::pieza(int forma,glm::vec3 p,glm::vec3 s,glm::vec3 color,int tex,const glm::mat4& padre,float giro,float emision,glm::vec2 uv) {
    piezas.push_back({forma,tex,glm::scale(padre*colocar(p,giro),s),color,uv,emision});
}
void Escenario::barra(glm::vec3 a,glm::vec3 b,float radio,glm::vec3 color,const glm::mat4& padre,int tex) {
    glm::vec3 d=b-a;float largo=glm::length(d);if(largo<.001f)return;
    auto m=glm::translate(padre,(a+b)*.5f);
    auto eje=glm::cross(glm::vec3(0,1,0),d/largo);float coseno=glm::clamp(d.y/largo,-1.0f,1.0f);
    if(glm::length(eje)>.0001f)m=glm::rotate(m,std::acos(coseno),glm::normalize(eje));
    else if(coseno<0)m=glm::rotate(m,PI,glm::vec3(1,0,0));
    piezas.push_back({CILINDRO,tex,glm::scale(m,glm::vec3(radio,largo,radio)),color,glm::vec2(1),0});
}
void Escenario::obstaculo(glm::vec3 p,glm::vec2 mitad,const glm::mat4& padre) {
    auto q=glm::vec3(padre*glm::vec4(p,1));
    glm::vec2 size(std::abs(padre[0][0])*mitad.x+std::abs(padre[2][0])*mitad.y,std::abs(padre[0][2])*mitad.x+std::abs(padre[2][2])*mitad.y);
    colisiones.obstaculos.push_back({{q.x,q.z},size});
}
void Escenario::kiosco() {
    pieza(OCTAGONO,{0,.18f,0},{4.65f,.36f,4.65f},crema,PIEDRA);
    pieza(OCTAGONO,{0,.58f,0},{4.35f,.80f,4.35f},crema,PIEDRA);
    pieza(OCTAGONO,{0,1.02f,0},{4.43f,.12f,4.43f},crema,PIEDRA);
    // Entrada abierta hacia el acceso; escalera transitable.
    for(int i=0;i<5;i++)pieza(CUBO,{0,.1f+i*.1f,6.1f-i*.42f},{2.4f,.2f+i*.2f,.62f},crema,PIEDRA);
    for(int i=0;i<8;i++) {
        float a=i*PI/4+PI/8;glm::vec3 p(std::cos(a)*3.85f,0,std::sin(a)*3.85f);
        pieza(CILINDRO,p+glm::vec3(0,2.8f,0),{.10f,3.5f,.10f},metal);
        pieza(CILINDRO,p+glm::vec3(0,1.23f,0),{.20f,.28f,.20f},metal);
        pieza(CILINDRO,p+glm::vec3(0,4.38f,0),{.20f,.16f,.20f},oro);
        obstaculo(p,{.15f,.15f});
        auto siguiente=glm::vec3(std::cos(a+PI/4)*3.85f,0,std::sin(a+PI/4)*3.85f);
        barra(p+glm::vec3(0,4.5f,0),siguiente+glm::vec3(0,4.5f,0),.07f,metal);
        // Adorno curvo hecho con segmentos de arco bajo cada claro.
        for(int j=0;j<10;j++) {
            float t=float(j)/10,u=float(j+1)/10;
            auto q=p*(1-t)+siguiente*t+glm::vec3(0,3.62f+.55f*std::sin(PI*t),0);
            auto r=p*(1-u)+siguiente*u+glm::vec3(0,3.62f+.55f*std::sin(PI*u),0);barra(q,r,.035f,oro);
        }
        if((p.z+siguiente.z)*.5f>3.2f)continue;
        barra(p+glm::vec3(0,2.05f,0),siguiente+glm::vec3(0,2.05f,0),.04f,metal);
        for(int j=1;j<8;j++){float t=float(j)/8;auto q=p*(1-t)+siguiente*t;barra(q+glm::vec3(0,1.1f,0),q+glm::vec3(0,2.05f,0),.022f,metal);}
        // Segmentos del pretil; el claro frontal queda libre para subir.
        for(int j=0;j<5;j++){float t=(j+.5f)/5;auto q=p*(1-t)+siguiente*t;obstaculo(q,{.28f,.28f});}
    }
    pieza(OCTAGONO,{0,4.64f,0},{4.8f,.20f,4.8f},metal);
    pieza(TECHO,{0,5.45f,0},{5.05f,1.52f,5.05f},{1,1,1},COBRE);
    pieza(OCTAGONO,{0,6.23f,0},{.64f,.27f,.64f},metal);
    pieza(TECHO,{0,6.55f,0},{.88f,.46f,.88f},{1,1,1},COBRE);
    barra({0,6.7f,0},{0,7.3f,0},.045f,oro);pieza(ESFERA,{0,7.35f,0},{.12f,.12f,.12f},oro);
    pieza(ESFERA,{0,4.13f,0},{.28f,.32f,.28f},{1,.75f,.36f},LISO,glm::mat4(1),0,1.0f);
    luces.push_back({{0,3.75f,0},{1,.73f,.36f},2.8f});
}
void Escenario::puesto(Render&,int i,glm::vec3 p,float angulo,int cartel,int modelo) {
    auto padre=colocar(p,angulo);
    const glm::vec3 colores[]={{.57f,.10f,.21f},{.16f,.38f,.32f},{.63f,.28f,.08f},{.52f,.38f,.16f},{.48f,.15f,.36f},{.20f,.29f,.44f}};
    auto c=colores[i];
    pieza(CUBO,{0,.07f,0},{6.2f,.14f,4.0f},madera,MADERA,padre,0,0,{3,2});
    for(float x:{-2.85f,2.85f}) for(float z:{-1.65f,1.65f}) {
        pieza(CUBO,{x,1.77f,z},{.14f,3.55f,.14f},madera,MADERA,padre);
        obstaculo({x,0,z},{.12f,.12f},padre);
    }
    pieza(CUBO,{0,1.35f,-1.7f},{5.65f,2.5f,.12f},madera,MADERA,padre,0,0,{4,1});
    pieza(CUBO,{0,.62f,.2f},{4.8f,1.05f,1.65f},c,TELA,padre,0,0,{3,1});
    pieza(CUBO,{0,1.19f,.18f},{5.0f,.12f,1.8f},madera,MADERA,padre,0,0,{3,1});
    obstaculo({0,0,.18f},{2.5f,.9f},padre);
    obstaculo({0,0,-1.7f},{2.9f,.12f},padre);
    pieza(CUBO,{0,1.27f,.18f},{2.3f,.12f,1.5f},crema,PIEDRA,padre);
    if(modelo>=0)pieza(modelo,{0,1.34f,.18f},{.95f,.95f,.95f},{1,1,1},LISO,padre);
    // Lona a dos aguas, franjas y faldon.
    for(int k=0;k<12;k++) {
        float x=-2.91f+k*.53f;auto color=k%2?crema:c;
        pieza(CUBO,{x,3.65f,-.92f},{.53f,.09f,2.12f},color,TELA,padre,0,0,{1,2});
        pieza(CUBO,{x,3.65f,1.02f},{.53f,.09f,2.12f},color,TELA,padre,0,0,{1,2});
        pieza(CUBO,{x,3.4f,2.03f},{.53f,.42f,.09f},color,TELA,padre);
    }
    pieza(CUBO,{0,2.98f,1.83f},{4.6f,.86f,.05f},{1,1,1},cartel,padre);
    for(float x:{-2.05f,2.05f}) {
        pieza(ESFERA,{x,2.6f,1.15f},{.11f,.14f,.11f},{1,.77f,.40f},LISO,padre,0,1.0f);
        barra({x,3.5f,1.15f},{x,2.75f,1.15f},.018f,metal,padre);
    }
    auto light=glm::vec3(padre*glm::vec4(0,2.55f,1.0f,1));luces.push_back({light,{1,.70f,.34f},3.2f});
    auto target=glm::vec3(padre*glm::vec4(0,1.8f,1.9f,1));
    const std::wstring titulos[]={L"Danza de rábanos",L"La vida en el mercado",L"El jaguar de la sierra",L"Memoria del maíz",L"Jardín de Oaxaca",L"El taller del artesano"};
    const std::wstring categorias[]={L"01 / CATEGORÍA LIBRE",L"02 / CATEGORÍA TRADICIONAL",L"03 / CATEGORÍA LIBRE",L"04 / TOTOMOXTLE",L"05 / FLOR INMORTAL",L"06 / PLAZA Y TALLER"};
    const std::wstring textos[]={
        L"Tres figuras de rábano forman una pequeña danza. El contraste entre piel rojiza y cortes claros permite sugerir rostros, prendas y gestos.\n\nEsta composición original interpreta el encuentro y la convivencia alrededor de las exhibiciones.",
        L"Una escena cotidiana recuerda el vínculo entre productos del campo, intercambio y trabajo artesanal. El mercado forma parte del contexto histórico que presenta la propuesta del equipo.\n\nLas figuras y hortalizas son una interpretación propia, no una copia de una obra ganadora.",
        L"La silueta de un jaguar se construye con volúmenes de rábano, incisiones claras y hojas. La pieza propone explorar cómo un material vegetal puede transformarse en una figura reconocible.\n\nDiseño original inspirado en fauna regional; no representa una leyenda específica ni atribuye significados a una comunidad.",
        L"El totomoxtle es la hoja seca que envuelve la mazorca de maíz. Sus pliegues, tonos y superficies delgadas ofrecen posibilidades distintas a las del rábano.\n\nEsta figura original usa capas y piezas dobladas para sugerir un vestido elaborado con hojas de maíz.",
        L"La flor inmortal seca permite crear arreglos y composiciones de color. Aquí se repiten pequeñas flores alrededor de una base para formar una pieza de exhibición.\n\nLa obra distingue pétalos, centros y tallos; es un arreglo original para este recorrido.",
        L"Mesa de demostración con un artesano en pose fija, piezas de rábano, tabla y herramienta. El montaje permite observar materiales y reconocer las etapas de una elaboración.\n\nEl tallado y los movimientos de manos se incorporarán cuando se trabajen animaciones. En esta versión no hay secuencias animadas."
    };
    const std::wstring tecnicas[]={L"Rábano / volúmenes tallados y hojas",L"Rábano / composición de figuras y objetos",L"Rábano / modelado orgánico e incisiones",L"Totomoxtle / superficies plegadas",L"Flor inmortal / repetición de geometría",L"Demostración estática / jerarquía de piezas"};
    puntos.push_back({target,titulos[i],categorias[i],textos[i],tecnicas[i],i<5?i:-1});
}
void Escenario::persona(glm::vec3 p,float giro,glm::vec3 ropa,bool artesano) {
    auto padre=colocar(p,giro);glm::vec3 piel(.70f,.43f,.26f);
    pieza(ESFERA,{0,1.54f,0},{.16f,.21f,.16f},piel,LISO,padre);
    pieza(ESFERA,{0,1.68f,.025f},{.17f,.11f,.16f},{.12f,.07f,.045f},LISO,padre);
    pieza(CUBO,{0,1.13f,0},{.38f,.55f,.25f},ropa,TELA,padre);
    for(float x:{-.11f,.11f}) {
        barra({x,.85f,0},{x,.15f,.02f},.077f,{.12f,.17f,.19f},padre);
        pieza(ESFERA,{x,.10f,.08f},{.095f,.065f,.17f},{.12f,.075f,.04f},LISO,padre);
        float side=x<0?-1.0f:1.0f;
        glm::vec3 hombro(side*.23f,1.33f,0),codo(side*.28f,1.02f,artesano?.23f:0),mano(side*.20f,artesano?1.1f:.85f,artesano?.49f:.05f);
        barra(hombro,codo,.065f,ropa,padre);barra(codo,mano,.05f,piel,padre);pieza(ESFERA,mano,{.055f,.075f,.055f},piel,LISO,padre);
    }
    if(artesano){pieza(CUBO,{0,1.04f,.15f},{.34f,.52f,.035f},crema,TELA,padre);pieza(CILINDRO,{0,1.76f,0},{.30f,.035f,.30f},crema,TELA,padre);}
    obstaculo(p,{.25f,.25f});
}
void Escenario::arbol(glm::vec3 p,float s) {
    pieza(CILINDRO,p+glm::vec3(0,1.9f*s,0),{.29f*s,3.8f*s,.29f*s},{.55f,.48f,.40f},MADERA);
    for(int i=0;i<5;i++){float a=i*2.4f;glm::vec3 q=p+glm::vec3(std::cos(a)*1.0f,4.3f+float(i%2)*.4f,std::sin(a)*1.0f)*s;barra(p+glm::vec3(0,2.7f*s,0),q,.12f*s,{.4f,.33f,.25f},glm::mat4(1),MADERA);pieza(ESFERA,q,{1.65f*s,1.45f*s,1.65f*s},{.64f,.86f,.56f},HOJAS);}
    obstaculo(p,{.30f*s,.30f*s});
}
void Escenario::farol(glm::vec3 p) {
    pieza(CILINDRO,p+glm::vec3(0,.16f,0),{.3f,.32f,.3f},metal);
    pieza(CILINDRO,p+glm::vec3(0,1.85f,0),{.065f,3.7f,.065f},metal);
    pieza(CUBO,p+glm::vec3(0,3.85f,0),{.48f,.65f,.48f},metal);
    pieza(ESFERA,p+glm::vec3(0,3.85f,.02f),{.20f,.27f,.20f},{1,.78f,.40f},LISO,glm::mat4(1),0,1.4f);
    pieza(CONO,p+glm::vec3(0,4.28f,0),{.40f,.28f,.40f},metal);
    luces.push_back({p+glm::vec3(0,3.8f,0),{1,.74f,.43f},1.65f});obstaculo(p,{.24f,.24f});
}

Escenario::Escenario(Render& render) {
    pieza(CUBO,{0,-.12f,0},{58,.24f,53},{1,1,1},ADOQUIN,glm::mat4(1),0,0,{38,34});
    pieza(CUBO,{0,-.01f,0},{12,.06f,44},{.91f,.87f,.77f},PIEDRA,glm::mat4(1),0,0,{10,40});
    pieza(CUBO,{0,-.005f,0},{53,.065f,7},{.91f,.87f,.77f},PIEDRA,glm::mat4(1),0,0,{40,8});
    // Franja ornamental del pavimento, repetida sin cargar modelos externos.
    for(int i=-12;i<=12;i++)for(float z:{-13.5f,13.5f})pieza(CUBO,{i*2.0f,.035f,z},{.6f,.035f,.6f},{.54f,.28f,.21f},PIEDRA,glm::mat4(1),45);
    kiosco();
    for(float x:{-9.0f,9.0f})for(float z:{-7.5f,7.5f}) {
        pieza(CUBO,{x,.25f,z},{7,.5f,5},crema,PIEDRA);
        pieza(CUBO,{x,.54f,z},{6.7f,.10f,4.7f},{.19f,.27f,.13f},LISO);
        pieza(CUBO,{x,.81f,z},{6.55f,.52f,4.55f},{.64f,.9f,.60f},HOJAS,glm::mat4(1),0,0,{4,3});
        arbol({x,0,z},1.0f);obstaculo({x,0,z},{3.5f,2.5f});
    }
    for(float x:{-6.2f,6.2f})for(float z:{-12.5f,12.5f}) {
        auto padre=colocar({x,0,z});
        pieza(CUBO,{0,.55f,0},{2.4f,.12f,.65f},madera,MADERA,padre);
        pieza(CUBO,{0,.98f,-.27f},{2.4f,.65f,.10f},madera,MADERA,padre);
        for(float xx:{-.9f,.9f})pieza(CUBO,{xx,.28f,0},{.11f,.55f,.60f},metal,LISO,padre);
        obstaculo({x,0,z},{1.2f,.4f});
    }
    std::vector<int> carteles;for(int i=0;i<8;i++)carteles.push_back(render.textura("images/Proyecto/cartel_"+std::to_string(i)+".png"));
    std::vector<int> modelos;for(auto n:{"danza","mercado","jaguar","totomoxtle","flor"})modelos.push_back(render.importar(std::string("Models/Proyecto/")+n+".obj"));
    puesto(render,0,{-21,0,8},90,carteles[0],modelos[0]);
    puesto(render,1,{21,0,8},-90,carteles[1],modelos[1]);
    puesto(render,2,{-21,0,-5},90,carteles[2],modelos[2]);
    puesto(render,3,{-13,0,-18},0,carteles[3],modelos[3]);
    puesto(render,4,{13,0,-18},0,carteles[4],modelos[4]);
    puesto(render,5,{21,0,-5},-90,carteles[5],-1);
    // Taller en pose fija: mesa, materiales y herramienta; sin actualizacion temporal.
    auto taller=colocar({21,0,-5},-90);
    pieza(ESFERA,{-.5f,1.49f,.2f},{.20f,.31f,.17f},{.65f,.025f,.13f},LISO,taller);
    pieza(ESFERA,{.65f,1.36f,.25f},{.36f,.04f,.27f},madera,MADERA,taller);
    barra({.45f,1.42f,.1f},{.9f,1.42f,.1f},.027f,{.64f,.69f,.71f},taller);
    pieza(CUBO,{0,.14f,-.85f},{.8f,.28f,.6f},madera,MADERA,taller);
    persona(glm::vec3(taller*glm::vec4(0,.28f,-.85f,1)),-90,{.66f,.35f,.18f},true);
    for(auto p:std::vector<glm::vec3>{{-15,0,13},{15,0,13},{-15,0,-11},{15,0,-11},{-5,0,-17},{5,0,-17},{-5,0,17},{5,0,17}})farol(p);
    // Portales y fachadas perimetrales: volumetria estilizada del entorno urbano.
    for(int i=-4;i<=4;i++)for(float z:{-26.5f,27.5f}) {
        if(z>0 && std::abs(i)<1)continue;float x=i*6.2f;float h=6.6f+float((i+5)%3)*.8f;
        glm::vec3 tono=(i%2)?glm::vec3(.94f,.73f,.49f):glm::vec3(.76f,.66f,.52f);
        pieza(CUBO,{x,h*.5f,z},{6.1f,h,4},tono,MURO,glm::mat4(1),0,0,{3,3});
        pieza(CUBO,{x,h+.08f,z},{6.2f,.25f,4.2f},crema,PIEDRA);
        float frente=z<0?z+2.02f:z-2.02f;
        for(float dx:{-1.7f,1.7f}) {
            pieza(CUBO,{x+dx,4.5f,frente},{1.15f,1.65f,.06f},{.10f,.16f,.17f});
            pieza(CUBO,{x+dx,4.5f,frente+(z<0?.04f:-.04f)},{.065f,1.65f,.08f},crema);
            pieza(CUBO,{x+dx,4.5f,frente+(z<0?.04f:-.04f)},{1.15f,.065f,.08f},crema);
            pieza(CUBO,{x+dx,1.5f,frente},{1.8f,2.8f,.07f},{.13f,.10f,.08f});
            for(float ox:{-1.02f,1.02f})pieza(CUBO,{x+dx+ox,1.5f,frente+(z<0?.10f:-.10f)},{.20f,3.0f,.3f},crema,PIEDRA);
            pieza(ESFERA,{x+dx,2.85f,frente},{1.08f,.62f,.17f},crema,PIEDRA);
            pieza(CUBO,{x+dx,2.67f,frente+(z<0?.18f:-.18f)},{1.65f,.55f,.08f},{.16f,.10f,.07f});
        }
    }
    for(float x:{-31.5f,31.5f}) {
        pieza(CUBO,{x,3.3f,0},{5.0f,6.6f,48},{.75f,.64f,.48f},MURO,glm::mat4(1),0,0,{4,12});
        for(int z=-20;z<=20;z+=5)pieza(CUBO,{x+(x<0?2.52f:-2.52f),3.8f,float(z)},{.05f,1.65f,1.4f},{.11f,.17f,.18f});
    }
    // Acceso/puente y barandales. Se puede atravesar el vano central.
    pieza(CUBO,{0,.035f,23.6f},{5.1f,.09f,4.5f},madera,MADERA,glm::mat4(1),0,0,{2,8});
    for(float x:{-2.5f,2.5f}) {
        for(float z=21.8f;z<=25.6f;z+=.55f)barra({x,0,z},{x,1.0f,z},.035f,metal);
        barra({x,1.1f,21.7f},{x,1.1f,25.7f},.05f,metal);
        pieza(CUBO,{x,2.2f,21.6f},{.24f,4.4f,.24f},madera,MADERA);obstaculo({x,0,21.6f},{.13f,.13f});
    }
    pieza(CUBO,{0,4.1f,21.6f},{5.8f,1.0f,.12f},{1,1,1},carteles[6],glm::mat4(1),180);
    // Banderines de papel picado: perforaciones construidas con tiras de geometria.
    const glm::vec3 papel[]={{.74f,.15f,.36f},{.94f,.59f,.12f},{.18f,.53f,.43f},{.55f,.25f,.64f}};
    for(float z:{-12.5f,14.5f}) {
        barra({-26,5.7f,z},{26,5.7f,z},.014f,metal);
        for(int i=0;i<40;i++) {
            float x=-25+i*1.28f,y=5.46f-.45f*std::sin((i/39.0f)*PI);auto c=papel[i%4];
            pieza(CUBO,{x,y,z},{.94f,.10f,.015f},c);
            for(int j=0;j<4;j++)pieza(CUBO,{x-.39f+j*.26f,y-.28f,z},{.13f,.5f,.015f},c);
            pieza(CUBO,{x,y-.53f,z},{.94f,.09f,.015f},c);
        }
    }
    for(int i=0;i<12;i++) {
        float x=(i%2?-1.0f:1.0f)*(12.9f+float(i%3)*.6f),z=14.6f-float(i)*2.55f;
        persona({x,0,z},i%2?90.0f:-90.0f,papel[i%4]);
    }
    persona({1.8f,1.0f,-1.3f},120,{.42f,.25f,.18f});
    puntos.push_back({{0,1.7f,20.0f},L"Bienvenidos al Zócalo",L"ACCESO / HISTORIA",
        L"Explora una interpretación del Zócalo de Oaxaca durante la Noche de los Rábanos, celebrada el 23 de diciembre.\n\nLa propuesta relaciona su origen con el mercado de la Vigilia de Navidad y menciona el primer concurso formal en 1897. Hoy reúne obras de rábano, totomoxtle y flor inmortal.\n\nEl recorrido es libre: visita los cinco montajes, el taller y el kiosco central.",L"Contexto basado en la propuesta del equipo",-1});
    puntos.push_back({{0,2.5f,3.0f},L"El kiosco y la plaza",L"PLAZA / PUNTO DE ENCUENTRO",
        L"El kiosco central organiza el recorrido. Sus columnas, cubierta y barandales crean una referencia reconocible desde los pasillos de exhibición.\n\nEsta versión usa arquitectura simplificada, puestos modulares y obras propias. La escala está adaptada para caminar y observar las piezas. No es un levantamiento exacto del Zócalo.\n\nPuedes subir por el acceso frontal y observar el entorno desde el interior.",L"Modelado geométrico / columnas y cubierta octagonal",-1});
}
