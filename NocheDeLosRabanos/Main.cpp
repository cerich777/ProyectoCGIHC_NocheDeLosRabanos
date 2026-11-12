#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Escenario.h"
#include "Interfaz.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <stdexcept>

namespace {
struct Aplicacion {
    GLFWwindow* ventana=nullptr;
    Escenario* escena=nullptr;
    Interaccion interaccion;
    EstadoVista estado;
    glm::vec3 posicion{0,1.7f,20.0f};
    float yaw=-90,pitch=-5,fov=55;
    double mouseX=0,mouseY=0;
    bool primerMouse=true,capturar=false;
    glm::vec3 frente() const {
        return glm::normalize(glm::vec3(std::cos(glm::radians(yaw))*std::cos(glm::radians(pitch)),std::sin(glm::radians(pitch)),std::sin(glm::radians(yaw))*std::cos(glm::radians(pitch))));
    }
    bool modal() const {return estado.ayuda||estado.mapa||interaccion.abierto>=0||estado.general;}
    void cursor(){glfwSetInputMode(ventana,GLFW_CURSOR,(estado.cursor||modal())?GLFW_CURSOR_NORMAL:GLFW_CURSOR_DISABLED);primerMouse=true;}
    void inicio(){posicion={0,1.7f,20};yaw=-90;pitch=-5;estado.general=false;interaccion.abierto=-1;estado.mapa=false;}
    void teclado(int key) {
        if(key==GLFW_KEY_ESCAPE) {
            if(interaccion.abierto>=0)interaccion.abierto=-1;
            else if(estado.ayuda)estado.ayuda=false;
            else if(estado.mapa)estado.mapa=false;
            else glfwSetWindowShouldClose(ventana,GLFW_TRUE);
        }
        if(key==GLFW_KEY_ENTER||key==GLFW_KEY_H)estado.ayuda=key==GLFW_KEY_H?!estado.ayuda:false;
        if(key==GLFW_KEY_TAB)estado.cursor=!estado.cursor;
        if(key==GLFW_KEY_M&&!estado.ayuda&&interaccion.abierto<0)estado.mapa=!estado.mapa;
        if(key==GLFW_KEY_E&&!estado.ayuda&&!estado.mapa&&!estado.general)interaccion.activar(escena->puntos);
        if(key==GLFW_KEY_N)estado.noche=!estado.noche;
        if(key==GLFW_KEY_L)estado.lamparas=!estado.lamparas;
        if(key==GLFW_KEY_O&&!estado.ayuda) {estado.general=!estado.general;interaccion.abierto=-1;estado.mapa=false;}
        if(key==GLFW_KEY_R)inicio();
        if(key==GLFW_KEY_P)capturar=true;
        cursor();
    }
    void avanzar(float dt) {
        if(modal())return;
        auto f=frente();f.y=0;f=glm::normalize(f);auto derecha=glm::normalize(glm::cross(f,glm::vec3(0,1,0)));
        glm::vec3 d(0);
        if(glfwGetKey(ventana,GLFW_KEY_W)==GLFW_PRESS)d+=f;
        if(glfwGetKey(ventana,GLFW_KEY_S)==GLFW_PRESS)d-=f;
        if(glfwGetKey(ventana,GLFW_KEY_D)==GLFW_PRESS)d+=derecha;
        if(glfwGetKey(ventana,GLFW_KEY_A)==GLFW_PRESS)d-=derecha;
        if(glm::length(d)>.001f)posicion=escena->colisiones.mover(posicion,glm::normalize(d)*dt*(glfwGetKey(ventana,GLFW_KEY_LEFT_SHIFT)==GLFW_PRESS?5.6f:3.0f));
        interaccion.actualizar(escena->puntos,posicion,frente());
    }
};
void require(bool condicion,const char* mensaje){if(!condicion)throw std::runtime_error(std::string("Prueba fallida: ")+mensaje);}
void probar(Escenario& escena,Render& render) {
    require(escena.puntos.size()==8,"cinco obras, taller, entrada y kiosco");
    require(escena.luces.size()>=12&&escena.luces.size()<=24,"luces de puestos y plaza");
    require(render.triangulos()>20000&&render.lotes()<30,"geometria y agrupacion de materiales");
    const auto& c=escena.colisiones;
    require(c.libre({0,1.7f,20}),"entrada libre");
    require(!c.libre({9,1.7f,7.5f}),"jardin bloqueado");
    auto pared=c.mover({0,1.7f,20},{100,0,0});require(pared.x<27.3f,"limite lateral sin atravesarlo a alta velocidad");
    auto borde=c.mover({0,1.7f,20},{0,0,100});require(borde.z<25.5f,"limite del puente");
    auto jardin=c.mover({9,1.7f,12},{0,0,-12});require(jardin.z>10.0f,"no atravesar jardin con paso grande");
    auto escaleras=c.mover({0,1.7f,7},{0,0,-7});require(escaleras.z<.3f&&escaleras.y>2.6f,"escalera y piso del kiosco");
    const glm::vec3 miradores[]={{-16,1.7f,8},{16,1.7f,8},{-16,1.7f,-5},{-13,1.7f,-12},{13,1.7f,-12},{16,1.7f,-5}};
    Interaccion inter;
    for(int i=0;i<6;i++) {
        require(c.libre(miradores[i]),"acceso libre a cada puesto");
        inter.actualizar(escena.puntos,miradores[i],glm::normalize(escena.puntos[i].posicion-miradores[i]));
        require(inter.cercano==i,"seleccion del puesto observado");inter.activar(escena.puntos);require(inter.abierto==i,"abrir ficha");inter.activar(escena.puntos);require(inter.abierto==-1,"cerrar ficha");
    }
    require(inter.vistas()==5,"progreso de cinco obras sin contar taller");
    inter.actualizar(escena.puntos,{0,1.7f,15},{0,0,-1});require(inter.cercano==-1,"no seleccionar obras lejanas");
    std::cout<<"SELF TEST OK: importacion de 5 obras, 8 fichas, colisiones, escalera, interaccion y progreso.\n";
}
struct Toma { glm::vec3 ojo,objetivo; const char* nombre; int ficha=-1;bool dia=false; };
}

int main(int argc,char** argv) {
    bool smoke=false;std::string capturas,exportacion;
    for(int i=1;i<argc;i++) {
        std::string arg=argv[i];if(arg=="--smoke-test"||arg=="--self-test")smoke=true;
        if(arg=="--capture-dir"&&i+1<argc){capturas=std::filesystem::absolute(argv[++i]).string();smoke=true;}
        if(arg=="--export-scene"&&i+1<argc){exportacion=std::filesystem::absolute(argv[++i]).string();smoke=true;}
    }
    wchar_t exe[32768]={};if(!GetModuleFileNameW(nullptr,exe,32768))return 1;
    std::filesystem::current_path(std::filesystem::path(exe).parent_path());
    glfwSetErrorCallback([](int code,const char* text){std::cerr<<"GLFW "<<code<<": "<<text<<'\n';});
    if(!glfwInit())return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES,4);if(smoke)glfwWindowHint(GLFW_VISIBLE,GLFW_FALSE);
    auto* window=glfwCreateWindow(1600,900,"Noche de los Rabanos | Zocalo de Oaxaca",nullptr,nullptr);
    if(!window){glfwTerminate();return 1;}
    glfwMakeContextCurrent(window);glewExperimental=GL_TRUE;
    if(glewInit()!=GLEW_OK){glfwDestroyWindow(window);glfwTerminate();return 1;}
    while(glGetError()!=GL_NO_ERROR){};
    glfwSwapInterval(smoke?0:1);int resultado=0;
    try {
        // Todos los recursos GPU se destruyen antes de cerrar el contexto.
        Render render;Escenario escena(render);render.preparar(escena.piezas);Interfaz interfaz;
        if(!exportacion.empty())render.exportarEscenario(escena.piezas,exportacion);
        Aplicacion app;app.ventana=window;app.escena=&escena;app.estado.ayuda=!smoke;
        glfwSetWindowUserPointer(window,&app);
        glfwSetKeyCallback(window,[](GLFWwindow* w,int key,int,int action,int){if(action==GLFW_PRESS)static_cast<Aplicacion*>(glfwGetWindowUserPointer(w))->teclado(key);});
        glfwSetCursorPosCallback(window,[](GLFWwindow* w,double x,double y){
            auto& a=*static_cast<Aplicacion*>(glfwGetWindowUserPointer(w));
            if(a.estado.cursor||a.modal()){a.primerMouse=true;return;}
            if(a.primerMouse){a.mouseX=x;a.mouseY=y;a.primerMouse=false;}
            a.yaw=std::fmod(a.yaw+float(x-a.mouseX)*.12f,360.0f);a.pitch=std::clamp(a.pitch+float(a.mouseY-y)*.12f,-85.0f,85.0f);a.mouseX=x;a.mouseY=y;
        });
        glfwSetScrollCallback(window,[](GLFWwindow* w,double,double y){auto& a=*static_cast<Aplicacion*>(glfwGetWindowUserPointer(w));if(!a.modal())a.fov=std::clamp(a.fov-float(y)*2,30.0f,70.0f);});
        glfwSetWindowFocusCallback(window,[](GLFWwindow* w,int focus){auto& a=*static_cast<Aplicacion*>(glfwGetWindowUserPointer(w));if(!focus)a.estado.cursor=true;a.cursor();});
        if(!smoke)app.cursor();
        std::cout<<"OpenGL "<<glGetString(GL_VERSION)<<"\nEscena: "<<escena.piezas.size()<<" piezas, "<<render.triangulos()<<" triangulos, "<<render.lotes()<<" lotes.\n";
        if(smoke) {
            probar(escena,render);
            app.estado.ayuda=true;app.teclado(GLFW_KEY_ENTER);require(!app.estado.ayuda,"cerrar bienvenida");
            app.interaccion.cercano=0;app.teclado(GLFW_KEY_E);require(app.interaccion.abierto==0,"tecla de ficha");
            app.teclado(GLFW_KEY_ESCAPE);require(app.interaccion.abierto==-1&&!glfwWindowShouldClose(window),"Escape cierra ficha sin salir");
            app.teclado(GLFW_KEY_M);require(app.estado.mapa,"tecla de mapa");app.teclado(GLFW_KEY_M);
            app.teclado(GLFW_KEY_N);require(!app.estado.noche,"tecla de dia");app.teclado(GLFW_KEY_N);
            app.teclado(GLFW_KEY_L);require(!app.estado.lamparas,"tecla de luces");app.teclado(GLFW_KEY_L);
            app.teclado(GLFW_KEY_O);require(app.estado.general,"tecla de vista general");app.teclado(GLFW_KEY_R);require(!app.estado.general,"volver al acceso");
            app.interaccion=Interaccion{};
        }
        const std::vector<Toma> tomas={
            {{0,1.9f,20},{0,2,0},"01_acceso"},
            {{38,32,40},{0,0,0},"02_plaza_noche"},
            {{-16,1.9f,8},{-21,2.0f,8},"03_danza"},
            {{16,1.9f,8},{21,2.0f,8},"04_mercado"},
            {{-16,1.9f,-5},{-21,2.0f,-5},"05_jaguar"},
            {{-13,1.9f,-12},{-13,2.0f,-18},"06_totomoxtle"},
            {{13,1.9f,-12},{13,2.0f,-18},"07_flor"},
            {{16,1.9f,-5},{21,2.0f,-5},"08_taller"},
            {{13,1.9f,-12},{13,2.0f,-18},"09_ficha",4},
            {{38,32,40},{0,0,0},"10_plaza_dia",-1,true},
            {{0,2.7f,2},{-6,2.5f,-8},"11_kiosco"},
            {{0,1.9f,20},{0,2,0},"12_ayuda"},
            {{0,1.9f,20},{0,2,0},"13_mapa"},
            {{0,1.9f,20},{0,2,0},"14_sin_luces"},
            {{0,1.9f,20},{0,2,0},"15_progreso"},
            {{0,1.9f,20},{0,2,0},"16_ficha_acceso",6},
            {{0,2.7f,2},{-6,2.5f,-8},"17_ficha_kiosco",7},
            {{-16,1.9f,8},{-21,2,8},"18_ficha_danza",0},
            {{16,1.9f,8},{21,2,8},"19_ficha_mercado",1},
            {{-16,1.9f,-5},{-21,2,-5},"20_ficha_jaguar",2},
            {{-13,1.9f,-12},{-13,2,-18},"21_ficha_totomoxtle",3},
            {{16,1.9f,-5},{21,2,-5},"22_ficha_taller",5}
        };
        double previo=glfwGetTime();size_t frame=0;
        while(!glfwWindowShouldClose(window)) {
            double ahora=glfwGetTime();float dt=std::min(float(ahora-previo),.05f);previo=ahora;
            glfwPollEvents();app.avanzar(dt);
            int w=0,h=0;glfwGetFramebufferSize(window,&w,&h);
            if(w<=0||h<=0){glfwWaitEventsTimeout(.05);continue;}
            glm::vec3 ojo=app.posicion,objetivo=ojo+app.frente();
            if(app.estado.general){ojo={38,32,40};objetivo={0,0,0};}
            if(smoke) {
                const auto& t=tomas[frame];ojo=t.ojo;objetivo=t.objetivo;app.estado.noche=!t.dia;app.interaccion.abierto=t.ficha;app.estado.ayuda=frame==11;app.estado.general=frame==1||frame==9;
                app.estado.mapa=frame==12;app.estado.lamparas=frame!=13;
                if(frame==14)app.interaccion.visitadas.fill(true);
                if(t.ficha>=0&&escena.puntos[t.ficha].obra>=0)app.interaccion.visitadas[escena.puntos[t.ficha].obra]=true;
                app.interaccion.actualizar(escena.puntos,ojo,glm::normalize(objetivo-ojo));
            }
            auto vista=glm::lookAt(ojo,objetivo,glm::vec3(0,1,0));auto proyeccion=glm::perspective(glm::radians(app.estado.general?58.0f:app.fov),float(w)/h,.08f,180.0f);
            render.dibujar(vista,proyeccion,ojo,escena.luces,app.estado.noche,app.estado.lamparas,w,h);
            interfaz.dibujar(escena,app.interaccion,app.estado,ojo,glm::normalize(objetivo-ojo),w,h);
            if(smoke) {
                require(glGetError()==GL_NO_ERROR,"dibujado OpenGL sin errores");
                if(!capturas.empty())render.captura((std::filesystem::path(capturas)/(std::string(tomas[frame].nombre)+".bmp")).string(),w,h);
            }
            if(app.capturar){
                auto id=std::chrono::system_clock::now().time_since_epoch().count();std::string path="capturas/recorrido_"+std::to_string(id)+".bmp";render.captura(path,w,h);std::cout<<"Captura: "<<path<<'\n';app.capturar=false;
            }
            glfwSwapBuffers(window);
            if(smoke&&++frame>=tomas.size())break;
        }
        if(smoke)std::cout<<"SMOKE TEST OK: "<<tomas.size()<<" vistas, sombras, noche/dia, interfaz y cinco OBJ con materiales.\n";
    } catch(const std::exception& e){std::cerr<<e.what()<<'\n';resultado=1;}
    glfwDestroyWindow(window);glfwTerminate();return resultado;
}
