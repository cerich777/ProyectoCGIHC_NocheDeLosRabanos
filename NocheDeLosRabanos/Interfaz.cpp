#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "Interfaz.h"
#include "Shader.h"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <stdexcept>

namespace {
constexpr int W=1280,H=720;
const COLORREF fondo=RGB(19,29,36),vino=RGB(91,35,54),dorado=RGB(224,183,112),blanco=RGB(248,240,224),gris=RGB(183,194,193);
void rect(HDC dc,int x,int y,int w,int h,COLORREF c) {RECT r{x,y,x+w,y+h};auto b=CreateSolidBrush(c);FillRect(dc,&r,b);DeleteObject(b);}
void ellipse(HDC dc,int x,int y,int w,int h,COLORREF c) {auto b=CreateSolidBrush(c);auto old=SelectObject(dc,b);auto pen=SelectObject(dc,GetStockObject(NULL_PEN));Ellipse(dc,x,y,x+w,y+h);SelectObject(dc,pen);SelectObject(dc,old);DeleteObject(b);}
void line(HDC dc,int x,int y,int xx,int yy,COLORREF c,int width=1) {auto p=CreatePen(PS_SOLID,width,c);auto old=SelectObject(dc,p);MoveToEx(dc,x,y,nullptr);LineTo(dc,xx,yy);SelectObject(dc,old);DeleteObject(p);}
void text(HDC dc,int x,int y,int width,int height,const std::wstring& value,int size,COLORREF c=blanco,bool serif=false,bool bold=false) {
    auto font=CreateFontW(-size,0,0,0,bold?FW_BOLD:FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,DEFAULT_PITCH,serif?L"Georgia":L"Segoe UI");
    auto old=SelectObject(dc,font);SetTextColor(dc,c);SetBkMode(dc,TRANSPARENT);RECT r{x,y,x+width,y+height};
    DrawTextW(dc,value.c_str(),static_cast<int>(value.size()),&r,DT_LEFT|DT_TOP|DT_WORDBREAK|DT_NOPREFIX);SelectObject(dc,old);DeleteObject(font);
}
void panel(HDC dc,int x,int y,int w,int h) {rect(dc,x,y,w,h,fondo);rect(dc,x,y,w,3,dorado);}
void mapa(HDC dc,const Escenario& escena,const Interaccion& inter,glm::vec3 pos,glm::vec3 frente,int x,int y,int w,int h,bool grande) {
    panel(dc,x,y,w,h);text(dc,x+16,y+12,w-30,25,grande?L"EL RECORRIDO":L"ZÓCALO / MAPA",grande?23:15,dorado,false,true);
    int top=y+(grande?62:40),left=x+15,mw=w-30,mh=h-(grande?160:55);
    auto point=[&](glm::vec3 p){return POINT{left+static_cast<LONG>((p.x+29)/58*mw),top+static_cast<LONG>((p.z+24)/51*mh)};};
    rect(dc,left,top,mw,mh,RGB(47,61,62));
    auto a=point({-6,0,-23}),b=point({6,0,23});rect(dc,a.x,a.y,b.x-a.x,b.y-a.y,RGB(108,105,90));
    a=point({-27,0,-3});b=point({27,0,3});rect(dc,a.x,a.y,b.x-a.x,b.y-a.y,RGB(108,105,90));
    for(float xx:{-9.0f,9.0f})for(float zz:{-7.5f,7.5f}){a=point({xx-3.5f,0,zz-2.5f});b=point({xx+3.5f,0,zz+2.5f});rect(dc,a.x,a.y,b.x-a.x,b.y-a.y,RGB(67,100,67));}
    a=point({0,0,0});ellipse(dc,a.x-mw*4/58,a.y-mh*4/51,mw*8/58,mh*8/51,RGB(183,162,109));
    for(size_t i=0;i<6;i++) {
        a=point(escena.puntos[i].posicion);bool visitado=i<5&&inter.visitadas[i];
        ellipse(dc,a.x-5,a.y-5,10,10,visitado?RGB(133,195,153):dorado);
        if(grande)text(dc,a.x+10,a.y-11,180,24,std::to_wstring(i+1),18,blanco,false,true);
    }
    a=point(pos);ellipse(dc,a.x-5,a.y-5,10,10,RGB(255,245,223));
    auto f=point(pos+frente*3.0f);line(dc,a.x,a.y,f.x,f.y,RGB(255,245,223),2);
    if(grande)text(dc,x+20,y+h-77,w-35,68,L"1 Danza · 2 Mercado · 3 Jaguar\n4 Totomoxtle · 5 Flor · 6 Taller\nM para cerrar · Verde: obra consultada.",15,gris);
}
}
struct Interfaz::Datos {
    HDC dc=nullptr;HBITMAP bitmap=nullptr;HGDIOBJ anterior=nullptr;unsigned int* pixels=nullptr;
    GLuint textura=0,programa=0,vao=0;
};
Interfaz::Interfaz():datos(std::make_unique<Datos>()) {
    datos->dc=CreateCompatibleDC(nullptr);BITMAPINFO info{};info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);info.bmiHeader.biWidth=W;info.bmiHeader.biHeight=-H;info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=32;info.bmiHeader.biCompression=BI_RGB;
    datos->bitmap=CreateDIBSection(datos->dc,&info,DIB_RGB_COLORS,reinterpret_cast<void**>(&datos->pixels),nullptr,0);
    if(!datos->bitmap)throw std::runtime_error("No se pudo crear la interfaz");
    datos->anterior=SelectObject(datos->dc,datos->bitmap);
    datos->programa=Shader("Shader/pantalla.vert","Shader/interfaz.frag").Program;
    glGenTextures(1,&datos->textura);glBindTexture(GL_TEXTURE_2D,datos->textura);glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,W,H,0,GL_BGRA,GL_UNSIGNED_BYTE,nullptr);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glGenVertexArrays(1,&datos->vao);
}
Interfaz::~Interfaz(){
    glDeleteProgram(datos->programa);glDeleteTextures(1,&datos->textura);glDeleteVertexArrays(1,&datos->vao);
    SelectObject(datos->dc,datos->anterior);DeleteObject(datos->bitmap);DeleteDC(datos->dc);
}
void Interfaz::dibujar(const Escenario& escena,const Interaccion& inter,const EstadoVista& estado,glm::vec3 pos,glm::vec3 frente,int ancho,int alto) {
    auto dc=datos->dc;rect(dc,0,0,W,H,RGB(255,0,255));
    panel(dc,28,24,505,97);
    ellipse(dc,46,57,32,36,RGB(186,55,87));line(dc,63,58,57,43,RGB(137,176,110),4);line(dc,63,59,72,43,RGB(137,176,110),4);
    text(dc,94,38,420,35,L"Noche de los Rábanos",28,blanco,true);
    text(dc,95,80,420,24,L"OAXACA DE JUÁREZ  /  23 DE DICIEMBRE",13,dorado,false,true);
    panel(dc,28,136,250,57);text(dc,44,147,220,24,std::to_wstring(inter.vistas())+L" / 5 obras descubiertas",19,blanco,false,true);
    rect(dc,44,176,216,3,RGB(66,75,77));rect(dc,44,176,216*inter.vistas()/5,3,dorado);
    if(inter.abierto<0&&!estado.ayuda)mapa(dc,escena,inter,pos,frente,1028,24,224,220,false);
    panel(dc,28,648,1224,50);
    text(dc,44,657,1200,24,L"WASD caminar   Shift paso rápido   Ratón mirar   E ficha   M mapa   H ayuda   N día/noche   L luces   O vista general   R inicio   P captura",15,blanco);
    text(dc,44,678,1200,20,L"Tab libera el cursor  ·  Esc cierra la ficha o sale del recorrido",12,gris);
    rect(dc,28,608,366,29,fondo);
    text(dc,38,614,350,24,estado.general?L"VISTA GENERAL  /  O para volver":(estado.noche?L"NOCHE  /  Iluminación de exhibición":L"DÍA  /  Luz general"),14,dorado,false,true);
    if(!estado.ayuda&&inter.abierto<0&&!estado.mapa&&!estado.general) {
        line(dc,634,360,646,360,blanco);line(dc,640,354,640,366,blanco);
        if(inter.cercano>=0) {
            panel(dc,375,548,530,53);text(dc,393,561,497,32,L"E  /  "+escena.puntos[inter.cercano].titulo,20,blanco,false,true);
        } else if(estado.cursor) {panel(dc,442,555,397,43);text(dc,461,565,370,28,L"Tab para volver a mirar con el ratón",17,blanco);}
    }
    if(estado.mapa&&!estado.ayuda&&inter.abierto<0)mapa(dc,escena,inter,pos,frente,350,145,580,466,true);
    if(inter.abierto>=0) {
        const auto& p=escena.puntos[inter.abierto];panel(dc,720,136,532,482);
        rect(dc,740,156,4,34,dorado);text(dc,756,157,470,29,p.categoria,15,dorado,false,true);
        text(dc,744,202,480,74,p.titulo,30,blanco,true);
        // Medir antes de dibujar para que las fichas mas largas no se recorten.
        int cuerpo=18;
        for(;cuerpo>15;cuerpo--) {
            auto font=CreateFontW(-cuerpo,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,DEFAULT_PITCH,L"Segoe UI");
            auto old=SelectObject(dc,font);RECT medida{0,0,480,0};
            DrawTextW(dc,p.descripcion.c_str(),static_cast<int>(p.descripcion.size()),&medida,DT_CALCRECT|DT_WORDBREAK|DT_NOPREFIX);
            SelectObject(dc,old);DeleteObject(font);if(medida.bottom<=229)break;
        }
        text(dc,744,279,480,229,p.descripcion,cuerpo,blanco);
        rect(dc,744,519,482,1,RGB(77,80,78));text(dc,744,531,480,39,p.tecnica,15,dorado);
        text(dc,744,575,480,30,L"Fuente: propuesta del equipo / SIC y Gobierno de Oaxaca.\nE o Esc para cerrar la ficha.",12,gris);
    }
    if(estado.ayuda) {
        panel(dc,315,146,650,460);
        text(dc,346,173,585,26,L"UN RECORRIDO POR LA TRADICIÓN",15,dorado,false,true);
        text(dc,345,216,590,80,L"La plaza se llena\nde historias y color.",34,blanco,true);
        text(dc,346,318,575,80,L"Camina entre los puestos, descubre cinco obras y visita el kiosco. Acércate a una pieza y pulsa E para conocer su material y contexto.",20,blanco);
        text(dc,346,419,575,78,L"WASD + ratón: explorar     E: consultar     M: mapa\nN: día / noche     L: luces     O: vista general\nTab: liberar cursor     R: regresar al acceso",17,gris);
        rect(dc,346,529,310,44,vino);text(dc,364,538,285,30,L"Enter / comenzar a explorar",19,blanco,false,true);
        text(dc,680,542,245,30,L"H muestra esta ayuda",15,gris);
    }
    GdiFlush();
    for(int i=0;i<W*H;i++) {
        unsigned int p=datos->pixels[i]&0x00ffffff;
        datos->pixels[i]=p==0x00ff00ff?0:(p|0xff000000);
    }
    glActiveTexture(GL_TEXTURE0);glBindTexture(GL_TEXTURE_2D,datos->textura);glTexSubImage2D(GL_TEXTURE_2D,0,0,0,W,H,GL_BGRA,GL_UNSIGNED_BYTE,datos->pixels);
    glViewport(0,0,ancho,alto);glDisable(GL_DEPTH_TEST);glEnable(GL_BLEND);glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(datos->programa);glUniform1i(glGetUniformLocation(datos->programa,"panel"),0);glBindVertexArray(datos->vao);glDrawArrays(GL_TRIANGLES,0,3);
    glDisable(GL_BLEND);glEnable(GL_DEPTH_TEST);
}
