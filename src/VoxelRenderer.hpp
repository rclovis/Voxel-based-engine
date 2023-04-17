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
        void init(GLFWwindow* window);
        void initCamera();
        void updateCamera();
        void draw();
        void moveSun();
        std::vector<Chunk*> voxelDataToChunks (std::vector<Voxel> voxels, int sizeX, int sizeY, int sizeZ, std::vector<unsigned int> colors);
        std::vector<Chunk*> loadVox(const char *path);

    protected:
        std::map<char, std::vector<std::string>> _assets;
        std::vector<std::string> _map;

    private:
        GLFWwindow* _window;
        GLuint _VAO;
        GLuint _shaderProgram;
        GLuint _computeShader;

        glm::mat4 _proj;
        glm::mat4 _view;
        glm::mat4 _model;
        glm::mat4 _projection;
        glm::mat4 _sun_tansformation;

        glm::vec3 _camera_position;
        glm::vec3 _camera_direction;
        glm::vec3 _up_vector;

        std::vector<Chunk*> _chunks;

        double _last_x;
        double _last_y;
};

