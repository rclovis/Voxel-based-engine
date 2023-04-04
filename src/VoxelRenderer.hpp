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


GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);

struct Voxel {
  float r;
  float g;
  float b;
  float w;
};

struct VoxelHash {
  size_t operator()(const std::tuple<int, int, int>& pos) const {
    size_t hash = std::get<0>(pos);
    hash ^= std::get<1>(pos) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    hash ^= std::get<2>(pos) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    return hash;
  }
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

    protected:
        std::map<char, std::vector<std::string>> _assets;
        std::vector<std::string> _map;

    private:

        GLFWwindow* _window;
        GLuint _VAO;
        GLuint _shaderProgram;

        // GLuint _vertexbuffer;
        // GLuint _indexbuffer;
        // GLuint _colorbuffer;
        // GLuint _normalbuffer;

        GLuint _matrix;
        glm::mat4 _proj;
        glm::mat4 _view;
        glm::mat4 _model;
        glm::mat4 _projection;

        glm::vec3 _camera_position;
        glm::vec3 _camera_direction;
        glm::vec3 _up_vector;

        glm::vec3 _rayOrigin;

        std::vector<GLfloat> _vertices;
        std::vector<unsigned int> _faces;
        std::vector<GLfloat> _color_buffer;
        std::vector<GLfloat> _normal;

        GLuint _textureID;


        double _last_x;
        double _last_y;

        std::unordered_map<std::tuple<int, int, int>, Voxel, VoxelHash> _voxelMap;
};

