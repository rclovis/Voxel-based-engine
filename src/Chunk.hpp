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

#define CHUNK_SIZE 200

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
        void loadData();
        void bindTextures(GLuint shader);
        void setVoxel (int x, int y, int z, Voxel voxel);
        void printChunk ();

    protected:
    private:
        std::vector<Voxel> _voxels;
        GLuint _textureColor;
        GLuint _textureShade;
        GLuint _textureDisance;
};

