#version 330 core

in vec3 cameraPosition;
in mat4 cameraDirection;
in mat4 sunTransformaton;

out vec4 fragColor;

uniform sampler3D voxelTexture;
uniform sampler3D shaderTexture;

const int GRID_SIZE = 100;
const float VOXEL_SIZE = 5;
const int MAX_RAY_STEPS = 400;

vec3 sunPosition = (vec4(0, 0, 1, 1) * sunTransformaton).xyz;

bool rayIsIntersectingTexture(vec3 rayDir, vec3 rayOrigin, vec3 cubePos, float cubeSize) {
    float tmin = 0;
    float tmax = 2139095039;
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
    vec3 test = rayPos;
    rayPos = rayPos / VOXEL_SIZE + 0.5;
    ivec3 mapPos = ivec3(floor(rayPos + 0.));
    vec3 deltaDist = abs(vec3(length(rayDir)) / rayDir);
    ivec3 rayStep = ivec3(sign(rayDir));
    vec3 sideDist = (sign(rayDir) * (vec3(mapPos) - rayPos) + (sign(rayDir) * 0.5) + 0.5) * deltaDist;
    vec4 finalColor = vec4(0, 0, 0, 0);
    float light = 1;
    float face = 1;
    ivec3 mask = ivec3(0, 0, 0);

    for (int i = 0; i < MAX_RAY_STEPS; i++) {
        if (mapPos.x >= 0 && mapPos.x <= (GRID_SIZE) &&
            mapPos.y >= 0 && mapPos.y <= (GRID_SIZE) &&
            mapPos.z >= 0 && mapPos.z <= (GRID_SIZE)) {
            vec4 val = texture(voxelTexture, (vec3(mapPos) + 0.5) / float(GRID_SIZE));
            if (val.w != 0) {
                finalColor = vec4(mix(finalColor.xyz, val.xyz, 1 - finalColor.w), val.w );
                if (finalColor.w == 1) {
                    // light = raycastLignt((mapPos), sunPosition);
                    light = texture(shaderTexture, (vec3(mapPos) + 0.5) / float(GRID_SIZE)).r;
                    face = (((dot(sunPosition, vec3(mask.xyz)) + 1) * 1.1 / 2));
                    return vec4(finalColor.rgb * light * face, 1);
                }
            }
        }
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
    if (rayIsIntersectingTexture(rayDirection, cameraPosition, vec3(0.0, 0.0, 0.0), GRID_SIZE * VOXEL_SIZE) == false) {
        fragColor = vec4(0, 0, 1, 1);
    } else {
        fragColor = raycast(cameraPosition, rayDirection);
    }
}