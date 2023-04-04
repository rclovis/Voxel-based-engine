#version 330 core

in vec3 cameraPosition;
in mat4 cameraDirection;

out vec4 fragColor;

uniform sampler3D voxelTexture;

const int GRID_SIZE = 64;
const float VOXEL_SIZE = 50;
const int MAX_RAY_STEPS = 100;

const vec3 sunPosition = vec3(0, 0, 0);


vec4 raycast(vec3 rayPos, vec3 rayDir, out float distance)
{
    rayPos = rayPos / VOXEL_SIZE;
    ivec3 mapPos = ivec3(floor(rayPos + 0.));
    vec3 deltaDist = abs(vec3(length(rayDir)) / rayDir);
    ivec3 rayStep = ivec3(sign(rayDir));
    vec3 sideDist = (sign(rayDir) * (vec3(mapPos) - rayPos) + (sign(rayDir) * 0.5) + 0.5) * deltaDist;

    for (int i = 0; i < MAX_RAY_STEPS; i++) {
        if (mapPos.x >= 0 && mapPos.x <= (GRID_SIZE - 1) &&
            mapPos.y >= 0 && mapPos.y <= (GRID_SIZE - 1) &&
            mapPos.z >= 0 && mapPos.z <= (GRID_SIZE - 1)) {
            vec4 val = texture(voxelTexture, (vec3(mapPos) + 0.5) / float(GRID_SIZE));
            if (val.w == 1) {
                distance = sqrt((mapPos.x - rayPos.x) * (mapPos.x - rayPos.x) +
                                (mapPos.y - rayPos.y) * (mapPos.y - rayPos.y) +
                                (mapPos.z - rayPos.z) * (mapPos.z - rayPos.z)) * VOXEL_SIZE;
                return val;
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
    return vec4(0, 0, 0, 0);
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