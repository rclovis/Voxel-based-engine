#version 330 core

in vec3 cameraPosition;
in mat4 cameraDirection;
in mat4 sunTransformaton;
// const vec3 sunPosition = vec3(0, 1, -1);

out vec4 fragColor;

uniform sampler3D voxelTexture;
uniform sampler3D voxelShade;


const int GRID_SIZE = 100;
const float VOXEL_SIZE = 5;
const int MAX_RAY_STEPS = 200;

vec3 sunPosition = (vec4(0, 0, 1, 1) * sunTransformaton).xyz;


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


vec4 raycast(vec3 rayPos, vec3 rayDir, out float distance)
{
    rayPos = rayPos / VOXEL_SIZE + 0.5;
    ivec3 mapPos = ivec3(floor(rayPos + 0.));
    vec3 deltaDist = abs(vec3(length(rayDir)) / rayDir);
    ivec3 rayStep = ivec3(sign(rayDir));
    vec3 sideDist = (sign(rayDir) * (vec3(mapPos) - rayPos) + (sign(rayDir) * 0.5) + 0.5) * deltaDist;
    vec4 finalColor = vec4(0, 0, 0, 0);
    float light = 1;
    float face = 1;
    ivec3 mask = ivec3(1);

    for (int i = 0; i < MAX_RAY_STEPS; i++) {
        if (mapPos.x >= 0 && mapPos.x <= (GRID_SIZE) &&
            mapPos.y >= 0 && mapPos.y <= (GRID_SIZE) &&
            mapPos.z >= 0 && mapPos.z <= (GRID_SIZE)) {
            vec4 val = texture(voxelTexture, (vec3(mapPos) + 0.5) / float(GRID_SIZE));
            if (val.w != 0) {
                if (finalColor.w == 0) {
                    // light = raycastLignt((mapPos), sunPosition);
                    light = texture(voxelShade, (vec3(mapPos) + 0.5) / float(GRID_SIZE)).w;
                    face = dot(sunPosition, vec3(mask));
                    face = (((face + 1) * 1.1 / 2));
                }
                finalColor = vec4(mix(finalColor.xyz, val.xyz, 1 - finalColor.w), finalColor.w + val.w * (1 - finalColor.w));
                if (finalColor.w == 1) {
                    return vec4(finalColor.rgb * light * face, finalColor.w);
                }
            }
        }
        if (sideDist.x < sideDist.y) {
            if (sideDist.x < sideDist.z) {
                sideDist.x += deltaDist.x;
                mapPos.x += rayStep.x;
                if (rayDir.x < 0)
                    mask = ivec3(1, 0, 0);
                else
                    mask = ivec3(-1, 0, 0);
            } else {
                sideDist.z += deltaDist.z;
                mapPos.z += rayStep.z;
                if (rayDir.z < 0)
                    mask = ivec3(0, 0, 1);
                else
                    mask = ivec3(0, 0, -1);
            }
        } else {
            if (sideDist.y < sideDist.z) {
                sideDist.y += deltaDist.y;
                mapPos.y += rayStep.y;
                if (rayDir.y < 0)
                    mask = ivec3(0, 1, 0);
                else
                    mask = ivec3(0, -1, 0);
            } else {
                sideDist.z += deltaDist.z;
                mapPos.z += rayStep.z;
                if (rayDir.z < 0)
                    mask = ivec3(0, 0, 1);
                else
                    mask = ivec3(0, 0, -1);
            }
        }
    }
    return vec4(face * finalColor.rgb * light, finalColor.w);
}



void main()
{
    vec2 fragPosition = gl_FragCoord.xy;

    vec3 rayDirection = normalize(
        vec4(vec4(fragPosition, 0.0, 1.0) * cameraDirection).xyz
        - vec4(vec4(1024 / 2, 768 / 2, -1000.0, 1.0) * cameraDirection).xyz
    );

    // vec3 rayDirection = normalize(
    //     vec4(vec4(fragPosition, 0.0, 1.0)).xyz
    //     - vec4(vec4(1024 / 2, 768 / 2, -700.0, 1.0)).xyz
    // );

    float distance;

    fragColor = raycast(cameraPosition, rayDirection, distance);
    // fragColor = vec4(1, 1, 1, 0);
    // fragColor = vec4(vec3(distance / 1000), fragColor.w);
}