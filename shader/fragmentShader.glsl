#version 430 core

in vec3 cameraPosition;
in mat4 cameraDirection;
in mat4 sunTransformaton;
in vec3 sizeTexutre;

out vec4 fragColor;

uniform sampler3D voxelTexture;
uniform sampler3D shaderTexture;
uniform isampler3D sdfTexture;

const float VOXEL_SIZE = 10;
const int MAX_RAY_STEPS = 200;

vec3 sunPosition = (vec4(0, 0, 1, 1) * sunTransformaton).xyz;

vec4 raycastReflect(vec3 rayPos, vec3 rayDir)
{
    ivec3 mapPos = ivec3(floor(rayPos));
    vec3 deltaDist = abs(vec3(1.0) / rayDir);
    ivec3 rayStep = ivec3(sign(rayDir));
    vec3 sideDist = (sign(rayDir) * (vec3(mapPos) - rayPos) + (sign(rayDir) * 0.5) + 0.5) * deltaDist;
    vec4 finalColor = vec4(0, 0, 0, 0);
    float light = 1;
    float face = 1;
    ivec3 mask = ivec3(0, 0, 0);
    int multiplicator = 1;
    int steps = MAX_RAY_STEPS;

    int solid = 1;

    while (steps > 0) {
        if (mapPos.x >= 0 && mapPos.x <= (sizeTexutre.x) &&
            mapPos.y >= 0 && mapPos.y <= (sizeTexutre.y) &&
            mapPos.z >= 0 && mapPos.z <= (sizeTexutre.z)) {
            ivec4 sdf = texelFetch(sdfTexture, mapPos, 0);
            if (sdf.r == 0) {
                vec4 val = texelFetch(voxelTexture, mapPos, 0);
                if (val.w != 0) {
                    finalColor = vec4(mix(finalColor.xyz, val.xyz, 1 - finalColor.w), val.w);
                    if (finalColor.w == 1) {
                        light = texelFetch(shaderTexture, mapPos, 0).r;
                        face = (((dot(sunPosition, vec3(mask.xyz)) + 1) * 1.1 / 2));
                        return vec4(finalColor.rgb * light * face, 1);
                    }
                }
                multiplicator = 1;
            } else {
                multiplicator = sdf.r;
            }
        }
        steps -= multiplicator;
        int u = 0;
        while (u < multiplicator) {
            if (sideDist.x < sideDist.y) {
                if (sideDist.x < sideDist.z) {
                    sideDist.x += deltaDist.x;
                    mapPos.x += rayStep.x;
                    mask = ivec3((rayDir.x < 0) ? 1 : -1, 0, 0);
                } else {
                    sideDist.z += deltaDist.z;
                    mapPos.z += rayStep.z ;
                    mask = ivec3(0, 0, (rayDir.z < 0) ? 1 : -1);
                }
            } else {
                if (sideDist.y < sideDist.z) {
                    sideDist.y += deltaDist.y;
                    mapPos.y += rayStep.y ;
                    mask = ivec3(0, (rayDir.y < 0) ? 1 : -1, 0);
                } else {
                    sideDist.z += deltaDist.z;
                    mapPos.z += rayStep.z ;
                    mask = ivec3(0, 0, (rayDir.z < 0) ? 1 : -1);
                }
            }
            u++;
        }
    }
    return vec4(0, 0, 0, 0);
}

bool rayIsIntersectingTexture(vec3 rayDir, vec3 rayOrigin, vec3 cubePos, float cubeSize) {
    float tmin = 0;
    float tmax = MAX_RAY_STEPS * VOXEL_SIZE;
    vec3 normal = vec3(0, 0, 0);

    for (int i = 0; i < 3; i++) {
        if (abs(rayDir[i]) < 1e-6) {
            if (rayOrigin[i] < cubePos[i] || rayOrigin[i] > cubePos[i] + cubeSize) {
                return false;
            }
        } else {
            float t1 = (cubePos[i] - rayOrigin[i]) / rayDir[i];
            float t2 = (cubePos[i] + cubeSize - rayOrigin[i]) / rayDir[i];

            if (t1 > t2) {
                float temp = t1;
                t1 = t2;
                t2 = temp;
            }

            if (t1 > tmin) {
                tmin = t1;
            }

            if (t2 < tmax) {
                tmax = t2;
            }

            if (tmin > tmax) {
                return false;
            }
        }
    }
    return true;
}

vec4 raycast(vec3 rayPos, vec3 rayDir)
{
    vec3 caca = rayPos;
    rayPos = rayPos / VOXEL_SIZE;
    ivec3 mapPos = ivec3(floor(rayPos));
    vec3 deltaDist = abs(vec3(1.0) / rayDir);
    ivec3 rayStep = ivec3(sign(rayDir));
    vec3 sideDist = (sign(rayDir) * (vec3(mapPos) - rayPos) + (sign(rayDir) * 0.5) + 0.5) * deltaDist;
    vec4 finalColor = vec4(0, 0, 0, 0);
    float light = 1;
    float face = 1;
    ivec3 mask = ivec3(0, 0, 0);

    int multiplicator = 1;

    int steps = MAX_RAY_STEPS;

    while (steps > 0) {
        if (mapPos.x >= 0 && mapPos.x <= (sizeTexutre.x) &&
            mapPos.y >= 0 && mapPos.y <= (sizeTexutre.y) &&
            mapPos.z >= 0 && mapPos.z <= (sizeTexutre.z)) {
            ivec4 sdf = texelFetch(sdfTexture, mapPos, 0);
            if (sdf.r == 0) {
                vec4 val = texelFetch(voxelTexture, mapPos, 0);
                if (val.w != 0) {
                    if (finalColor.w == 0) {
                        if (val.w != 1) {
                            // vec3 dir = normalize(vec3(mapPos) - rayPos);
                            // vec3 pos = mapPos;

                            vec3 dir = rayDir;
                            vec3 pos = mapPos;

                            vec4 reflection = raycastReflect(pos, reflect(dir, mask)) * (val.w);

                            vec4 transparency = raycastReflect(pos, dir) * (1-val.w);

                            finalColor = vec4(reflection.xyz * val.w + transparency.xyz * (1 - val.w), 1);

                            // finalColor = vec4(mix(transparency.xyz, val.xyz, val.w), 1);
                            if (val.w > 0.5) {
                                finalColor = vec4(mix(finalColor.xyz, val.xyz, (1 - val.w)), 1);
                                return vec4(finalColor.rgb, (reflection.w == 0) ? (1-val.w) : 1);
                            } else {
                                finalColor = vec4(mix(finalColor.xyz, val.xyz, val.w), 1);
                                return vec4(finalColor.rgb, (transparency.w == 0) ? (val.w) : 1);

                            }
                        }
                    }
                    finalColor = vec4(mix(finalColor.xyz, val.xyz, 1 - finalColor.w), val.w);
                    light = texelFetch(shaderTexture, mapPos, 0).r;
                    face = (((dot(sunPosition, vec3(mask.xyz)) + 1) * 1.1 / 2));
                    return vec4(finalColor.rgb * light * face, 1);
                }
                multiplicator = 1;
            } else {
                multiplicator = sdf.r;
            }
        }
        steps -= multiplicator;
        int u = 0;
        while (u < multiplicator) {
            if (sideDist.x < sideDist.y) {
                if (sideDist.x < sideDist.z) {
                    sideDist.x += deltaDist.x;
                    mapPos.x += rayStep.x;
                    mask = ivec3((rayDir.x < 0) ? 1 : -1, 0, 0);
                } else {
                    sideDist.z += deltaDist.z;
                    mapPos.z += rayStep.z ;
                    mask = ivec3(0, 0, (rayDir.z < 0) ? 1 : -1);
                }
            } else {
                if (sideDist.y < sideDist.z) {
                    sideDist.y += deltaDist.y;
                    mapPos.y += rayStep.y ;
                    mask = ivec3(0,(rayDir.y < 0) ? 1 : -1, 0);
                } else {
                    sideDist.z += deltaDist.z;
                    mapPos.z += rayStep.z ;
                    mask = ivec3(0, 0, (rayDir.z < 0) ? 1 : -1);
                }
            }
            u++;
        }
    }
    return vec4(finalColor.rgb * face * light, finalColor.w);
}

void main()
{
    vec2 fragPosition = gl_FragCoord.xy;

    vec3 rayDirection = normalize(
        vec4(vec4(fragPosition, 0.0, 1.0) * cameraDirection).xyz
        - vec4(vec4(1024 / 2, 768 / 2, -1000.0, 1.0) * cameraDirection).xyz
    );
    if (rayIsIntersectingTexture(rayDirection, cameraPosition, vec3(0.0, 0.0, 0.0), sizeTexutre.x * VOXEL_SIZE) == false) {
        fragColor = vec4(0, 0, 0, 0);
    } else {
        fragColor = raycast(cameraPosition, rayDirection);
    }
}