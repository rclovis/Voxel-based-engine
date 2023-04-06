/*
** EPITECH PROJECT, 2023
** voxel opengl
** File description:
** Text
*/

#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "logger.hpp"

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);
GLuint LoadComputeShader (const char * computePath);

typedef struct {
    std::string text;
    glm::vec3 color;
    glm::vec2 pos;
    float scale;
} textInfo_t;

struct Character {
    GLuint textureID;   // ID handle of the glyph texture
    glm::ivec2 size;    // Size of glyph
    glm::ivec2 bearing; // Offset from baseline to left/top of glyph
    GLuint advance;     // Horizontal offset to advance to next glyph
};

class TextRenderer {
    public:
        TextRenderer();
        ~TextRenderer();
        void draw();
        void init(GLFWwindow* window);
        void renderText(textInfo_t info);

    private:
        std::map<GLchar, Character> _characters;
        GLuint _VAO;
        GLuint _VBO;
        GLuint _color;
        GLuint _shaderProgram;
        GLFWwindow* _window;
        std::map<GLchar, Character> Characters;
        std::vector<textInfo_t> _text;
};
