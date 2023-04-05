#include "VoxelRenderer.hpp"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define GRID_SIZE 100

VoxelRenderer::~VoxelRenderer ()
{
    // glDeleteBuffers(1, &_vertexbuffer);
    // glDeleteBuffers(1, &_colorbuffer);
    // glDeleteBuffers(1, &_indexbuffer);
    // glDeleteBuffers(1, &_normalbuffer);
    glDeleteProgram(_shaderProgram);
    glDeleteVertexArrays(1, &_VAO);
    std::cout << LOG_VOXEL("VoxelRenderer destroyed");
}

VoxelRenderer::VoxelRenderer()
{
    _window = NULL;
    _VAO = 0;
    _shaderProgram = 0;
    // _vertexbuffer = 0;
    // _indexbuffer = 0;
    // _colorbuffer = 0;
    // _normalbuffer = 0;
    _matrix = 0;
    _camera_position = glm::vec3(0, 0, 0);
    _camera_direction = glm::vec3(0, 0, -1);
    _up_vector = glm::vec3(0, 1, 0);
    // _voxelMap.insert({std::make_tuple(1, 2, 3), {255, 0.5f}});
}

void VoxelRenderer::init(GLFWwindow* window)
{
    std::cout << LOG_VOXEL("Init VoxelRenderer");
    _window = window;
    glGenVertexArrays(1, &_VAO);
    _shaderProgram = LoadShaders("shader/vertexShader.glsl", "shader/fragmentShader.glsl");

    initCamera();

    _vertices = {};
    _faces = {};
    _color_buffer = {};

    // Voxel ***voxels = new Voxel**[GRID_SIZE];
    // for (int i = 0; i < GRID_SIZE; i++) {
    //     voxels[i] = new Voxel*[GRID_SIZE];
    //     for (int j = 0; j < GRID_SIZE; j++) {
    //         voxels[i][j] = new Voxel[GRID_SIZE];
    //     }
    // }

    // Chunk chunk;
    // chunk.sizeX = GRID_SIZE;
    // chunk.sizeY = GRID_SIZE;
    // chunk.sizeZ = GRID_SIZE;

    // chunk.voxels = std::vector<Voxel>(GRID_SIZE * GRID_SIZE * GRID_SIZE);

    // for (int x = 0; x < GRID_SIZE; x++) {
    //     for (int y = 0; y < GRID_SIZE; y++) {
    //         for (int z = 0; z < GRID_SIZE; z++) {
    //             chunk.voxels[(z * GRID_SIZE * GRID_SIZE) + (y * GRID_SIZE) + x] = {
    //                 // static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
    //                 // static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
    //                 // static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
    //                 // // static_cast<float>(rand()) / static_cast<float>(RAND_MAX)
    //                 // (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) > 0.5) ? static_cast<float>(rand()) / static_cast<float>(RAND_MAX) : 0.0f
    //                 // // (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) > 0.5) ? 1.0f : 0.0f

    //                 (float) ((float) x / GRID_SIZE),
    //                 (float) ((float) y / GRID_SIZE),
    //                 (float) ((float) z / GRID_SIZE),
    //                 (sqrt(pow(x - GRID_SIZE / 2, 2) + pow(y - GRID_SIZE / 2, 2) + pow(z - GRID_SIZE / 2, 2)) < GRID_SIZE / 2) ? 1.0f : 0.0f
    //                 // static_cast<float>(rand()) / static_cast<float>(RAND_MAX)

    //             };
    //         }
    //     }
    // }

    Chunk chunk = loadVox("assets/Temple.vox");



    glGenTextures(1, &_textureID);
    glBindTexture(GL_TEXTURE_3D, _textureID);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, chunk.sizeX, chunk.sizeY, chunk.sizeZ, 0, GL_RGBA, GL_FLOAT, chunk.voxels.data());
}

void VoxelRenderer::initCamera()
{
    _matrix = glGetUniformLocation(_shaderProgram, "MVP");

    glm::mat4 _projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);

    _camera_position = glm::vec3(2.0f, 2.0f, 5.0f);  // Initial camera position
	_camera_direction = glm::vec3(-0.5f, -0.5f, -1.0f);  // Initial camera direction
	_up_vector = glm::vec3(0.0f, 1.0f, 0.0f);  // Up vector (usually (0,1,0))

    _view = glm::lookAt(
        _camera_position, // Camera is at (4,3,3), in World Space
        _camera_position + _camera_direction, // and looks at the origin
        _up_vector  // Head is up (set to 0,-1,0 to look upside-down)
    );

    glm::mat4 Model = glm::mat4(1.0f);

    _proj = _projection * _view * Model;

    _camera_position = glm::vec3(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 0.0f);  // Initial camera position
	_camera_direction = glm::vec3(0.0f, 0.0f, 1.0f);  // Initial camera direction
	_up_vector = glm::vec3(0.0f, 1.0f, 0.0f);  // Up vector (usually (0,1,0))
}

void VoxelRenderer::draw ()
{
    glUseProgram(_shaderProgram);
    glBindVertexArray(_VAO);
    glBindTextureUnit(0, _textureID);

    glUniformMatrix4fv(_matrix, 1, GL_FALSE, &_proj[0][0]);
    glUniform3f(glGetUniformLocation(_shaderProgram, "camera_position"), _camera_position.x, _camera_position.y, _camera_position.z);
    glUniform3f(glGetUniformLocation(_shaderProgram, "camera_direction"), _camera_direction.x, _camera_direction.y, _camera_direction.z);
    glUniform3f(glGetUniformLocation(_shaderProgram, "texture_size"), _chunk.sizeX, _chunk.sizeY, _chunk.sizeZ);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glfwGetCursorPos(_window, &_last_x, &_last_y);
    glfwPollEvents();
    updateCamera();

}

void VoxelRenderer::updateCamera ()
{
    float camera_speed = 10.0f;  // Adjust as needed

    double last_mouse_x, last_mouse_y;
    glfwGetCursorPos(_window, &last_mouse_x, &last_mouse_y);  // Get initial mouse position


    // Calculate mouse movement delta
    double delta_x = last_mouse_x - _last_x;
    double delta_y = _last_y - last_mouse_y;  // Inverted y-axis


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
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    _camera_direction = glm::normalize(direction);


    glm::vec3 up_vector = glm::vec3(0.0f, 1.0f, 0.0f);

    if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS) {
        _camera_position += camera_speed * _camera_direction;
    }
    if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS) {
        _camera_position -= camera_speed * _camera_direction;
    }
    if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS) {
		_camera_position -= glm::normalize(glm::cross(_camera_direction, up_vector)) * camera_speed;
    }
    if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS) {
		_camera_position += glm::normalize(glm::cross(_camera_direction, up_vector)) * camera_speed;
    }
	if (glfwGetKey(_window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		_camera_position += up_vector * camera_speed;
	}
	if (glfwGetKey(_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		_camera_position -= up_vector * camera_speed;
	}

    _view = glm::lookAt(_camera_position, _camera_position + _camera_direction, _up_vector);
    glm::mat4 Model = glm::mat4(1.0f);
	_projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
    _proj = _projection * _view * Model;
}

Chunk VoxelRenderer::loadVox(const char *path)
{
    //load a .vox file
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        printf("Error: Could not open file %s\n", path);
    }

    //read the header
    char header[4];
    fread(header, 4, 1, file);
    if (strncmp(header, "VOX ", 4) != 0) {
        printf("Error: File %s is not a valid .vox file\n", path);
    }

    //read the version
    int version;
    fread(&version, 4, 1, file);
    if (version != 150) {
        printf("Error: File %s is not a valid .vox file\n", path);
    }

    // read the main chunk
    char chunkID[4];
    int chunkContentBytes;
    int chunkChildrenBytes;
    fread(chunkID, 4, 1, file);
    fread(&chunkContentBytes, 4, 1, file);
    fread(&chunkChildrenBytes, 4, 1, file);

    int tmp = chunkChildrenBytes;

    // std::cout << "chunkID: " << chunkChildrenBytes << std::endl;

    if (strncmp(chunkID, "MAIN", 4) != 0) {
        printf("Error: File %s is not a valid .vox file\n", path);
    }


    Chunk chunk;
    std::vector<Voxel> palette;
    int numVoxels;

    //get current position in file
    long pos = ftell(file);
    for (int i = 0; i < chunkChildrenBytes; i++) {
        char chunkID[4];
        int chunkContentBytes;
        int chunkChildrenBytes;
        fread(chunkID, 4, 1, file);
        fread(&chunkContentBytes, 4, 1, file);
        fread(&chunkChildrenBytes, 4, 1, file);
        if (strncmp(chunkID, "RGBA", 4) == 0) {
            //read the palette
            printf("chunkContentBytes: %d chunkChildrenBytes: %d chunkID: %s\n", chunkContentBytes, chunkChildrenBytes, chunkID);
            for (int i = 0; i < chunkContentBytes / 4; i += 1) {
                unsigned char r, g, b, a;
                fread(&r, 1, 1, file);
                fread(&g, 1, 1, file);
                fread(&b, 1, 1, file);
                fread(&a, 1, 1, file);
                palette.push_back({r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f});
            }
            break;
        } else {
            fseek(file, chunkContentBytes + chunkChildrenBytes, SEEK_CUR);
        }
    }
    fseek(file, pos, SEEK_SET);

    for (int i = 0; i < chunkChildrenBytes; i++) {
        char chunkID[4];
        int chunkContentBytes;
        int chunkChildrenBytes;
        fread(chunkID, 4, 1, file);
        fread(&chunkContentBytes, 4, 1, file);
        fread(&chunkChildrenBytes, 4, 1, file);

        if (strncmp(chunkID, "SIZE", 4) == 0) {
            fread(&chunk.sizeX, 4, 1, file);
            fread(&chunk.sizeY, 4, 1, file);
            fread(&chunk.sizeZ, 4, 1, file);
            chunk.voxels = std::vector<Voxel>(chunk.sizeX * chunk.sizeY * chunk.sizeZ);
            numVoxels = chunk.sizeX * chunk.sizeY * chunk.sizeZ;
            for (int i = 0; i < numVoxels; i++) {
                chunk.voxels[i].r = 0;
                chunk.voxels[i].g = 0;
                chunk.voxels[i].b = 0;
                chunk.voxels[i].w = 0;
            }
        } else if (strncmp(chunkID, "XYZI", 4) == 0) {
            int numVoxels;
            fread(&numVoxels, 4, 1, file);
            for (int i = 0; i < numVoxels; i++) {
                unsigned char x, y, z, colorIndex;
                fread(&x, 1, 1, file);
                fread(&y, 1, 1, file);
                fread(&z, 1, 1, file);
                fread(&colorIndex, 1, 1, file);
                chunk.voxels[x + z * chunk.sizeX + y * chunk.sizeX * chunk.sizeY].r = palette[colorIndex].r;
                chunk.voxels[x + z * chunk.sizeX + y * chunk.sizeX * chunk.sizeY].g = palette[colorIndex].g;
                chunk.voxels[x + z * chunk.sizeX + y * chunk.sizeX * chunk.sizeY].b = palette[colorIndex].b;
                chunk.voxels[x + z * chunk.sizeX + y * chunk.sizeX * chunk.sizeY].w = palette[colorIndex].w;
            }
        } else {
            fseek(file, chunkContentBytes + chunkChildrenBytes, SEEK_CUR);
        }
    }

    return chunk;
}
