#include "Chunk.hpp"

Chunk::Chunk()
{
    _voxels = std::vector<Voxel>(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE, {SDF_LIMIT, 0, 0, 0});
}

Chunk::~Chunk()
{
}

void Chunk::updateShadows(GLuint computeShader, GLuint computeShaderAverage, glm::mat4 sun_tansformation)
{
    glUseProgram(computeShader);
    glBindImageTexture(0, _textureInfo, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8UI);

    glUniform1i(glGetUniformLocation(computeShader, "outputTexture"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, _textureInfo);

    GLuint blockIndex = glGetUniformBlockIndex(computeShader, "colorPalette");
    glUniformBlockBinding(computeShader, blockIndex, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, _colorPalette);

    glUniformMatrix4fv(glGetUniformLocation(computeShader, "sunTransformation"), 1, GL_FALSE, &sun_tansformation[0][0]);
    glUniform3f(glGetUniformLocation(computeShader, "size"), CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE);
    glUniform1i(glGetUniformLocation(computeShader, "sdf"), 0);

    glDispatchCompute(CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, 0);
    glBindImageTexture(0, 0, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8UI);






    glUseProgram(computeShaderAverage);
    glBindImageTexture(0, _textureInfo, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8UI);
    glBindImageTexture(1, _textureSDF, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32I);

    glUniform1i(glGetUniformLocation(computeShaderAverage, "outputTexture"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, _textureInfo);

    glUniform1i(glGetUniformLocation(computeShaderAverage, "tmp"), 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, _textureSDF);

    glUniformMatrix4fv(glGetUniformLocation(computeShaderAverage, "sunTransformation"), 1, GL_FALSE, &sun_tansformation[0][0]);
    glUniform3f(glGetUniformLocation(computeShaderAverage, "size"), CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE);
    glUniform1i(glGetUniformLocation(computeShaderAverage, "stepNo"), 0);

    glDispatchCompute(CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    glUniform1i(glGetUniformLocation(computeShaderAverage, "stepNo"), 1);

    glDispatchCompute(CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


    // glUniform1i(glGetUniformLocation(computeShaderAverage, "outputTexture"), 0);
    // if (glGetUniformLocation(computeShaderAverage, "outputTexture") == -1) {
    //     std::cout << "outputTexture not found" << std::endl;
    // }
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_3D, _textureInfo);

    // glUniform1i(glGetUniformLocation(computeShaderAverage, "tmp"), 1);
    // if (glGetUniformLocation(computeShaderAverage, "tmp") == -1) {
    //     std::cout << "tmp not found" << std::endl;
    // }
    // glActiveTexture(GL_TEXTURE1);
    // glBindTexture(GL_TEXTURE_3D, _textureSDF);

    // glUniformMatrix4fv(glGetUniformLocation(computeShaderAverage, "sunTransformation"), 1, GL_FALSE, &sun_tansformation[0][0]);
    // glUniform3f(glGetUniformLocation(computeShaderAverage, "size"), CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE);
    // glUniform1i(glGetUniformLocation(computeShaderAverage, "stepNo"), 1);

    // glDispatchCompute(CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE);
    // glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void Chunk::updateSdf(GLuint computeShader, glm::mat4 sun_tansformation)
{
    glUseProgram(computeShader);
    glBindImageTexture(0, _textureInfo, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8UI);

    glUniform1i(glGetUniformLocation(computeShader, "outputTexture"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, _textureInfo);

    glUniform1i(glGetUniformLocation(computeShader, "sdfTexture"), 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, _textureSDF);

    glUniformMatrix4fv(glGetUniformLocation(computeShader, "sunTransformation"), 1, GL_FALSE, &sun_tansformation[0][0]);
    glUniform3f(glGetUniformLocation(computeShader, "size"), CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE);
    glUniform1i(glGetUniformLocation(computeShader, "sdf"), 1);

    glDispatchCompute(CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, 0);
    glBindImageTexture(0, 0, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8UI);
    glBindImageTexture(1, 0, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32I);
}

void Chunk::loadData()
{
    glGenTextures(1, &_textureInfo);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, _textureInfo);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8UI, CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE, 0, GL_RGBA_INTEGER,  GL_UNSIGNED_BYTE, _voxels.data());
    glBindImageTexture(0, _textureInfo, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8UI);
    glBindTexture(GL_TEXTURE_3D, 0);


    std::vector<int> distance(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE, SDF_LIMIT);

    glGenTextures(1, &_textureSDF);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, _textureSDF);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R32I, CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE, 0, GL_RED_INTEGER, GL_INT, distance.data());
    glBindImageTexture(1, _textureSDF, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32I);
    glBindTexture(GL_TEXTURE_3D, 0);
}

void Chunk::bindTextures (GLuint shader)
{
    glBindImageTexture(0, _textureInfo, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8UI);
    glUniform1i(glGetUniformLocation(shader, "voxelTexture"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, _textureInfo);

    GLuint blockIndex = glGetUniformBlockIndex(shader, "colorPalette");
    glUniformBlockBinding(shader, blockIndex, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, _colorPalette);
}


void Chunk::unbindTextures ()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, 0);
}

void Chunk::setVoxel (int x, int y, int z, Voxel voxel)
{
    _voxels[z * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + x] = voxel;
}

void Chunk::setPalette (std::vector<unsigned int> palette)
{
    glGenBuffers(1, &_colorPalette);
    glBindBuffer(GL_UNIFORM_BUFFER, _colorPalette);
    glBufferData(GL_UNIFORM_BUFFER, 256 * sizeof(unsigned int), palette.data(), GL_STATIC_DRAW);
}
