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

#define CHUNK_SIZE 16

struct Voxel {
  float r;
  float g;
  float b;
  float w;
};

class Chunk {
    public:
        Chunk();
        ~Chunk();
        void updateSdf(GLuint computeShader, glm::mat4 sun_tansformation);
        void updateShadows(GLuint computeShader, glm::mat4 sun_tansformation);
        void loadData(std::vector<Voxel> voxels, int sizeX, int sizeY, int sizeZ);
        void bindTextures(GLuint shader);

    protected:
    private:
        std::vector<Voxel> voxels;
        GLuint _textureColor;
        GLuint _textureShade;
        GLuint _textureDisance;
};

