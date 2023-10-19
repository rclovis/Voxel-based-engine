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

// #define CHUNK_SIZE 16
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
        Chunk(int chunkSize);
        ~Chunk();
        void updateSdf(GLuint computeShader, glm::mat4 sun_tansformation);
        void updateShadows(GLuint computeShader, GLuint computeShaderAverage, glm::mat4 sun_tansformation);
        void setPalette (std::vector<unsigned int> palette);
        void setPosition (int x, int y, int z);
        void loadData();
        void unbindTextures();
        void bindTextures(GLuint shader, int index);
        void setVoxel (int x, int y, int z, Voxel voxel);

    protected:
    private:
        std::vector<Voxel> _voxels;
        GLuint _textureSDF;
        GLuint _textureInfo;
        GLuint _colorPalette;
        int _X;
        int _Y;
        int _Z;
        int _chunkSize;
};
