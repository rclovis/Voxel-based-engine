#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <logger.hpp>
#include <unordered_map>

#define CHUNK_SIZE 256
#define SDF_LIMIT 7

struct Voxel {
    unsigned char distance;
    unsigned char color;
    unsigned char shade;
    unsigned char index;
};

struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

class Chunk {
    public:
        Chunk();
        ~Chunk();
        void updateSdf(GLuint computeShader, glm::mat4 sun_tansformation);
        void updateShadows(GLuint computeShader, GLuint computeShaderAverage, glm::mat4 sun_tansformation);
        void setPalette (std::vector<unsigned int> palette);
        void loadData();
        void unbindTextures();
        void bindTextures(GLuint shader);
        void setVoxel (int x, int y, int z, Voxel voxel);

    protected:
    private:
        std::vector<Voxel> _voxels;
        GLuint _textureColor;
        GLuint _textureShade;
        GLuint _textureSDF;
        GLuint _textureInfo;
        GLuint _colorPalette;
};
