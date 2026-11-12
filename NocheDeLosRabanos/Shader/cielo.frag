#version 330 core
in vec2 UV;
out vec4 fragmento;
uniform bool noche;
float hash(vec2 p){return fract(sin(dot(p,vec2(127.1,311.7)))*43758.5453);}
void main(){
    float altura=clamp(UV.y,0.0,1.0);
    vec3 c=noche?mix(vec3(0.12,0.14,0.23),vec3(0.013,0.024,0.07),altura):mix(vec3(0.78,0.84,0.86),vec3(0.20,0.48,0.73),altura);
    if(noche){
        vec2 celda=floor(UV*vec2(210,130));
        vec2 local=fract(UV*vec2(210,130));
        float estrella=step(0.993,hash(celda))*(1.0-smoothstep(0.0,0.13,length(local-0.5)));
        c+=vec3(0.65)*estrella*smoothstep(0.35,0.8,altura);
        vec2 luna=(UV-vec2(0.79,0.79))*vec2(1.7778,1.0);
        float d=length(luna);
        c+=vec3(0.60,0.63,0.57)*(1.0-smoothstep(0.024,0.026,d));
        c+=vec3(0.04,0.045,0.06)*exp(-d*25.0);
    }
    fragmento=vec4(c,1);
}
