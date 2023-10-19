#version 330 core

layout (location = 0) in vec4 vertex;
layout (location = 1) in vec3 color_i;

out vec2 UV;
out vec3 color;
out float alpha;

void main(){
    vec2 pos = vertex.xy - vec2(1024/2,-(768/2) + 50);
    pos /= vec2(1024/2,768/2);
    gl_Position =  vec4(pos,0,1);
    UV = vertex.zw;
    color = color_i;
}

