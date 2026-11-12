#version 330 core
in vec3 mundo;
in vec3 N;
in vec2 UV;
in vec3 tinte;
in float brillo;
in vec4 coordLuz;
out vec4 fragmento;
uniform sampler2D albedo;
uniform sampler2D mapaSombra;
uniform vec3 ojo;
uniform bool noche;
uniform int cantidadLuces;
struct Luz { vec3 posicion; vec3 color; float intensidad; };
uniform Luz luces[24];
float sombraSolar(vec3 normal) {
    vec3 p=coordLuz.xyz/coordLuz.w*0.5+0.5;
    if(p.z>1.0 || p.x<0.0 || p.x>1.0 || p.y<0.0 || p.y>1.0) return 0.0;
    float bias=max(0.0018*(1.0-dot(normal,normalize(vec3(-25,45,20)))),0.0006);
    vec2 texel=1.0/vec2(textureSize(mapaSombra,0));
    float resultado=0.0;
    for(int x=-1;x<=1;x++) for(int y=-1;y<=1;y++)
        resultado+=p.z-bias>texture(mapaSombra,p.xy+vec2(x,y)*texel).r?1.0:0.0;
    return resultado/9.0;
}
void main() {
    vec3 base=pow(texture(albedo,UV).rgb,vec3(2.2))*tinte;
    vec3 normal=normalize(N)*(gl_FrontFacing?1.0:-1.0);
    vec3 vista=normalize(ojo-mundo);
    vec3 direccion=normalize(vec3(-25,45,20));
    vec3 ambiente=noche?vec3(0.025,0.035,0.065):vec3(0.34,0.39,0.44);
    vec3 solar=noche?vec3(0.095,0.13,0.22):vec3(1.05,0.95,0.81);
    vec3 luz=base*(ambiente+solar*max(dot(normal,direccion),0.0)*(1.0-sombraSolar(normal)*0.7));
    for(int i=0;i<cantidadLuces;i++) {
        vec3 delta=luces[i].posicion-mundo;
        float d=length(delta);
        vec3 L=delta/max(d,0.001);
        float atenuacion=luces[i].intensidad/(1.0+0.16*d+0.065*d*d);
        float difusa=max(dot(normal,L),0.0);
        float especular=pow(max(dot(normal,normalize(L+vista)),0.0),32.0)*0.14;
        luz+=(base*difusa+vec3(especular))*luces[i].color*atenuacion;
    }
    luz+=base*brillo*2.8;
    float niebla=1.0-exp(-pow(length(ojo-mundo)*0.0035,2.0));
    luz=mix(luz,noche?vec3(0.025,0.038,0.075):vec3(0.49,0.64,0.75),niebla);
    // Mapeo tonal suave y correccion gamma sobre el framebuffer convencional.
    luz=luz/(luz+vec3(0.75));
    fragmento=vec4(pow(luz,vec3(1.0/2.2)),1.0);
}
