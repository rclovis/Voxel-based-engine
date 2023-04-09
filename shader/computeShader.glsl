#version 430 core


layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(binding = 0, r32f) uniform image3D outputTexture;
layout(binding = 1, rgba32f) uniform image3D inputTexture;
layout(binding = 2, r32i) uniform iimage3D sdfTexture;


layout (location = 0) uniform mat4 sun_transformation;
layout (location = 1) uniform vec3 size;
layout (location = 2) uniform int sdf;

const int GRID_SIZE = 100;
const float VOXEL_SIZE = 5;
const int MAX_RAY_STEPS = 400;

vec3 sunPosition = (vec4(0, 0, 1, 1) * sun_transformation).xyz;


float raycastLignt(vec3 rayPos, vec3 rayDir)
{
    if (imageLoad(inputTexture, ivec3(rayPos)).w == 0) {
        return 1.0f;
    } else if (imageLoad(inputTexture, ivec3(rayPos + vec3(0, 0, 1))).w == 1 &&
        imageLoad(inputTexture, ivec3(rayPos + vec3(0, 0, -1))).w == 1 &&
        imageLoad(inputTexture, ivec3(rayPos + vec3(0, 1, 0))).w == 1 &&
        imageLoad(inputTexture, ivec3(rayPos + vec3(0, -1, 0))).w == 1 &&
        imageLoad(inputTexture, ivec3(rayPos + vec3(1, 0, 0))).w == 1 &&
        imageLoad(inputTexture, ivec3(rayPos + vec3(-1, 0, 0))).w == 1) {
        return 1.0f;
    }


    ivec3 mapPos = ivec3(floor(rayPos + 0.));
    vec3 deltaDist = abs(vec3(length(rayDir)) / rayDir);
    ivec3 rayStep = ivec3(sign(rayDir));
    vec3 sideDist = (sign(rayDir) * (vec3(mapPos) - rayPos) + (sign(rayDir) * 0.5) + 0.5) * deltaDist;
    float opacity = 1;

    for (int i = 0; i < MAX_RAY_STEPS; i++) {
        if (mapPos.x >= 0 && mapPos.x <= (size.x) &&
            mapPos.y >= 0 && mapPos.y <= (size.y) &&
            mapPos.z >= 0 && mapPos.z <= (size.z) && i != 0) {
            vec4 val = imageLoad(inputTexture, mapPos);
            val.w /= 6;
            opacity *= (1 - val.w);
            if (opacity <= 0.2) {
                return opacity;
            }
        }
        if (sideDist.x < sideDist.y) {
            if (sideDist.x < sideDist.z) {
                sideDist.x += deltaDist.x;
                mapPos.x += rayStep.x;
            } else {
                sideDist.z += deltaDist.z;
                mapPos.z += rayStep.z;
            }
        } else {
            if (sideDist.y < sideDist.z) {
                sideDist.y += deltaDist.y;
                mapPos.y += rayStep.y;
            } else {
                sideDist.z += deltaDist.z;
                mapPos.z += rayStep.z;
            }
        }
    }
    return opacity;
}

int distance_c (ivec3 a, ivec3 b)
{
    return max(max(abs(b.x - a.x), abs(b.y - a.y)), abs(b.z - a.z));
    // return abs(a.x - b.x) + abs(a.y - b.y) + abs(a.z - b.z);
}

void main()
{
    // ivec3 texSize = textureSize(inputTexture);
    ivec3 texel = ivec3(gl_GlobalInvocationID.xyz);

    float result = raycastLignt(texel, sunPosition);

    if (sdf == 1 && imageLoad(inputTexture, texel).w != 0) {
        int x = texel.x - 5;
        int y = texel.y - 5;
        int z = texel.z - 5;
        for (int i = x; i < x + 10; i++) {
            for (int j = y; j < y + 10; j++) {
                for (int k = z; k < z + 10; k++) {
                    if (i < size.x && j < size.y && k < size.z && i >= 0 && j >= 0 && k >= 0) {
                        ivec3 pos = ivec3(i, j, k);
                        int dist = distance_c(pos, texel);
                        if (imageLoad(sdfTexture, pos).r > dist) {
                            imageStore(sdfTexture, pos, ivec4(dist, 0, 0, 0));
                        }
                    }
                }
            }
        }
        imageStore(sdfTexture, texel, ivec4(0, 0, 0, 0));
    }
    imageStore(outputTexture, texel, vec4(result, 0.0f, 0.0f, 0.0f));
}
