#version 330 core
in vec2 UV;
out vec4 fragmento;
uniform sampler2D panel;
void main(){fragmento=texture(panel,vec2(UV.x,1.0-UV.y));}
