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
    _camera_position = glm::vec3(0, 0, 0);
    _camera_direction = glm::vec3(0, 0, -1);
    _up_vector = glm::vec3(0, 1, 0);
}

void VoxelRenderer::init(GLFWwindow* window)
{
    std::cout << LOG_VOXEL("Init VoxelRenderer");
    _window = window;
    glGenVertexArrays(1, &_VAO);
    glBindVertexArray(_VAO);

    _shaderProgram = LoadShaders("shader/vertexShader.glsl", "shader/fragmentShader.glsl");
    _computeShader = LoadComputeShader("shader/computeShader.glsl");

    initCamera();

    // _chunks = loadVox("assets/torus.vox");
    _chunks = loadVox("assets/Temple.vox");
    // _chunks = loadVox("assets/untitled.vox");
    // _chunks[0]->printChunk();
    // _chunks = loadVox("assets/pieta.vox");
    // _chunks = loadVox("assets/city.vox");
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
    _chunks[0]->bindTextures(_shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(_shaderProgram, "MVP"), 1, GL_FALSE, &_proj[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(_shaderProgram, "sun_transformation"), 1, GL_FALSE, &_sun_tansformation[0][0]);
    glUniform3f(glGetUniformLocation(_shaderProgram, "camera_position"), _camera_position.x, _camera_position.y, _camera_position.z);
    glUniform3f(glGetUniformLocation(_shaderProgram, "camera_direction"), _camera_direction.x, _camera_direction.y, _camera_direction.z);
    glUniform3f(glGetUniformLocation(_shaderProgram, "size"), CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    _chunks[0]->unbindTextures();

    glfwGetCursorPos(_window, &_last_x, &_last_y);
    glfwPollEvents();
    updateCamera();
    moveSun();
}


void VoxelRenderer::moveSun ()
{
    if (glfwGetKey(_window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        _sun_tansformation = glm::rotate(_sun_tansformation, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        for (auto &chunk : _chunks) chunk->updateShadows(_computeShader, _sun_tansformation);
    }
    if (glfwGetKey(_window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        _sun_tansformation = glm::rotate(_sun_tansformation, glm::radians(-1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        for (auto &chunk : _chunks) chunk->updateShadows(_computeShader, _sun_tansformation);
    }
    if (glfwGetKey(_window, GLFW_KEY_UP) == GLFW_PRESS) {
        _sun_tansformation = glm::rotate(_sun_tansformation, glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        for (auto &chunk : _chunks) chunk->updateShadows(_computeShader, _sun_tansformation);
    }
    if (glfwGetKey(_window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        _sun_tansformation = glm::rotate(_sun_tansformation, glm::radians(-1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        for (auto &chunk : _chunks) chunk->updateShadows(_computeShader, _sun_tansformation);
    }
    // glm::vec4 sun_pos = glm::vec4(0, 0, 1, 1) * _sun_tansformation;
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

    _view = glm::lookAt(_camera_position, _camera_position + _camera_direction, _up_vector);
    glm::mat4 Model = glm::mat4(1.0f);
	_projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
    _proj = _projection * _view * Model;
}

std::vector<Chunk*> VoxelRenderer::loadVox(const char *path)
{
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        printf("Error: Could not open file %s\n", path);
    }
    fseek(file, 16, SEEK_CUR);
    int totalSize;
    fread(&totalSize, 4, 1, file);

    std::vector<unsigned int> palette;

    long pos = ftell(file);

    for (int i = 0; i < totalSize;) {
        char chunkID[4];
        int chunkContentBytes;
        fread(chunkID, 4, 1, file);
        fread(&chunkContentBytes, 4, 1, file);
        fseek(file, 4, SEEK_CUR);
        i += chunkContentBytes;
        if (strncmp(chunkID, "RGBA", 4) == 0) {
            for (int i = 0; i < chunkContentBytes / 4; i += 1) {
                unsigned int color;
                fread(&color, 4, 1, file);

                if (color == 0xFFFFFFFF) {
                    unsigned char r = 0, g = 0, b = 100, a = 100;
                    color = (a << 24) | (b << 16) | (g << 8) | r;
                }
                if (color == 0xFFFF00FF) {
                    unsigned char r = 0, g = 0, b = 0, a = 250;
                    color = (a << 24) | (b << 16) | (g << 8) | r;
                }

                if (palette.size() == 255)
                    palette.insert(palette.begin(), color);
                else
                    palette.push_back(color);
            }
            break;
        } else {
            fseek(file, chunkContentBytes, SEEK_CUR);
        }
    }
    if (palette.size() != 256) {
        std::cout << "Use default palette" << std::endl;
        unsigned int colors[256] = {
            0x00000000, 0xffffffff, 0xffccffff, 0xff99ffff, 0xff66ffff, 0xff33ffff, 0xff00ffff, 0xffffccff, 0xffccccff, 0xff99ccff, 0xff66ccff, 0xff33ccff, 0xff00ccff, 0xffff99ff, 0xffcc99ff, 0xff9999ff,
            0xff6699ff, 0xff3399ff, 0xff0099ff, 0xffff66ff, 0xffcc66ff, 0xff9966ff, 0xff6666ff, 0xff3366ff, 0xff0066ff, 0xffff33ff, 0xffcc33ff, 0xff9933ff, 0xff6633ff, 0xff3333ff, 0xff0033ff, 0xffff00ff,
            0xffcc00ff, 0xff9900ff, 0xff6600ff, 0xff3300ff, 0xff0000ff, 0xffffffcc, 0xffccffcc, 0xff99ffcc, 0xff66ffcc, 0xff33ffcc, 0xff00ffcc, 0xffffcccc, 0xffcccccc, 0xff99cccc, 0xff66cccc, 0xff33cccc,
            0xff00cccc, 0xffff99cc, 0xffcc99cc, 0xff9999cc, 0xff6699cc, 0xff3399cc, 0xff0099cc, 0xffff66cc, 0xffcc66cc, 0xff9966cc, 0xff6666cc, 0xff3366cc, 0xff0066cc, 0xffff33cc, 0xffcc33cc, 0xff9933cc,
            0xff6633cc, 0xff3333cc, 0xff0033cc, 0xffff00cc, 0xffcc00cc, 0xff9900cc, 0xff6600cc, 0xff3300cc, 0xff0000cc, 0xffffff99, 0xffccff99, 0xff99ff99, 0xff66ff99, 0xff33ff99, 0xff00ff99, 0xffffcc99,
            0xffcccc99, 0xff99cc99, 0xff66cc99, 0xff33cc99, 0xff00cc99, 0xffff9999, 0xffcc9999, 0xff999999, 0xff669999, 0xff339999, 0xff009999, 0xffff6699, 0xffcc6699, 0xff996699, 0xff666699, 0xff336699,
            0xff006699, 0xffff3399, 0xffcc3399, 0xff993399, 0xff663399, 0xff333399, 0xff003399, 0xffff0099, 0xffcc0099, 0xff990099, 0xff660099, 0xff330099, 0xff000099, 0xffffff66, 0xffccff66, 0xff99ff66,
            0xff66ff66, 0xff33ff66, 0xff00ff66, 0xffffcc66, 0xffcccc66, 0xff99cc66, 0xff66cc66, 0xff33cc66, 0xff00cc66, 0xffff9966, 0xffcc9966, 0xff999966, 0xff669966, 0xff339966, 0xff009966, 0xffff6666,
            0xffcc6666, 0xff996666, 0xff666666, 0xff336666, 0xff006666, 0xffff3366, 0xffcc3366, 0xff993366, 0xff663366, 0xff333366, 0xff003366, 0xffff0066, 0xffcc0066, 0xff990066, 0xff660066, 0xff330066,
            0xff000066, 0xffffff33, 0xffccff33, 0xff99ff33, 0xff66ff33, 0xff33ff33, 0xff00ff33, 0xffffcc33, 0xffcccc33, 0xff99cc33, 0xff66cc33, 0xff33cc33, 0xff00cc33, 0xffff9933, 0xffcc9933, 0xff999933,
            0xff669933, 0xff339933, 0xff009933, 0xffff6633, 0xffcc6633, 0xff996633, 0xff666633, 0xff336633, 0xff006633, 0xffff3333, 0xffcc3333, 0xff993333, 0xff663333, 0xff333333, 0xff003333, 0xffff0033,
            0xffcc0033, 0xff990033, 0xff660033, 0xff330033, 0xff000033, 0xffffff00, 0xffccff00, 0xff99ff00, 0xff66ff00, 0xff33ff00, 0xff00ff00, 0xffffcc00, 0xffcccc00, 0xff99cc00, 0xff66cc00, 0xff33cc00,
            0xff00cc00, 0xffff9900, 0xffcc9900, 0xff999900, 0xff669900, 0xff339900, 0xff009900, 0xffff6600, 0xffcc6600, 0xff996600, 0xff666600, 0xff336600, 0xff006600, 0xffff3300, 0xffcc3300, 0xff993300,
            0xff663300, 0xff333300, 0xff003300, 0xffff0000, 0xffcc0000, 0xff990000, 0xff660000, 0xff330000, 0xff0000ee, 0xff0000dd, 0xff0000bb, 0xff0000aa, 0xff000088, 0xff000077, 0xff000055, 0xff000044,
            0xff000022, 0xff000011, 0xff00ee00, 0xff00dd00, 0xff00bb00, 0xff00aa00, 0xff008800, 0xff007700, 0xff005500, 0xff004400, 0xff002200, 0xff001100, 0xffee0000, 0xffdd0000, 0xffbb0000, 0xffaa0000,
            0xff880000, 0xff770000, 0xff550000, 0xff440000, 0xff220000, 0xff110000, 0xffeeeeee, 0xffdddddd, 0xffbbbbbb, 0xffaaaaaa, 0xff888888, 0xff777777, 0xff555555, 0xff444444, 0xff222222, 0xff111111
        };
        palette.clear();
        for (int i = 0; i < 256; i++) {
            palette.push_back(colors[i]);
        }
    }
    fseek(file, pos, SEEK_SET);


    int sizeX, sizeY, sizeZ;
    std::vector<std::vector<Voxel>> fullVoxels;
    std::vector<sizeChunk> transform;

    for (int i = 0; i < totalSize;) {
        char chunkID[4];
        int chunkContentBytes;
        int chunkChildrenBytes;
        fread(chunkID, 4, 1, file);
        fread(&chunkContentBytes, 4, 1, file);
        fread(&chunkChildrenBytes, 4, 1, file);
        i += chunkContentBytes;

        if (strncmp(chunkID, "SIZE", 4) == 0) {
            fread(&sizeX, 4, 1, file);
            fread(&sizeY, 4, 1, file);
            fread(&sizeZ, 4, 1, file);
            fullVoxels.push_back(std::vector<Voxel>(sizeX * sizeY * sizeZ, {SDF_LIMIT, 0, 0, 0}));
        } else if (strncmp(chunkID, "XYZI", 4) == 0) {
            int numVoxels;
            fread(&numVoxels, 4, 1, file);
            for (int j = 0; j < numVoxels; j++) {
                unsigned char x, y, z, colorIndex;
                fread(&x, 1, 1, file);
                fread(&y, 1, 1, file);
                fread(&z, 1, 1, file);
                fread(&colorIndex, 1, 1, file);

                fullVoxels[fullVoxels.size() - 1][x + (y * sizeX) + (z * sizeX * sizeY)] = {SDF_LIMIT, colorIndex, 0, 0};
            }
        } else if (strncmp(chunkID, "nTRN", 4) == 0) {
            fseek(file, 4 * 7, SEEK_CUR);
            int j = 0;
            while (j < chunkContentBytes - 4 * 7) {
                int size = 0;
                fread(&size, 4, 1, file);
                std::string key = std::string(size, '\0');
                fread((char *) key.data(), size, 1, file);
                j += size + 4;

                size = 0;
                fread(&size, 4, 1, file);
                std::string value = std::string(size, '\0');
                fread((char *) value.data(), size, 1, file);
                if (key == "_t") {
                    transform.push_back({
                        std::stoi(value.substr(0, value.find(" "))),
                        std::stoi(value.substr(value.find(" ") + 1, value.find(" ", value.find(" ") + 1))),
                        std::stoi(value.substr(value.find(" ", value.find(" ") + 1) + 1, value.size()))
                    });
                }
                j += size + 4;
            }
        } else {
            fseek(file, chunkContentBytes, SEEK_CUR);
        }
    }
    sizeChunk min = {2000000000, 2000000000, 2000000000}, max = {0, 0, 0};
    for (size_t i = 0; i < transform.size(); i++) {
        (transform[i].x < min.x) ? min.x = transform[i].x : 0;
        (transform[i].y < min.y) ? min.y = transform[i].y : 0;
        (transform[i].z < min.z) ? min.z = transform[i].z : 0;
    }
    for (size_t i = 0; i < transform.size(); i++) {
        transform[i].x -= min.x;
        transform[i].y -= min.y;
        transform[i].z -= min.z;
        (transform[i].x > max.x) ? max.x = transform[i].x : 0;
        (transform[i].y > max.y) ? max.y = transform[i].y : 0;
        (transform[i].z > max.z) ? max.z = transform[i].z : 0;
    }

    std::vector<Voxel> voxels = std::vector<Voxel>((max.x + sizeX) * (max.y + sizeY) * (max.z + sizeZ), {SDF_LIMIT, 0, 0, 0});
    for (size_t i = 0; i < fullVoxels.size(); i++) {
        for (size_t j = 0; j < fullVoxels[i].size(); j++) {
            int moveX = 0, moveY = 0, moveZ = 0;
            if (transform.size() > i) {
                moveX = transform[i].x;
                moveY = transform[i].y;
                moveZ = transform[i].z;
            }
            int x = moveX + (j % sizeX);
            int y = moveY + ((j / sizeX) % sizeY);
            int z = moveZ + ((j / sizeX) / sizeY);
            int pos = x + (y * (max.x + sizeX)) + (z * (max.x + sizeX) * (max.y + sizeY));
            voxels[pos] = fullVoxels[i][j];
        }
    }

    for (size_t i = 0; i < fullVoxels.size(); i++) {
        fullVoxels[i].clear();
    }
    fullVoxels.clear();
    transform.clear();

    return voxelDataToChunks(voxels, max.x + sizeX, max.y + sizeY, max.z + sizeZ, palette);
}

std::vector<Chunk*> VoxelRenderer::voxelDataToChunks (std::vector<Voxel> voxels, int sizeX, int sizeY, int sizeZ, std::vector<unsigned int> colors)
{
    std::vector<Chunk*> chunks;
    for (int z = 0;z < sizeZ;z += CHUNK_SIZE) {
        for (int y = 0;y < sizeY;y += CHUNK_SIZE) {
            for (int x = 0;x < sizeX;x += CHUNK_SIZE) {
                Chunk* chunk = new Chunk();
                chunks.push_back(chunk);
            }
        }
    }
    for (int z = 0;z < sizeZ;z++) {
        for (int y = 0;y < sizeY;y++) {
            for (int x = 0;x < sizeX;x++) {
                Voxel voxel = voxels[x + (y * sizeX) + (z * sizeX * sizeY)];
                int pos = (x / CHUNK_SIZE) + ((y / CHUNK_SIZE) * (sizeX / CHUNK_SIZE)) + ((z / CHUNK_SIZE) * (sizeX / CHUNK_SIZE) * (sizeY / CHUNK_SIZE));
                chunks[pos]->setVoxel(x % CHUNK_SIZE, y % CHUNK_SIZE, z % CHUNK_SIZE, voxel);
            }
        }
    }
    for (auto chunk : chunks) {
        chunk->setPalette(colors);
        chunk->loadData();
        chunk->updateSdf(_computeShader, _sun_tansformation);
        chunk->updateShadows(_computeShader, _sun_tansformation);
    }
    return chunks;
}
