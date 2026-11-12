#include "Render.h"
#include "Shader.h"
#include <SOIL2/SOIL2.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>

namespace {
constexpr float PI = 3.14159265359f;
void triangulo(Geometria& m, VerticeEscena a, VerticeEscena b, VerticeEscena c) {
    if (glm::dot(glm::cross(b.posicion-a.posicion,c.posicion-a.posicion),a.normal+b.normal+c.normal)<0) std::swap(b,c);
    m.vertices.insert(m.vertices.end(), {a,b,c});
}
VerticeEscena vert(glm::vec3 p, glm::vec3 n, glm::vec2 uv) { return {p,n,uv,glm::vec3(1),0}; }
Geometria cubo() {
    Geometria m;
    const glm::vec3 normales[]={{1,0,0},{-1,0,0},{0,1,0},{0,-1,0},{0,0,1},{0,0,-1}};
    for (auto n: normales) {
        glm::vec3 u=std::abs(n.y)>0.5f?glm::vec3(1,0,0):glm::normalize(glm::cross(glm::vec3(0,1,0),n));
        glm::vec3 v=glm::cross(n,u);
        auto a=vert((n-u-v)*0.5f,n,{0,0}); auto b=vert((n+u-v)*0.5f,n,{1,0});
        auto c=vert((n+u+v)*0.5f,n,{1,1}); auto d=vert((n-u+v)*0.5f,n,{0,1});
        triangulo(m,a,b,c); triangulo(m,a,c,d);
    }
    return m;
}
Geometria esfera() {
    Geometria m; const int segmentos=20, anillos=12;
    auto punto=[&](int x,int y) {
        float u=float(x)/segmentos,v=float(y)/anillos;
        glm::vec3 p(std::sin(v*PI)*std::cos(u*2*PI), std::cos(v*PI),std::sin(v*PI)*std::sin(u*2*PI));
        return vert(p,p,{u,1-v});
    };
    for(int y=0;y<anillos;y++) for(int x=0;x<segmentos;x++) {
        triangulo(m,punto(x,y),punto(x+1,y),punto(x+1,y+1));
        triangulo(m,punto(x,y),punto(x+1,y+1),punto(x,y+1));
    }
    return m;
}
Geometria cilindro(int segmentos,bool cono) {
    Geometria m;
    for(int i=0;i<segmentos;i++) {
        float a=float(i)/segmentos*PI*2,b=float(i+1)/segmentos*PI*2;
        glm::vec3 n1(std::cos(a),cono?1.0f:0.0f,std::sin(a)),n2(std::cos(b),cono?1.0f:0.0f,std::sin(b));
        n1=glm::normalize(n1); n2=glm::normalize(n2);
        glm::vec3 p1(std::cos(a),-.5f,std::sin(a)),p2(std::cos(b),-.5f,std::sin(b));
        glm::vec3 p3(cono?0:std::cos(b),.5f,cono?0:std::sin(b));
        glm::vec3 p4(cono?0:std::cos(a),.5f,cono?0:std::sin(a));
        auto v1=vert(p1,n1,{float(i)/segmentos,0}),v2=vert(p2,n2,{float(i+1)/segmentos,0});
        auto v3=vert(p3,n2,{float(i+1)/segmentos,1}),v4=vert(p4,n1,{float(i)/segmentos,1});
        triangulo(m,v1,v2,v3); if(!cono) triangulo(m,v1,v3,v4);
        triangulo(m,vert({0,-.5f,0},{0,-1,0},{.5f,.5f}),vert(p2,{0,-1,0},{(p2.x+1)/2,(p2.z+1)/2}),vert(p1,{0,-1,0},{(p1.x+1)/2,(p1.z+1)/2}));
        if(!cono) triangulo(m,vert({0,.5f,0},{0,1,0},{.5f,.5f}),vert(p4,{0,1,0},{(p4.x+1)/2,(p4.z+1)/2}),vert(p3,{0,1,0},{(p3.x+1)/2,(p3.z+1)/2}));
    }
    return m;
}
void matriz(GLuint p,const char* n,const glm::mat4& m){ glUniformMatrix4fv(glGetUniformLocation(p,n),1,GL_FALSE,glm::value_ptr(m)); }
void vector3(GLuint p,const std::string& n,const glm::vec3& v){ glUniform3fv(glGetUniformLocation(p,n.c_str()),1,glm::value_ptr(v)); }
}

Render::Render() {
    programa=Shader("Shader/recorrido.vert","Shader/recorrido.frag").Program;
    profundidadPrograma=Shader("Shader/sombra.vert","Shader/sombra.frag").Program;
    cieloPrograma=Shader("Shader/pantalla.vert","Shader/cielo.frag").Program;
    mallas={cubo(),esfera(),cilindro(24,false),cilindro(24,true),cilindro(8,false),cilindro(8,true),cubo()};
    unsigned char blanco[4]={255,255,255,255}; GLuint t=0;
    glGenTextures(1,&t); glBindTexture(GL_TEXTURE_2D,t);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,1,1,0,GL_RGBA,GL_UNSIGNED_BYTE,blanco);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    texturas.push_back(t);archivosTextura.push_back("");
    for(auto archivo: {"piedra.png","madera.png","cobre.png","hojas.png","tela.png","muro.png","adoquin.png"}) textura(std::string("images/Proyecto/")+archivo);
    glGenVertexArrays(1,&quad);
    matrizLuz=glm::ortho(-48.0f,48.0f,-48.0f,48.0f,1.0f,130.0f)*glm::lookAt(glm::vec3(-35,60,30),glm::vec3(0),glm::vec3(0,1,0));
    glGenFramebuffers(1,&sombraFbo); glGenTextures(1,&sombra); glBindTexture(GL_TEXTURE_2D,sombra);
    glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT24,2048,2048,0,GL_DEPTH_COMPONENT,GL_FLOAT,nullptr);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST); glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_BORDER); glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_BORDER);
    float borde[]={1,1,1,1}; glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,borde);
    glBindFramebuffer(GL_FRAMEBUFFER,sombraFbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,sombra,0);
    glDrawBuffer(GL_NONE); glReadBuffer(GL_NONE);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE) throw std::runtime_error("No se pudo crear el mapa de sombras");
    glBindFramebuffer(GL_FRAMEBUFFER,0);
}
Render::~Render() {
    for(auto& g:grupos){glDeleteBuffers(1,&g.vbo);glDeleteVertexArrays(1,&g.vao);}
    if(!texturas.empty()) glDeleteTextures(static_cast<GLsizei>(texturas.size()),texturas.data());
    glDeleteTextures(1,&sombra); glDeleteFramebuffers(1,&sombraFbo); glDeleteVertexArrays(1,&quad);
    glDeleteProgram(programa);glDeleteProgram(profundidadPrograma);glDeleteProgram(cieloPrograma);
}
int Render::textura(const std::string& archivo) {
    int w=0,h=0; unsigned char* datos=SOIL_load_image(archivo.c_str(),&w,&h,nullptr,SOIL_LOAD_RGBA);
    if(!datos) throw std::runtime_error("Falta textura: "+archivo);
    for(int y=0;y<h/2;y++) for(int x=0;x<w*4;x++) std::swap(datos[y*w*4+x],datos[(h-1-y)*w*4+x]);
    GLuint t=0;glGenTextures(1,&t);glBindTexture(GL_TEXTURE_2D,t);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,datos);SOIL_free_image_data(datos);glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    if(GLEW_EXT_texture_filter_anisotropic) glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,4.0f);
    texturas.push_back(t);archivosTextura.push_back(archivo);return static_cast<int>(texturas.size())-1;
}
int Render::importar(const std::string& archivo) {
    Assimp::Importer lector;
    const aiScene* escena=lector.ReadFile(archivo,aiProcess_Triangulate|aiProcess_GenSmoothNormals|aiProcess_PreTransformVertices|aiProcess_JoinIdenticalVertices);
    if(!escena || !escena->HasMeshes()) throw std::runtime_error("No se pudo importar "+archivo+": "+lector.GetErrorString());
    Geometria geometria;
    for(unsigned k=0;k<escena->mNumMeshes;k++) {
        const auto* mesh=escena->mMeshes[k]; aiColor3D color(1,1,1);
        escena->mMaterials[mesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE,color);
        for(unsigned f=0;f<mesh->mNumFaces;f++) for(unsigned j=0;j<mesh->mFaces[f].mNumIndices;j++) {
            unsigned v=mesh->mFaces[f].mIndices[j]; const auto& p=mesh->mVertices[v]; const auto& n=mesh->mNormals[v];
            glm::vec2 uv(0); if(mesh->HasTextureCoords(0)) uv={mesh->mTextureCoords[0][v].x,mesh->mTextureCoords[0][v].y};
            geometria.vertices.push_back({{p.x,p.y,p.z},{n.x,n.y,n.z},uv,{color.r,color.g,color.b},0});
        }
    }
    if(geometria.vertices.empty()) throw std::runtime_error("Malla vacia: "+archivo);
    mallas.push_back(std::move(geometria));return static_cast<int>(mallas.size())-1;
}
void Render::preparar(const std::vector<Pieza>& piezas) {
    std::map<int,std::vector<VerticeEscena>> lotes;
    for(const auto& p:piezas) {
        const auto normal=glm::transpose(glm::inverse(glm::mat3(p.matriz)));
        auto& lote=lotes[p.textura];
        for(auto v:mallas.at(p.malla).vertices) {
            v.posicion=glm::vec3(p.matriz*glm::vec4(v.posicion,1));v.normal=glm::normalize(normal*v.normal);
            v.color*=p.color;v.uv*=p.repeticion;v.emision=p.emision;lote.push_back(v);
        }
    }
    for(auto& par:lotes) {
        Grupo g;g.textura=par.first;g.cantidad=static_cast<int>(par.second.size());numeroTriangulos+=g.cantidad/3;
        glGenVertexArrays(1,&g.vao);glGenBuffers(1,&g.vbo);glBindVertexArray(g.vao);glBindBuffer(GL_ARRAY_BUFFER,g.vbo);
        glBufferData(GL_ARRAY_BUFFER,par.second.size()*sizeof(VerticeEscena),par.second.data(),GL_STATIC_DRAW);
        const int tamanos[]={3,3,2,3,1};
        const size_t offsets[]={offsetof(VerticeEscena,posicion),offsetof(VerticeEscena,normal),offsetof(VerticeEscena,uv),offsetof(VerticeEscena,color),offsetof(VerticeEscena,emision)};
        for(int i=0;i<5;i++){glEnableVertexAttribArray(i);glVertexAttribPointer(i,tamanos[i],GL_FLOAT,GL_FALSE,sizeof(VerticeEscena),reinterpret_cast<void*>(offsets[i]));}
        grupos.push_back(g);
    }
    glEnable(GL_DEPTH_TEST);glDisable(GL_CULL_FACE);
    glBindFramebuffer(GL_FRAMEBUFFER,sombraFbo);glViewport(0,0,2048,2048);glClear(GL_DEPTH_BUFFER_BIT);
    glUseProgram(profundidadPrograma);matriz(profundidadPrograma,"matrizLuz",matrizLuz);
    for(const auto& g:grupos){glBindVertexArray(g.vao);glDrawArrays(GL_TRIANGLES,0,g.cantidad);}
    glBindFramebuffer(GL_FRAMEBUFFER,0);
}
void Render::dibujar(const glm::mat4& vista,const glm::mat4& proyeccion,const glm::vec3& ojo,
                     const std::vector<LuzPuntual>& luces,bool noche,bool lamparas,int ancho,int alto) {
    glViewport(0,0,ancho,alto);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);glUseProgram(cieloPrograma);glUniform1i(glGetUniformLocation(cieloPrograma,"noche"),noche);
    glBindVertexArray(quad);glDrawArrays(GL_TRIANGLES,0,3);glEnable(GL_DEPTH_TEST);
    glUseProgram(programa);matriz(programa,"vista",vista);matriz(programa,"proyeccion",proyeccion);matriz(programa,"matrizLuz",matrizLuz);vector3(programa,"ojo",ojo);
    glUniform1i(glGetUniformLocation(programa,"noche"),noche);
    int count=lamparas?std::min(static_cast<int>(luces.size()),24):0;
    glUniform1i(glGetUniformLocation(programa,"cantidadLuces"),count);
    for(int i=0;i<count;i++) {
        std::string n="luces["+std::to_string(i)+"].";
        vector3(programa,n+"posicion",luces[i].posicion);vector3(programa,n+"color",luces[i].color);
        glUniform1f(glGetUniformLocation(programa,(n+"intensidad").c_str()),luces[i].intensidad*(noche?1.0f:0.2f));
    }
    glActiveTexture(GL_TEXTURE1);glBindTexture(GL_TEXTURE_2D,sombra);glUniform1i(glGetUniformLocation(programa,"mapaSombra"),1);
    glActiveTexture(GL_TEXTURE0);glUniform1i(glGetUniformLocation(programa,"albedo"),0);
    for(const auto& g:grupos){glBindTexture(GL_TEXTURE_2D,texturas.at(g.textura));glBindVertexArray(g.vao);glDrawArrays(GL_TRIANGLES,0,g.cantidad);}
}
void Render::captura(const std::string& ruta,int ancho,int alto) const {
    std::filesystem::path path(ruta);if(path.has_parent_path()) std::filesystem::create_directories(path.parent_path());
    const uint32_t stride=(ancho*3+3)&~3,size=stride*alto;
    std::vector<unsigned char> pixels(size);glPixelStorei(GL_PACK_ALIGNMENT,4);glReadBuffer(GL_BACK);
    glReadPixels(0,0,ancho,alto,GL_BGR,GL_UNSIGNED_BYTE,pixels.data());
    std::ofstream out(path,std::ios::binary);if(!out)throw std::runtime_error("No se pudo guardar captura");
    auto u16=[&](uint16_t n){out.write(reinterpret_cast<const char*>(&n),2);};auto u32=[&](uint32_t n){out.write(reinterpret_cast<const char*>(&n),4);};
    u16(0x4d42);u32(54+size);u16(0);u16(0);u32(54);u32(40);u32(ancho);u32(alto);u16(1);u16(24);u32(0);u32(size);u32(2835);u32(2835);u32(0);u32(0);
    out.write(reinterpret_cast<const char*>(pixels.data()),pixels.size());
}
void Render::exportarEscenario(const std::vector<Pieza>& piezas,const std::string& ruta) const {
    // Exportacion opcional para editar la misma arquitectura en Blender.
    auto path=std::filesystem::path(ruta);std::filesystem::create_directories(path.parent_path());
    auto materialPath=path;materialPath.replace_extension(".mtl");
    std::ofstream obj(path),mtl(materialPath);if(!obj||!mtl)throw std::runtime_error("No se pudo exportar el escenario");
    obj<<"# Escenario original estatico. Eje Y arriba.\nmtllib "<<materialPath.filename().string()<<'\n';
    std::map<std::string,int> materiales;size_t indice=1,piezaId=0;
    for(const auto& p:piezas) {
        obj<<"o Pieza_"<<piezaId++<<'\n';const auto& vertices=mallas.at(p.malla).vertices;
        auto normal=glm::transpose(glm::inverse(glm::mat3(p.matriz)));int ultimo=-1;
        for(size_t j=0;j+2<vertices.size();j+=3) {
            auto c=vertices[j].color*p.color;std::ostringstream key;key<<p.textura<<'_'<<c.r<<'_'<<c.g<<'_'<<c.b<<'_'<<p.emision;
            auto entrada=materiales.find(key.str());int material;
            if(entrada==materiales.end()) {
                material=static_cast<int>(materiales.size());materiales[key.str()]=material;
                mtl<<"newmtl Material_"<<material<<"\nKd "<<c.r<<' '<<c.g<<' '<<c.b<<"\nKs 0.1 0.1 0.1\nNs 32\n";
                if(p.emision>0)mtl<<"Ke "<<c.r<<' '<<c.g<<' '<<c.b<<'\n';
                if(p.textura>0) {
                    std::filesystem::path origen(archivosTextura.at(p.textura));auto destino=path.parent_path()/origen.filename();
                    std::filesystem::copy_file(origen,destino,std::filesystem::copy_options::overwrite_existing);
                    mtl<<"map_Kd "<<origen.filename().string()<<'\n';
                }
                mtl<<'\n';
            }else material=entrada->second;
            if(material!=ultimo){obj<<"usemtl Material_"<<material<<'\n';ultimo=material;}
            for(size_t k=0;k<3;k++) {
                const auto& v=vertices[j+k];auto posicion=glm::vec3(p.matriz*glm::vec4(v.posicion,1));auto n=glm::normalize(normal*v.normal);auto uv=v.uv*p.repeticion;
                obj<<"v "<<posicion.x<<' '<<posicion.y<<' '<<posicion.z<<"\nvt "<<uv.x<<' '<<uv.y<<"\nvn "<<n.x<<' '<<n.y<<' '<<n.z<<'\n';
            }
            obj<<"f "<<indice<<'/'<<indice<<'/'<<indice<<' '<<indice+1<<'/'<<indice+1<<'/'<<indice+1<<' '<<indice+2<<'/'<<indice+2<<'/'<<indice+2<<'\n';indice+=3;
        }
    }
    std::cout<<"Escenario exportado para Blender: "<<path.string()<<'\n';
}
