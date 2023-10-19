#include "Camera.hpp"

Camera::Camera(int _width, int _height, float _fov) :
    _fov(_fov),
    _width(_width),
    _height(_height)
{
    _cameraPosition = glm::vec3(_width / 2, _height / 2, 0.0f);
    _cameraDirection = glm::vec3(0.0f, 0.0f, 1.0f);
    _upVector = glm::vec3(0.0f, 0.0f, 1.0f);
    _rotationMatrix = glm::mat3(1.0f);
}

Camera::~Camera()
{
}

void Camera::update(GLFWwindow *window)
{
    float camera_speed = 10.0f;

    double last_mouse_x, last_mouse_y;
    glfwGetCursorPos(window, &last_mouse_x, &last_mouse_y);


    // Calculate mouse movement delta
    double delta_x = _last_x - last_mouse_x;
    double delta_y = _last_y - last_mouse_y;

    // Calculate camera direction
    static float yaw = -90.0f;
    static float pitch = 0.0f;

    yaw += delta_x *  0.05f;
    pitch += delta_y *  0.05f;

    // Clamp pitch to avoid flipping
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.z = sin(glm::radians(pitch));
    _cameraDirection = glm::normalize(direction);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        _cameraPosition += camera_speed * _cameraDirection;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        _cameraPosition -= camera_speed * _cameraDirection;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		_cameraPosition -= glm::normalize(glm::cross(_cameraDirection, _upVector)) * camera_speed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		_cameraPosition += glm::normalize(glm::cross(_cameraDirection, _upVector)) * camera_speed;
    }
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		_cameraPosition += _upVector * camera_speed;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		_cameraPosition -= _upVector * camera_speed;
	}

    _rotationMatrix = glm::mat3(glm::lookAt(glm::vec3(0.0f), _cameraDirection, _upVector));
}

void Camera::loadCursor(GLFWwindow *window)
{
    glfwGetCursorPos(window, &_last_x, &_last_y);
}
