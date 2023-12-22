#version 430 core

vec2 positions[6] = vec2[](
    vec2(-1.0, -1.0),
    vec2( 1.0, -1.0),
    vec2( 1.0,  1.0),
    vec2(-1.0, -1.0),
    vec2( 1.0,  1.0),
    vec2(-1.0,  1.0)
);

uniform vec3 _cameraPosition;
uniform mat3 _cameraDirection;
uniform mat4 _sunTransformaton;
uniform vec3 _sizeTexutre;
uniform int _numberOfTextures;
uniform int _distanceDisplay;
uniform int _fogDisplay;
uniform float _fov;

out vec3 cameraPosition;
out mat3 cameraDirection;
out mat4 sunTransformaton;
out vec3 sizeTexutre;
out flat int numberOfTextures;
out flat int distanceDisplay;
out flat int fogDisplay;
out flat float fov;

void main()
{
    gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);

    cameraPosition = _cameraPosition;
    cameraDirection = _cameraDirection;
    sunTransformaton = _sunTransformaton;
    sizeTexutre = _sizeTexutre;
    numberOfTextures = _numberOfTextures;
    distanceDisplay = _distanceDisplay;
    fogDisplay = _fogDisplay;
    fov = _fov;
}
