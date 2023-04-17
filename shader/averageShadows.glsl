#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(binding = 0, rgba8ui) uniform uimage3D outputTexture;
layout(binding = 1, r32i) uniform iimage3D tmp;

layout (location = 0) uniform mat4 sunTransformation;
layout (location = 1) uniform vec3 size;
layout (location = 2) uniform int stepNo;


vec3 sunPosition = (vec4(0, 0, 1, 1) * sunTransformation).xyz;

void main()
{
    ivec3 texel = ivec3(gl_GlobalInvocationID.xyz);
    if (stepNo == 0) {
        uint finalShadow = imageLoad(outputTexture, texel).b +
            imageLoad(outputTexture, texel + ivec3(0, 0, 1)).b +
            imageLoad(outputTexture, texel + ivec3(0, 0, -1)).b +
            imageLoad(outputTexture, texel + ivec3(0, 1, 0)).b +
            imageLoad(outputTexture, texel + ivec3(0, -1, 0)).b +
            imageLoad(outputTexture, texel + ivec3(1, 0, 0)).b +
            imageLoad(outputTexture, texel + ivec3(-1, 0, 0)).b +
            imageLoad(outputTexture, texel + ivec3(0, 1, 1)).b +
            imageLoad(outputTexture, texel + ivec3(0, 1, -1)).b +
            imageLoad(outputTexture, texel + ivec3(0, -1, 1)).b +
            imageLoad(outputTexture, texel + ivec3(0, -1, -1)).b +
            imageLoad(outputTexture, texel + ivec3(1, 0, 1)).b +
            imageLoad(outputTexture, texel + ivec3(1, 0, -1)).b +
            imageLoad(outputTexture, texel + ivec3(-1, 0, 1)).b +
            imageLoad(outputTexture, texel + ivec3(-1, 0, -1)).b +
            imageLoad(outputTexture, texel + ivec3(1, 1, 0)).b +
            imageLoad(outputTexture, texel + ivec3(1, -1, 0)).b +
            imageLoad(outputTexture, texel + ivec3(-1, 1, 0)).b +
            imageLoad(outputTexture, texel + ivec3(-1, -1, 0)).b +
            imageLoad(outputTexture, texel + ivec3(1, 1, 1)).b +
            imageLoad(outputTexture, texel + ivec3(1, 1, -1)).b +
            imageLoad(outputTexture, texel + ivec3(1, -1, 1)).b +
            imageLoad(outputTexture, texel + ivec3(1, -1, -1)).b +
            imageLoad(outputTexture, texel + ivec3(-1, 1, 1)).b +
            imageLoad(outputTexture, texel + ivec3(-1, 1, -1)).b +
            imageLoad(outputTexture, texel + ivec3(-1, -1, 1)).b +
            imageLoad(outputTexture, texel + ivec3(-1, -1, -1)).b;
        finalShadow = finalShadow / 26;
        imageStore(tmp, texel, ivec4(10, 0, 0, 0));
    } else {
        uvec4 data = imageLoad(outputTexture, texel);
        imageStore(outputTexture, texel, uvec4(data.r, data.g, imageLoad(tmp, texel).r, data.a));
    }
}
