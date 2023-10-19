#version 330 core

in vec2 UV;
in vec3 color;
out vec4 FragColor;
uniform sampler2D myTextureSampler;

void main(){
    FragColor = vec4(color.r / 255, color.g / 255, color.b / 255, texture(myTextureSampler, UV).r);
}