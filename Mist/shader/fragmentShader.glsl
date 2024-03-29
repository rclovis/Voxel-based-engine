#version 430 core

in vec3 cameraPosition;
in mat3 cameraDirection;
in mat4 sunTransformaton;
in vec3 sizeTexutre;
in flat int numberOfTextures;
in flat int distanceDisplay;
in flat int fogDisplay;
in flat float fov;
out vec4 fragColor;

uniform usampler3D voxelTexture[30];
uniform ivec3 voxelTexturePosition[30];

uniform colorPalette {
    uint colorData[256];
};

const float VOXEL_SIZE = 10;
const int MAX_RAY_STEPS = 200;

vec3 sunPosition = (vec4(0, 0, 1, 1) * sunTransformaton).xyz;

ivec3 rayIsIntersectingTexture(vec3 rayDir, vec3 rayOrigin, vec3 cubePos, float cubeSize) {
    float tmin = 0;
    float tmax = MAX_RAY_STEPS * VOXEL_SIZE;
    vec3 normal = vec3(0, 0, 0);

    for (int i = 0; i < 3; i++) {
        if (abs(rayDir[i]) < 1e-6) {
            if (rayOrigin[i] < cubePos[i] || rayOrigin[i] > cubePos[i] + cubeSize) {
                return ivec3(0, 0, 0);
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
                return ivec3(0, 0, 0);
            }
        }
    }
    ivec3 pointCoord = ivec3(rayOrigin + rayDir * tmin);
    return pointCoord;
}

float moveRayToTexture(vec3 rayDir, vec3 rayOrigin, vec3 cubePos, float cubeSize) {
    float tmin = 0;
    float tmax = MAX_RAY_STEPS * VOXEL_SIZE;
    rayOrigin -= cubePos * (VOXEL_SIZE);

    for (int i = 0; i < 3; i++) {
        if (abs(rayDir[i]) < 1e-6) {
            if (rayOrigin[i] < 0 || rayOrigin[i] > 0 + cubeSize) {
                return MAX_RAY_STEPS * VOXEL_SIZE;
            }
        } else {
            float t1 = (0 - rayOrigin[i]) / rayDir[i];
            float t2 = (0 + cubeSize - rayOrigin[i]) / rayDir[i];
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
                return MAX_RAY_STEPS * VOXEL_SIZE;
            }
        }
    }
    return tmin;
}

uvec4 fetchData (ivec3 mapPos, vec3 rayDir) {
    for (int i = 0;i < 8;i++) {
        if (mapPos.x >= voxelTexturePosition[i].x && mapPos.x < (voxelTexturePosition[i].x + sizeTexutre.x) &&
            mapPos.y >= voxelTexturePosition[i].y && mapPos.y < (voxelTexturePosition[i].y + sizeTexutre.y) &&
            mapPos.z >= voxelTexturePosition[i].z && mapPos.z < (voxelTexturePosition[i].z + sizeTexutre.z)) {
            return texelFetch(voxelTexture[i], ivec3(mapPos.x - voxelTexturePosition[i].x, mapPos.y - voxelTexturePosition[i].y, mapPos.z - voxelTexturePosition[i].z), 0);
        }
    }
    if (rayDir != ivec3(0, 0, 0)) {
        uint distanceV[] = uint[](MAX_RAY_STEPS, MAX_RAY_STEPS, MAX_RAY_STEPS, MAX_RAY_STEPS, MAX_RAY_STEPS, MAX_RAY_STEPS, MAX_RAY_STEPS, MAX_RAY_STEPS);
        for (int i = 0;i < 8;i++) {
            ivec3 p = rayIsIntersectingTexture(rayDir, vec3(mapPos), vec3(voxelTexturePosition[i]), sizeTexutre.x);
            if (p != ivec3(0, 0, 0)) {
                // distanceV[i] = uint(sqrt(pow(p.x - mapPos.x, 2) + pow(p.y - mapPos.y, 2) + pow(p.z - mapPos.z, 2)));
                distanceV[i] = uint(max(max(abs(mapPos.x - p.x), abs(mapPos.y - p.y)), abs(mapPos.z - p.z)));
            }
        }
        int minV = MAX_RAY_STEPS;
        for (int i = 0;i < 8;i++) {
            if (distanceV[i] < minV) {
                minV = 1;
                break;
            }
        }
        // minV -= 7;
        return uvec4(minV, 0, 0, 0);
    }

    return uvec4(1, 0, 0, 0);
}

vec4 raycastReflect(vec3 rayPos, vec3 rayDir, float opacity, int steps) {
    ivec3 mapPos = ivec3(rayPos);
    vec3 deltaDist = abs(vec3(1.0) / rayDir);
    ivec3 rayStep = ivec3(sign(rayDir));
    vec3 sideDist = (sign(rayDir) * (vec3(mapPos) - rayPos) + (sign(rayDir) * 0.5) + 0.5) * deltaDist;
    int multiplicator = 1;

    while (steps > 0) {
        uvec4 sdf = fetchData(mapPos, rayDir);
        if (sdf.r == 0) {
            vec4 val = unpackUnorm4x8(colorData[sdf.g]);
            if (val.w != opacity) {
                return vec4(val.rgb * (sdf.b / 255.0 + 0.2), 1);
            }
            multiplicator = 1;
        } else {
            multiplicator = int(sdf.r);
        }
        steps -= multiplicator;
        if (steps < 0) break;
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
    return vec4(0, 0, 0, 0);
}

vec4 SpecularShading (vec3 reflect_ray, ivec3 texel_pos) {
    float specular = 0;
    if (fetchData(texel_pos, ivec3(0, 0, 0)).b > 100) {
        specular = pow(max(0, dot(reflect_ray, normalize(sunPosition))), 32);
    }
    return vec4(1, 1, 1, specular);
}

vec4 computeColor (ivec3 mapPos, vec3 rayPos, ivec3 mask, int steps)
{
    vec4 finalColor = vec4(0, 0, 0, 0);
    vec4 val = unpackUnorm4x8(colorData[fetchData(mapPos, ivec3(0, 0, 0)).g]);
    if (val.w != 1) {
        vec3 dir = normalize(vec3(mapPos) - rayPos);
        vec3 pos = mapPos;
        vec4 transparency = raycastReflect(pos, dir, val.w, steps) * (1-val.w);
        vec4 reflection = raycastReflect(pos, reflect(dir, ivec3(mask)), val.w, steps) * (val.w);
        vec4 specular = SpecularShading(reflect(dir, ivec3(mask)), mapPos);
        reflection = vec4(mix(reflection.rgb, specular.rgb, specular.a), reflection.w);
        finalColor = vec4(reflection.xyz * val.w + transparency.xyz * (1 - val.w), 1);
        if (val.w > 0.5) {
            finalColor = vec4(mix(finalColor.xyz, val.xyz, (1 - val.w)), 1);
            return vec4(finalColor.rgb, (reflection.w == 0) ? (1-val.w) : 1);
        } else {
            finalColor = vec4(mix(finalColor.xyz, val.xyz, val.w), 1);
            return vec4(finalColor.rgb, (transparency.w == 0) ? (val.w) : 1);
        }
    }
    finalColor = vec4(mix(finalColor.xyz, val.xyz, 1 - finalColor.w), val.w);
    float shadow = 1;
    float light = (fetchData(mapPos + mask, ivec3(0, 0, 0)).b) / 255.0;
    float diffuse = max(0, dot(sunPosition, vec3(mask))) * 1.1;
    shadow = diffuse * light + 0.2;
    return vec4(finalColor.rgb * shadow, 1);
}

vec4 raycast(vec3 rayPos, vec3 rayDir, int maxSteps, out float distance)
{
    rayPos /= VOXEL_SIZE;
    ivec3 mapPos = ivec3(rayPos);
    vec3 deltaDist = abs(vec3(1.0) / rayDir);
    ivec3 rayStep = ivec3(sign(rayDir));
    vec3 sideDist = (sign(rayDir) * (vec3(mapPos) - rayPos) + (sign(rayDir) * 0.5) + 0.5) * deltaDist;
    ivec3 mask = ivec3(0, 0, 0);
    int multiplicator = 1;
    int steps = maxSteps;
    int numberOfChecks = 0;
    int numberOfJump = 0;

    while (steps > 0) {
        uvec4 sdf = fetchData(mapPos, rayDir);
        numberOfChecks++;
        if (sdf.r == 0) {
            if (distanceDisplay == 1) {
                distance = length(vec3(mapPos) - rayPos);
                return mix(computeColor (mapPos, rayPos, mask, steps), vec4(1, 0, 0, 1),  float(numberOfChecks) / float(numberOfJump));
            } else {
                distance = length(vec3(mapPos) - rayPos);
                return computeColor (mapPos, rayPos, mask, steps);
            }
        } else {
            multiplicator = int(sdf.r);
            numberOfJump += multiplicator;
        }
        steps -= multiplicator;
        if (steps < 0) break;
        int u = 0;
        while (u < multiplicator) {
            if (sideDist.x < sideDist.y) {
                if (sideDist.x < sideDist.z) {
                    sideDist.x += deltaDist.x;
                    mapPos.x += rayStep.x;
                    mask = ivec3((rayDir.x < 0) ? 1 : -1, 0, 0);
                } else {
                    sideDist.z += deltaDist.z;
                    mapPos.z += rayStep.z;
                    mask = ivec3(0, 0, (rayDir.z < 0) ? 1 : -1);
                }
            } else {
                if (sideDist.y < sideDist.z) {
                    sideDist.y += deltaDist.y;
                    mapPos.y += rayStep.y;
                    mask = ivec3(0,(rayDir.y < 0) ? 1 : -1, 0);
                } else {
                    sideDist.z += deltaDist.z;
                    mapPos.z += rayStep.z;
                    mask = ivec3(0, 0, (rayDir.z < 0) ? 1 : -1);
                }
            }
            u++;
        }
    }
    if (distanceDisplay == 1) {
        distance = length(vec3(mapPos) - rayPos);
        return mix(vec4(0, 0, 0, 0), vec4(1, 0, 0, 1),  float(numberOfChecks) / float(numberOfJump));
    } else {
        distance = length(vec3(mapPos) - rayPos);
        return vec4(0, 0, 0, 0);
    }
}

vec3 getRay(vec2 fragPosition)
{
    float fovRad = fov * (3.1415926535897932384626433832795 / 180.);

    float size = max(1024, 768);
    int offset = abs(1024 - 768) / 2;
    if (1024 > 768) {
        fragPosition.y += offset;
    } else {
        fragPosition.x += offset;
    }
    float ndcX = (2. * fragPosition.x) / size - 1.;
    float ndcY = (2. * fragPosition.y) / size - 1.;

    // Calculate direction vector in camera space looking to Y+
    float dirX = ndcX * tan(fovRad / 2.);
    float dirY = ndcY * tan(fovRad / 2.);
    float dirZ = -1.;
    return vec3(dirX, dirY, dirZ) * cameraDirection;
}

void main()
{
    vec2 fragPosition = gl_FragCoord.xy;
    vec3 rayDirection = getRay(fragPosition);
    float minT = MAX_RAY_STEPS * VOXEL_SIZE;
    for (int i = 0;i < 8;i++) {
        float tmp = moveRayToTexture(rayDirection, cameraPosition, vec3(voxelTexturePosition[i]), sizeTexutre.x * VOXEL_SIZE);
        if (tmp < minT) minT = tmp;
    }
    float distance;
    if (minT == 0) {
        fragColor = raycast(cameraPosition, rayDirection, MAX_RAY_STEPS, distance);
        distance /= MAX_RAY_STEPS;
        if (fogDisplay == 1) {
            fragColor = mix(fragColor, vec4 (distance, distance, distance, 1.), 0.5);
        }
    } else {
        fragColor = raycast(cameraPosition + rayDirection * (minT), rayDirection, int(MAX_RAY_STEPS - minT / VOXEL_SIZE), distance);
        distance += (length(rayDirection) * minT) / VOXEL_SIZE;
        distance /= MAX_RAY_STEPS ;
        if (fogDisplay == 1) {
            fragColor = mix(fragColor, vec4 (distance, distance, distance, 1.), 0.5);
        }
    }
}
