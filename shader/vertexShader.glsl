#version 430 core

vec2 positions[6] = vec2[](
    vec2(-1.0, -1.0),
    vec2( 1.0, -1.0),
    vec2( 1.0,  1.0),
    vec2(-1.0, -1.0),
    vec2( 1.0,  1.0),
    vec2(-1.0,  1.0)
);

uniform mat3 MVP;
uniform mat4 sun_transformation;
uniform vec3 camera_position;
uniform vec3 size;
uniform int nbrTexture;
uniform int distanceDisplayv;

out vec3 cameraPosition;
out mat3 cameraDirection;
out mat4 sunTransformaton;
out vec3 sizeTexutre;
out flat int numberOfTextures;
out flat int distanceDisplay;

void main()
{
    gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);

    cameraPosition = camera_position;
    cameraDirection = MVP;
    sunTransformaton = sun_transformation;
    sizeTexutre = size;
    numberOfTextures = nbrTexture;
    distanceDisplay = distanceDisplayv;
}
