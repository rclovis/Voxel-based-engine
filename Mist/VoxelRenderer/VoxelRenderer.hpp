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
#include "Chunk.hpp"
#include "Camera.hpp"
#include "Shader.hpp"
#include <unistd.h>


GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);
GLuint LoadComputeShader (const char * computePath);

struct sizeChunk {
  int x;
  int y;
  int z;
};

class VoxelRenderer
{
    public:
        VoxelRenderer();
        ~VoxelRenderer();
        void init(GLFWwindow* window, int chunkSize, int debug);
        void draw();
        void moveSun();
        void setPalette(std::vector<uint32_t> palette);
        void loadChunks();
        void updateShadows();
        Chunk* createChunk();
        int getChunkSize() {return _chunkSize;}

    protected:
        std::map<char, std::vector<std::string>> _assets;
        std::vector<std::string> _map;

    private:
        GLFWwindow* _window;
        GLuint _VAO;
        GLuint _shaderProgram;
        GLuint _computeShader;
        GLuint _computeShaderAverage;
        GLuint _textureArray;

        glm::mat4 _sun_tansformation;
        std::vector<Chunk*> _chunks;

        int _chunkSize;
        int _debug;

        Camera _camera;
};

