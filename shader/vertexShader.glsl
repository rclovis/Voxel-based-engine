#version 330 core

vec2 positions[6] = vec2[](
    vec2(-1.0, -1.0),
    vec2( 1.0, -1.0),
    vec2( 1.0,  1.0),
    vec2(-1.0, -1.0),
    vec2( 1.0,  1.0),
    vec2(-1.0,  1.0)
);


uniform mat4 MVP;
uniform vec3 camera_position;
// uniform vec3 camera_direction;

out vec3 cameraPosition;
out mat4 cameraDirection;


void main()
{
    gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);

    cameraPosition = camera_position;
    cameraDirection = MVP;
}

