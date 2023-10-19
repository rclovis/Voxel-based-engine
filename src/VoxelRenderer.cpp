#include "VoxelRenderer.hpp"
#include <unistd.h>

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

VoxelRenderer::~VoxelRenderer ()
{
    glDeleteProgram(_shaderProgram);
    glDeleteVertexArrays(1, &_VAO);
    std::cout << LOG_VOXEL("VoxelRenderer destroyed");
}

VoxelRenderer::VoxelRenderer()
{
    _window = NULL;
    _VAO = 0;
    _shaderProgram = 0;
}

void VoxelRenderer::init(GLFWwindow* window, int chunkSize, int debug)
{
    _debug = debug;
    std::cout << LOG_VOXEL("Init VoxelRenderer");
    _window = window;
    glGenVertexArrays(1, &_VAO);
    glBindVertexArray(_VAO);
    _chunkSize = chunkSize;

    _shaderProgram = LoadShaders("shader/vertexShader.glsl", "shader/fragmentShader.glsl");
    _computeShader = LoadComputeShader("shader/computeShader.glsl");
    _computeShaderAverage = LoadComputeShader("shader/averageShadows.glsl");

    initCamera();
}

void VoxelRenderer::initCamera()
{
	_up_vector = glm::vec3(0.0f, 0.0f, 1.0f);
    _camera_position = glm::vec3(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 0.0f);
	_camera_direction = glm::vec3(0.0f, 0.0f, 1.0f);
    _sun_tansformation = glm::mat4(1.0f);
}

void VoxelRenderer::draw ()
{
    glUseProgram(_shaderProgram);
    glBindVertexArray(_VAO);
    for (size_t i = 0;i < _chunks.size();i++) {
        _chunks[i]->bindTextures(_shaderProgram, i);
    }
    glUniformMatrix3fv(glGetUniformLocation(_shaderProgram, "MVP"), 1, GL_FALSE, &_rotationMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(_shaderProgram, "sun_transformation"), 1, GL_FALSE, &_sun_tansformation[0][0]);
    glUniform3f(glGetUniformLocation(_shaderProgram, "camera_position"), _camera_position.x, _camera_position.y, _camera_position.z);
    glUniform3f(glGetUniformLocation(_shaderProgram, "camera_direction"), _camera_direction.x, _camera_direction.y, _camera_direction.z);
    glUniform3f(glGetUniformLocation(_shaderProgram, "size"), _chunkSize, _chunkSize, _chunkSize);
    glUniform1i(glGetUniformLocation(_shaderProgram, "nbrTexture"), _chunks.size());
    glUniform1i(glGetUniformLocation(_shaderProgram, "distanceDisplayv"), _debug);


    glDrawArrays(GL_TRIANGLES, 0, 6);
    for (size_t i = 0;i < _chunks.size();i++) {
        _chunks[i]->unbindTextures();
    }

    glfwGetCursorPos(_window, &_last_x, &_last_y);
    glfwPollEvents();
    updateCamera();
    moveSun();
}

void VoxelRenderer::moveSun ()
{
    if (glfwGetKey(_window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        _sun_tansformation = glm::rotate(_sun_tansformation, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        for (auto &chunk : _chunks) chunk->updateShadows(_computeShader, _computeShaderAverage, _sun_tansformation);
    }
    if (glfwGetKey(_window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        _sun_tansformation = glm::rotate(_sun_tansformation, glm::radians(-1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        for (auto &chunk : _chunks) chunk->updateShadows(_computeShader, _computeShaderAverage,_sun_tansformation);
    }
    if (glfwGetKey(_window, GLFW_KEY_UP) == GLFW_PRESS) {
        _sun_tansformation = glm::rotate(_sun_tansformation, glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        for (auto &chunk : _chunks) chunk->updateShadows(_computeShader, _computeShaderAverage,_sun_tansformation);
    }
    if (glfwGetKey(_window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        _sun_tansformation = glm::rotate(_sun_tansformation, glm::radians(-1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        for (auto &chunk : _chunks) chunk->updateShadows(_computeShader, _computeShaderAverage,_sun_tansformation);
    }
}

void VoxelRenderer::updateCamera ()
{
    float camera_speed = 10.0f;

    double last_mouse_x, last_mouse_y;
    glfwGetCursorPos(_window, &last_mouse_x, &last_mouse_y);


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
    _camera_direction = glm::normalize(direction);

    if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS) {
        _camera_position += camera_speed * _camera_direction;
    }
    if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS) {
        _camera_position -= camera_speed * _camera_direction;
    }
    if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS) {
		_camera_position -= glm::normalize(glm::cross(_camera_direction, _up_vector)) * camera_speed;
    }
    if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS) {
		_camera_position += glm::normalize(glm::cross(_camera_direction, _up_vector)) * camera_speed;
    }
	if (glfwGetKey(_window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		_camera_position += _up_vector * camera_speed;
	}
	if (glfwGetKey(_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		_camera_position -= _up_vector * camera_speed;
	}

    _rotationMatrix = glm::mat3(glm::lookAt(glm::vec3(0.0f), _camera_direction, _up_vector));
}

Chunk* VoxelRenderer::createChunk()
{
    Chunk* chunk = new Chunk(_chunkSize);
    _chunks.push_back(chunk);
    return chunk;
}

void VoxelRenderer::setPalette(std::vector<uint32_t> palette)
{
    for (size_t i = 0; i < _chunks.size(); i++) {
        _chunks[i]->setPalette(palette);
    }
}

void VoxelRenderer::loadChunks()
{
    for (size_t i = 0; i < _chunks.size(); i++) {
        _chunks[i]->loadData();
        _chunks[i]->updateSdf(_computeShader, _sun_tansformation);
    }
}

void VoxelRenderer::updateShadows()
{
    for (size_t i = 0; i < _chunks.size(); i++) {
        _chunks[i]->updateShadows(_computeShader, _computeShaderAverage, _sun_tansformation);
    }
}
