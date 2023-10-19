#include "VoxelRenderer.hpp"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

VoxelRenderer::~VoxelRenderer ()
{
    glDeleteProgram(_shaderProgram);
    glDeleteVertexArrays(1, &_VAO);
    std::cout << LOG_VOXEL("VoxelRenderer destroyed");
}

VoxelRenderer::VoxelRenderer() :
    _camera(WINDOW_WIDTH, WINDOW_HEIGHT, 90.0f)
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

    _shaderProgram = Shader::LoadShaders("Mist/shader/vertexShader.glsl", "Mist/shader/fragmentShader.glsl");
    _computeShader = Shader::LoadComputeShader("Mist/shader/computeShader.glsl");
    _computeShaderAverage = Shader::LoadComputeShader("Mist/shader/averageShadows.glsl");
    _sun_tansformation = glm::mat4(1.0f);
}

void VoxelRenderer::draw ()
{
    glUseProgram(_shaderProgram);
    glBindVertexArray(_VAO);
    for (size_t i = 0;i < _chunks.size();i++) {
        _chunks[i]->bindTextures(_shaderProgram, i);
    }
    glUniform3f(glGetUniformLocation(_shaderProgram, "_cameraPosition"), _camera.getCameraPosition().x, _camera.getCameraPosition().y, _camera.getCameraPosition().z);
    glUniformMatrix3fv(glGetUniformLocation(_shaderProgram, "_cameraDirection"), 1, GL_FALSE, _camera.getRotationMatrix());
    glUniformMatrix4fv(glGetUniformLocation(_shaderProgram, "_sunTransformaton"), 1, GL_FALSE, &_sun_tansformation[0][0]);
    glUniform3f(glGetUniformLocation(_shaderProgram, "_sizeTexutre"), _chunkSize, _chunkSize, _chunkSize);
    glUniform1i(glGetUniformLocation(_shaderProgram, "_numberOfTextures"), _chunks.size());
    glUniform1i(glGetUniformLocation(_shaderProgram, "_distanceDisplay"), _debug);
    glUniform1f(glGetUniformLocation(_shaderProgram, "_fov"), _camera.getFov());


    glDrawArrays(GL_TRIANGLES, 0, 6);
    for (size_t i = 0;i < _chunks.size();i++) {
        _chunks[i]->unbindTextures();
    }

    _camera.loadCursor(_window);
    glfwPollEvents();
    _camera.update(_window);
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
