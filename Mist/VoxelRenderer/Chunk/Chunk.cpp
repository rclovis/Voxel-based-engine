#include "Chunk.hpp"

Chunk::Chunk(int chunkSize)
{
    _chunkSize = chunkSize;
    _voxels = std::vector<Voxel>(_chunkSize * _chunkSize * _chunkSize, {SDF_LIMIT, 0, 0, 0});
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
    glUniform3f(glGetUniformLocation(computeShader, "size"), _chunkSize, _chunkSize, _chunkSize);
    glUniform1i(glGetUniformLocation(computeShader, "sdf"), 0);

    glDispatchCompute(_chunkSize, _chunkSize, _chunkSize);
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
    glUniform3f(glGetUniformLocation(computeShaderAverage, "size"), _chunkSize, _chunkSize, _chunkSize);
    glUniform1i(glGetUniformLocation(computeShaderAverage, "stepNo"), 0);

    glDispatchCompute(_chunkSize, _chunkSize, _chunkSize);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    glUniform1i(glGetUniformLocation(computeShaderAverage, "stepNo"), 1);

    glDispatchCompute(_chunkSize, _chunkSize, _chunkSize);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
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
    glUniform3f(glGetUniformLocation(computeShader, "size"), _chunkSize, _chunkSize, _chunkSize);
    glUniform1i(glGetUniformLocation(computeShader, "sdf"), 1);

    glDispatchCompute(_chunkSize, _chunkSize, _chunkSize);
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
    for (int z = 0; z < _chunkSize; z++) {
        for (int y = 0; y < _chunkSize; y++) {
            for (int x = 0; x < _chunkSize; x++) {
                int index = x + y * _chunkSize + z * _chunkSize * _chunkSize;
                int tmp1 = std::min(std::min(x, y), z) + 1;
                int tmp2 = std::min(std::min(_chunkSize - x, _chunkSize - y), _chunkSize - z);
                int tmp3 = std::min(tmp1, tmp2);
                if (tmp3 < SDF_LIMIT) _voxels[index].distance = tmp3;
                // printf("%d ", _voxels[index].distance);
            }
            // printf("\n");
        }
        // printf("\n\n");
    }
    // exit(0);

    glGenTextures(1, &_textureInfo);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, _textureInfo);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8UI, _chunkSize, _chunkSize, _chunkSize, 0, GL_RGBA_INTEGER,  GL_UNSIGNED_BYTE, _voxels.data());
    glBindImageTexture(0, _textureInfo, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8UI);
    glBindTexture(GL_TEXTURE_3D, 0);


    std::vector<int> distance(_chunkSize * _chunkSize * _chunkSize, SDF_LIMIT);

    for (int z = 0; z < _chunkSize; z++) {
        for (int y = 0; y < _chunkSize; y++) {
            for (int x = 0; x < _chunkSize; x++) {
                int index = x + y * _chunkSize + z * _chunkSize * _chunkSize;
                distance[index] = _voxels[index].distance;
            }
            // printf("\n");
        }
        // printf("\n\n");
    }

    glGenTextures(1, &_textureSDF);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, _textureSDF);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R32I, _chunkSize, _chunkSize, _chunkSize, 0, GL_RED_INTEGER, GL_INT, distance.data());
    glBindImageTexture(1, _textureSDF, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32I);
    glBindTexture(GL_TEXTURE_3D, 0);
}

void Chunk::bindTextures (GLuint shader, int index)
{
    // glBindImageTexture(0, _textureInfo, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8UI);
    glUniform1i(glGetUniformLocation(shader, ("voxelTexture[" + std::to_string(index) + "]").c_str()), index);
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_3D, _textureInfo);
    glUniform3i(glGetUniformLocation(shader, ("voxelTexturePosition[" + std::to_string(index) + "]").c_str()), _X, _Y, _Z);
    // printf("Chunk %d %d %d\n", _X, _Y, _Z);

    GLuint blockIndex = glGetUniformBlockIndex(shader, "colorPalette");
    glUniformBlockBinding(shader, blockIndex, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, _colorPalette);
}


void Chunk::unbindTextures()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, 0);
}

void Chunk::setVoxel (int x, int y, int z, Voxel voxel)
{
    _voxels[z * _chunkSize * _chunkSize + y * _chunkSize + x] = voxel;
}

void Chunk::setPalette (std::vector<unsigned int> palette)
{
    glGenBuffers(1, &_colorPalette);
    glBindBuffer(GL_UNIFORM_BUFFER, _colorPalette);
    glBufferData(GL_UNIFORM_BUFFER, 256 * sizeof(unsigned int), palette.data(), GL_STATIC_DRAW);
}

void Chunk::setPosition (int x, int y, int z)
{
    _X = x;
    _Y = y;
    _Z = z;
}