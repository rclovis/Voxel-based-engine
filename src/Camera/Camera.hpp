#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

class Camera {
    public:
        Camera(int _width, int _height, float _fov);
        ~Camera();
        void update(GLFWwindow *window);
        void loadCursor(GLFWwindow *window);
        const GLfloat *getRotationMatrix() {return &_rotationMatrix[0][0];}
        glm::vec3 getCameraPosition() {return _cameraPosition;}
        float getFov() {return _fov;}

    protected:
    private:
        glm::vec3 _cameraPosition;
        glm::vec3 _cameraDirection;
        glm::vec3 _upVector;
        glm::mat3 _rotationMatrix;
        float _fov;
        int _width;
        int _height;
        double _last_x;
        double _last_y;
};
