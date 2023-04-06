#include "VoxelRenderer.hpp"
#include <unistd.h>

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define GRID_SIZE 100

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

    _vertices = {};
    _faces = {};
    _color_buffer = {};


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


    //                 // 1,
    //                 // 1,
    //                 // 1,
    //                 // 1

    //             };
    //         }
    //     }
    // }

    _chunk = loadVox("assets/untitled.vox");

    glGenTextures(1, &_chunk._textureShade);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, _chunk._textureShade);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, _chunk.sizeX, _chunk.sizeY, _chunk.sizeZ, 0, GL_RED, GL_FLOAT, NULL);
    glBindImageTexture(0, _chunk._textureShade, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32F);

    glGenTextures(1, &_chunk._textureColor);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, _chunk._textureColor);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, _chunk.sizeX, _chunk.sizeY, _chunk.sizeZ, 0, GL_RGBA, GL_FLOAT, _chunk.voxels.data());
    updateShadows();
}

void VoxelRenderer::initCamera()
{

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

    _sun_tansformation = glm::mat4(1.0f);
}

void VoxelRenderer::draw ()
{
    glUseProgram(_shaderProgram);
    glBindVertexArray(_VAO);

    glUniform1i(glGetUniformLocation(_shaderProgram, "voxelTexture"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, _chunk._textureColor);

    glUniform1i(glGetUniformLocation(_shaderProgram, "shaderTexture"), 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, _chunk._textureShade);

    glUniformMatrix4fv(glGetUniformLocation(_shaderProgram, "MVP"), 1, GL_FALSE, &_proj[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(_shaderProgram, "sun_transformation"), 1, GL_FALSE, &_sun_tansformation[0][0]);
    glUniform3f(glGetUniformLocation(_shaderProgram, "camera_position"), _camera_position.x, _camera_position.y, _camera_position.z);
    glUniform3f(glGetUniformLocation(_shaderProgram, "camera_direction"), _camera_direction.x, _camera_direction.y, _camera_direction.z);
    glUniform3f(glGetUniformLocation(_shaderProgram, "texture_size"), _chunk.sizeX, _chunk.sizeY, _chunk.sizeZ);

    glDrawArrays(GL_TRIANGLES, 0, 6);


    glfwGetCursorPos(_window, &_last_x, &_last_y);
    glfwPollEvents();
    updateCamera();
    moveSun();

}

void VoxelRenderer::updateShadows()
{
    glUseProgram(_computeShader);

    glUniform1i(glGetUniformLocation(_computeShader, "inputTexture"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, _chunk._textureColor);

    glUniform1i(glGetUniformLocation(_computeShader, "outputTexture"), 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, _chunk._textureShade);

    glUniformMatrix4fv(glGetUniformLocation(_computeShader, "sun_transformation"), 1, GL_FALSE, &_sun_tansformation[0][0]);


    glDispatchCompute(_chunk.sizeX , _chunk.sizeY , _chunk.sizeZ);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // glUniform1i(glGetUniformLocation(_computeShader, "outputTexture"), 0);
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_3D, _chunk._textureShade);
    std::vector<float> textureData = std::vector<float>(_chunk.sizeX * _chunk.sizeY * _chunk.sizeZ);
    glGetTexImage(GL_TEXTURE_3D, 0, GL_RED, GL_FLOAT, textureData.data());

    for (auto i : textureData) {
        std::cout << i << std::endl;
    }
}


void VoxelRenderer::moveSun ()
{
    if (glfwGetKey(_window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        _sun_tansformation = glm::rotate(_sun_tansformation, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    if (glfwGetKey(_window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        _sun_tansformation = glm::rotate(_sun_tansformation, glm::radians(-1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    if (glfwGetKey(_window, GLFW_KEY_UP) == GLFW_PRESS) {
        _sun_tansformation = glm::rotate(_sun_tansformation, glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    }
    if (glfwGetKey(_window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        _sun_tansformation = glm::rotate(_sun_tansformation, glm::radians(-1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    }
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

    if (strncmp(chunkID, "MAIN", 4) != 0) {
        printf("Error: File %s is not a valid .vox file\n", path);
    }

    Chunk chunk;
    std::vector<Voxel> palette;
    palette.push_back({0, 0, 0, 0});
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
            for (int i = 0; i < chunkContentBytes / 4; i += 1) {
                unsigned char r, g, b, a;
                fread(&r, 1, 1, file);
                fread(&g, 1, 1, file);
                fread(&b, 1, 1, file);
                fread(&a, 1, 1, file);

                if (r == 255 && g == 255 && b == 255 && a == 255) {
                    a = 100;
                    r = 111;
                }

                palette.push_back({r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f});
            }
            break;
        } else {
            fseek(file, chunkContentBytes + chunkChildrenBytes, SEEK_CUR);
        }
    }
    if (palette.size() != 257) {
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
        for (int i = 0; i < 255; i++) {
            palette.push_back({ (uint8_t)(colors[i] >> 24) / 255.0f, (uint8_t)(colors[i] >> 16) /  255.0f, (uint8_t)(colors[i] >> 8) /  255.0f, 1});
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
