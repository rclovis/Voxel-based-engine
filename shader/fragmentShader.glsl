#version 330 core

in vec3 cameraPosition;
in mat4 cameraDirection;

out vec4 fragColor;

uniform sampler3D voxelTexture;

const int GRID_SIZE = 32;
const float VOXEL_SIZE = 50;

const float MAX_DISTANCE = 10000.0;

vec4 raycast(vec3 start, vec3 d) {
    float steps = max(max(abs(d.x), abs(d.y)), abs(d.z));
    d = normalize(d);
    if (steps == 0)
        return vec4(1, 1, 1, 1);

    float X = start.x;
    float Y = start.y;
    float Z = start.z;
    for (int i = 0;i < MAX_DISTANCE / VOXEL_SIZE;i++) {
        float nX = floor(X / VOXEL_SIZE) / GRID_SIZE;
        float nY = floor(Y / VOXEL_SIZE) / GRID_SIZE;
        float nZ = floor(Z / VOXEL_SIZE) / GRID_SIZE;

        if (nX < 0 || nX > 1 || nY < 0 || nY > 1 || nZ < 0 || nZ > 1) {
            X += (d.x) * VOXEL_SIZE;
            Y += (d.y) * VOXEL_SIZE;
            Z += (d.z) * VOXEL_SIZE;
            continue;
        }
        vec4 val = texture(voxelTexture, vec3(nX, nY, nZ));
        if (val.w == 1)
            return val;

        X += (d.x) * VOXEL_SIZE;
        Y += (d.y) * VOXEL_SIZE;
        Z += (d.z) * VOXEL_SIZE;
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

    fragColor = raycast(cameraPosition, rayDirection);
}



