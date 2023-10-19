#pragma once
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Shader {
    public:
        Shader();
        ~Shader();
        static GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);
        static GLuint LoadComputeShader (const char * computePath);

    protected:
    private:
};
