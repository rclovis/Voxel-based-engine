#include "Chunk.hpp"

Chunk::Chunk()
{
    _voxels = std::vector<Voxel>(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE, {0.0f, 0.0f, 0.0f, 0.0f});
}

Chunk::~Chunk()
{
}

void Chunk::updateShadows(GLuint computeShader, glm::mat4 sun_tansformation)
{
    glBindImageTexture(0, _textureShade, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, _textureColor, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);
    glBindImageTexture(2, _textureDisance, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32I);
    glUseProgram(computeShader);

    glUniform1i(glGetUniformLocation(computeShader, "outputTexture"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, _textureShade);

    glUniform1i(glGetUniformLocation(computeShader, "inputTexture"), 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, _textureColor);

    glUniformMatrix4fv(glGetUniformLocation(computeShader, "sun_transformation"), 1, GL_FALSE, &sun_tansformation[0][0]);
    glUniform3f(glGetUniformLocation(computeShader, "size"), CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE);
    glUniform1i(glGetUniformLocation(computeShader, "sdf"), 0);
    glDispatchCompute(CHUNK_SIZE , CHUNK_SIZE , CHUNK_SIZE);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // glUniform1i(glGetUniformLocation(computeShader, "outputTexture"), 0);
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_3D, _textureShade);
    // std::vector<float> textureData = std::vector<float>(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);
    // glGetTexImage(GL_TEXTURE_3D, 0, GL_RED, GL_FLOAT, textureData.data());

    // for (auto i : textureData) {
    //     std::cout << i << std::endl;
    // }
}

void Chunk::updateSdf(GLuint computeShader, glm::mat4 sun_tansformation)
{
    glBindImageTexture(0, _textureShade, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, _textureColor, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);
    glBindImageTexture(2, _textureDisance, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32I);
    glUseProgram(computeShader);

    glUniform1i(glGetUniformLocation(computeShader, "inputTexture"), 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, _textureColor);

    glUniform1i(glGetUniformLocation(computeShader, "sdfTexture"), 2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_3D, _textureDisance);

    glUniformMatrix4fv(glGetUniformLocation(computeShader, "sun_transformation"), 1, GL_FALSE, &sun_tansformation[0][0]);
    glUniform3f(glGetUniformLocation(computeShader, "size"), CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE);
    glUniform1i(glGetUniformLocation(computeShader, "sdf"), 1);
    glDispatchCompute(CHUNK_SIZE  , CHUNK_SIZE  , CHUNK_SIZE );
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    glUniform1i(glGetUniformLocation(computeShader, "sdfTexture"), 2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_3D, _textureDisance);
    std::vector<int> textureData2 = std::vector<int>(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);
    glGetTexImage(GL_TEXTURE_3D, 0, GL_RED_INTEGER, GL_INT, textureData2.data());

    // for (int z = 0;z < CHUNK_SIZE;z++) {
    //     for (int y = 0;y < CHUNK_SIZE;y++) {
    //         for (int x = 0;x < CHUNK_SIZE;x++) {
    //             printf("%d", textureData2[z * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + x]);
    //             if (textureData2[z * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + x] < 10)
    //                 printf("  ");
    //             else
    //                 printf(" ");
    //         }
    //         printf("\n");
    //     }
    //     printf("\n\n");
    // }
}


void Chunk::loadData()
{
    glGenTextures(1, &_textureShade);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, _textureShade);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE, 0, GL_RED, GL_FLOAT, NULL);
    glBindImageTexture(0, _textureShade, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32F);
    glBindTexture(GL_TEXTURE_3D, 0);

    glGenTextures(1, &_textureColor);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, _textureColor);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE, 0, GL_RGBA, GL_FLOAT, _voxels.data());
    glBindImageTexture(1, _textureColor, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);
    glBindTexture(GL_TEXTURE_3D, 0);

    std::vector<int> distance(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE, 5);

    glGenTextures(1, &_textureDisance);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, _textureDisance);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R32I, CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE, 0, GL_RED_INTEGER, GL_INT, distance.data());
    glBindImageTexture(2, _textureDisance, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32I);
    glBindTexture(GL_TEXTURE_3D, 0);
}

void Chunk::bindTextures (GLuint shader)
{
    glBindImageTexture(0, _textureShade, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, _textureColor, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);
    glBindImageTexture(2, _textureDisance, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32I);
    glUniform1i(glGetUniformLocation(shader, "voxelTexture"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, _textureColor);

    glUniform1i(glGetUniformLocation(shader, "shaderTexture"), 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, _textureShade);

    glUniform1i(glGetUniformLocation(shader, "sdfTexture"), 2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_3D, _textureDisance);





    // glUniform1i(glGetUniformLocation(_shaderProgram, "voxelTexture"), 0);
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_3D, _chunk._textureColor);

    // glUniform1i(glGetUniformLocation(_shaderProgram, "shaderTexture"), 1);
    // glActiveTexture(GL_TEXTURE1);
    // glBindTexture(GL_TEXTURE_3D, _chunk._textureShade);

    // glUniform1i(glGetUniformLocation(_shaderProgram, "sdfTexture"), 2);
    // glActiveTexture(GL_TEXTURE2);
    // glBindTexture(GL_TEXTURE_3D, _chunk._textureDisance);
}

void Chunk::setVoxel (int x, int y, int z, Voxel voxel)
{
    _voxels[z * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + x] = voxel;
}

void Chunk::printChunk ()
{
    for (int z = 0;z < CHUNK_SIZE;z++) {
        for (int y = 0;y < CHUNK_SIZE;y++) {
            for (int x = 0;x < CHUNK_SIZE;x++) {
                printf("%0.2f ", _voxels[z * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + x].r);
                printf("%0.2f ", _voxels[z * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + x].g);
                printf("%0.2f ", _voxels[z * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + x].b);
                printf("%0.2f ", _voxels[z * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + x].w);
                printf("|| ");
            }
            printf("\n");
        }
        printf("\n\n");
    }
}
