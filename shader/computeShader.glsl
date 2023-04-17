#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(binding = 0, rgba8ui) uniform uimage3D outputTexture;
layout(binding = 1, r32i) uniform iimage3D sdfTexture;
layout(binding = 2) uniform colorPalette {
    uint data[256];
};

layout (location = 0) uniform mat4 sunTransformation;
layout (location = 1) uniform vec3 size;
layout (location = 2) uniform int sdf;


const float VOXEL_SIZE = 5;
const int MAX_RAY_STEPS = 400;
const int SDF_LIMIT = 7;

vec3 sunPosition = (vec4(0, 0, 1, 1) * sunTransformation).xyz;

uint raycastLignt(vec3 rayPos, vec3 rayDir)
{
    if (imageLoad(outputTexture, ivec3(rayPos + ivec3(0, 0, 1))).r == 0 &&
        imageLoad(outputTexture, ivec3(rayPos + ivec3(0, 0, -1))).r == 0 &&
        imageLoad(outputTexture, ivec3(rayPos + ivec3(0, 1, 0))).r == 0 &&
        imageLoad(outputTexture, ivec3(rayPos + ivec3(0, -1, 0))).r == 0 &&
        imageLoad(outputTexture, ivec3(rayPos + ivec3(1, 0, 0))).r == 0 &&
        imageLoad(outputTexture, ivec3(rayPos + ivec3(-1, 0, 0))).r == 0) {
        return 0;
    }
    int x = int(rayPos.x) - 1;
    int y = int(rayPos.y) - 1;
    int z = int(rayPos.z) - 1;
    int ok = 0;
    for (int i = x; i < x + 1 * 2; i++) {
        for (int j = y; j < y + 1 * 2; j++) {
            for (int k = z; k < z + 1 * 2; k++) {
                if (i < size.x && j < size.y && k < size.z && i >= 0 && j >= 0 && k >= 0) {
                    if (imageLoad(outputTexture, ivec3(i, j, k)).r == 0) {
                        ok = 1;
                        break;
                    }
                }
            }
        }
    }
    if (ok == 0) return 0;

    ivec3 mapPos = ivec3(rayPos);
    vec3 deltaDist = abs(vec3(1.0) / rayDir);
    ivec3 rayStep = ivec3(sign(rayDir));
    vec3 sideDist = (sign(rayDir) * (sign(rayDir) * 0.5) + 0.5) * deltaDist;
    int multiplicator = 1;
    float opacity = 1;
    int steps = MAX_RAY_STEPS;

    while (steps > 0) {
        if (mapPos.x >= 0 && mapPos.x < (size.x) &&
            mapPos.y >= 0 && mapPos.y < (size.y) &&
            mapPos.z >= 0 && mapPos.z < (size.z) && mapPos != ivec3(rayPos)) {
            uvec4 sdf = imageLoad(outputTexture, ivec3(mapPos));
            if (sdf.r == 0) {
                vec4 val = unpackUnorm4x8(data[sdf.g]);
                opacity *= (1 - (val.w));
                if (opacity <= 0.2) {
                    return uint(opacity * 255);
                }
                multiplicator = 1;
            } else {
                multiplicator = int(sdf.r);
            }
        }
        steps -= multiplicator;
        int u = 0;
        while (u < multiplicator) {
            if (sideDist.x < sideDist.y) {
                if (sideDist.x < sideDist.z) {
                    sideDist.x += deltaDist.x;
                    mapPos.x += rayStep.x;
                } else {
                    sideDist.z += deltaDist.z;
                    mapPos.z += rayStep.z ;
                }
            } else {
                if (sideDist.y < sideDist.z) {
                    sideDist.y += deltaDist.y;
                    mapPos.y += rayStep.y ;
                } else {
                    sideDist.z += deltaDist.z;
                    mapPos.z += rayStep.z ;
                }
            }
            u++;
        }
    }
    return uint(opacity * 255);
}

uint distance_c (ivec3 a, ivec3 b)
{
    return uint(max(max(abs(b.x - a.x), abs(b.y - a.y)), abs(b.z - a.z)));
    // return abs(a.x - b.x) + abs(a.y - b.y) + abs(a.z - b.z);
}

void main()
{
    ivec3 texel = ivec3(gl_GlobalInvocationID.xyz);
    uvec4 data = imageLoad(outputTexture, texel);
    uint result = 0;

    if (sdf == 1) {
        if (data.g == 0) return;
        int x = texel.x - SDF_LIMIT;
        int y = texel.y - SDF_LIMIT;
        int z = texel.z - SDF_LIMIT;
        for (int i = x; i < x + SDF_LIMIT * 2; i++) {
            for (int j = y; j < y + SDF_LIMIT * 2; j++) {
                for (int k = z; k < z + SDF_LIMIT * 2; k++) {
                    if (i < size.x && j < size.y && k < size.z && i >= 0 && j >= 0 && k >= 0) {
                        ivec3 pos = ivec3(i, j, k);
                        uint dist = distance_c(pos, texel);
                        uvec4 localData = imageLoad(outputTexture, pos);
                        if (imageAtomicMin(sdfTexture, pos, int(dist)) > dist) {
                            imageStore(outputTexture, pos, uvec4(dist, localData.g, localData.b, localData.a));
                            imageStore(sdfTexture, pos, ivec4(dist, 0, 0, 0));
                        }
                    }
                }
            }
        }
        imageStore(outputTexture, texel, uvec4(0, data.g, data.b, data.a));
    } else {
        
        vec3 posVoxel = vec3(texel);
        uint shadow = raycastLignt(vec3(posVoxel.x, posVoxel.y, posVoxel.z), sunPosition);
        //     + raycastLignt(vec3(posVoxel.x, posVoxel.y, posVoxel.z + 1), sunPosition)
        //     + raycastLignt(vec3(posVoxel.x, posVoxel.y, posVoxel.z - 1), sunPosition)
        //     + raycastLignt(vec3(posVoxel.x, posVoxel.y + 1, posVoxel.z), sunPosition)
        //     + raycastLignt(vec3(posVoxel.x, posVoxel.y - 1, posVoxel.z), sunPosition)
        //     + raycastLignt(vec3(posVoxel.x + 1, posVoxel.y, posVoxel.z), sunPosition)
        //     + raycastLignt(vec3(posVoxel.x - 1, posVoxel.y, posVoxel.z), sunPosition)

        //     + raycastLignt(vec3(posVoxel.x, posVoxel.y + 1, posVoxel.z + 1), sunPosition)
        //     + raycastLignt(vec3(posVoxel.x, posVoxel.y + 1, posVoxel.z - 1), sunPosition)
        //     + raycastLignt(vec3(posVoxel.x, posVoxel.y - 1, posVoxel.z + 1), sunPosition)
        //     + raycastLignt(vec3(posVoxel.x, posVoxel.y - 1, posVoxel.z - 1), sunPosition)
        //     + raycastLignt(vec3(posVoxel.x + 1, posVoxel.y, posVoxel.z + 1), sunPosition)
        //     + raycastLignt(vec3(posVoxel.x + 1, posVoxel.y, posVoxel.z - 1), sunPosition)
        //     + raycastLignt(vec3(posVoxel.x - 1, posVoxel.y, posVoxel.z + 1), sunPosition)
        //     + raycastLignt(vec3(posVoxel.x - 1, posVoxel.y, posVoxel.z - 1), sunPosition)
        //     + raycastLignt(vec3(posVoxel.x + 1, posVoxel.y + 1, posVoxel.z), sunPosition)
        //     + raycastLignt(vec3(posVoxel.x + 1, posVoxel.y - 1, posVoxel.z), sunPosition)
        //     + raycastLignt(vec3(posVoxel.x - 1, posVoxel.y + 1, posVoxel.z), sunPosition)
        //     + raycastLignt(vec3(posVoxel.x - 1, posVoxel.y - 1, posVoxel.z), sunPosition);

        // shadow /= 24;
        imageStore(outputTexture, texel, uvec4(data.r, data.g, shadow, data.a));
    }
}
