#version 330 core

in vec3 cameraPosition;
in mat4 cameraDirection;

out vec4 fragColor;

uniform sampler3D voxelTexture;

const int GRID_SIZE = 100;
const float VOXEL_SIZE = 5;
const int MAX_RAY_STEPS = 400;

const vec3 sunPosition = (vec3(1, 1, 0.5));

float raycastLignt(vec3 rayPos, vec3 rayDir)
{
    ivec3 mapPos = ivec3(floor(rayPos + 0.));
    vec3 deltaDist = abs(vec3(length(rayDir)) / rayDir);
    ivec3 rayStep = ivec3(sign(rayDir));
    vec3 sideDist = (sign(rayDir) * (vec3(mapPos) - rayPos) + (sign(rayDir) * 0.5) + 0.5) * deltaDist;
    float opacity = 1;

    for (int i = 0; i < MAX_RAY_STEPS; i++) {
        if (mapPos.x >= 0 && mapPos.x <= (GRID_SIZE) &&
            mapPos.y >= 0 && mapPos.y <= (GRID_SIZE) &&
            mapPos.z >= 0 && mapPos.z <= (GRID_SIZE) && i != 0) {
            vec4 val = texture(voxelTexture, (vec3(mapPos) + 0.5) / float(GRID_SIZE));
            val.w /= 2;
            opacity *= (1 - val.w);
            if (opacity <= 0.01) {
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


vec4 raycast(vec3 rayPos, vec3 rayDir, out float distance)
{
    rayPos = rayPos / VOXEL_SIZE + 0.5;
    ivec3 mapPos = ivec3(floor(rayPos + 0.));
    vec3 deltaDist = abs(vec3(length(rayDir)) / rayDir);
    ivec3 rayStep = ivec3(sign(rayDir));
    vec3 sideDist = (sign(rayDir) * (vec3(mapPos) - rayPos) + (sign(rayDir) * 0.5) + 0.5) * deltaDist;
    vec4 finalColor = vec4(0, 0, 0, 0);
    float lignt = 1;
    bvec3 mask = bvec3(false);

    for (int i = 0; i < MAX_RAY_STEPS; i++) {
        if (mapPos.x >= 0 && mapPos.x <= (GRID_SIZE) &&
            mapPos.y >= 0 && mapPos.y <= (GRID_SIZE) &&
            mapPos.z >= 0 && mapPos.z <= (GRID_SIZE)) {
            vec4 val = texture(voxelTexture, (vec3(mapPos) + 0.5) / float(GRID_SIZE));
            if (val.w != 0) {
                if (finalColor.w == 0) {
                    lignt = raycastLignt((mapPos), sunPosition);
                }
                finalColor = vec4(mix(finalColor.xyz, val.xyz, 1 - finalColor.w), finalColor.w + val.w * (1 - finalColor.w));
                if (finalColor.w == 1) {
                    // rayDir = normalize(rayDir);
                    // if (mask.x) {
                    //     lignt *= rayDir.x;
                    // } else if (mask.y) {
                    //     lignt *= rayDir.y;
                    // } else {
                    //     lignt *= rayDir.z;
                    // }
                    return vec4(finalColor.rgb * lignt, finalColor.w);
                }
            }
        }
        if (sideDist.x < sideDist.y) {
            if (sideDist.x < sideDist.z) {
                sideDist.x += deltaDist.x;
                mapPos.x += rayStep.x;
                mask = bvec3(true, false, false);
            } else {
                sideDist.z += deltaDist.z;
                mapPos.z += rayStep.z;
                mask = bvec3(false, false, true);
            }
        } else {
            if (sideDist.y < sideDist.z) {
                sideDist.y += deltaDist.y;
                mapPos.y += rayStep.y;
                mask = bvec3(false, true, false);
            } else {
                sideDist.z += deltaDist.z;
                mapPos.z += rayStep.z;
                mask = bvec3(false, false, true);
            }
        }
    }
    return vec4(finalColor.rgb * lignt, finalColor.w);
}



void main()
{
    vec2 fragPosition = gl_FragCoord.xy;

    vec3 rayDirection = normalize(
        vec4(vec4(fragPosition, 0.0, 1.0) * cameraDirection).xyz
        - vec4(vec4(1024 / 2, 768 / 2, -500.0, 1.0) * cameraDirection).xyz
    );

    float distance;

    fragColor = raycast(cameraPosition, rayDirection, distance);
    // fragColor = vec4(vec3(distance / 1000), fragColor.w);
}