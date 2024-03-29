#include "Engine.hpp"

Engine::Engine()
{
    _window = nullptr;
}

Engine::~Engine()
{
    glfwDestroyWindow(_window);
    glfwTerminate();
    std::cout << LOG_FRAME("Engine destructor");
}

void Engine::init(int chunkSize)
{
    _chunkSize = chunkSize;
    std::cout << LOG_FRAME("Initializing GLFW");
    if (!glfwInit()) {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    _window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Generator", NULL, NULL);
    if(_window == NULL) {
        fprintf(stderr, "Failed to open GLFW window.\n");
        getchar();
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(_window);

    std::cout << LOG_FRAME("Initializing GLEW");
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
    }
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    _voxelRenderer = std::unique_ptr<VoxelRenderer>(new VoxelRenderer());
    _textRenderer = std::unique_ptr<TextRenderer>(new TextRenderer());
    _voxelRenderer->init(_window, _chunkSize);
    _textRenderer->init(_window);
    _voxLoader = std::unique_ptr<VoxLoader>(new VoxLoader(_voxelRenderer.get()));
}

void Engine::run()
{
    std::cout << LOG_FRAME("Running");
    auto currentFrameTime = std::chrono::high_resolution_clock::now();
    int frameCount = 0;

    GLuint64 start, end;

    GLuint query[2];
    glGenQueries(2, query);

    textInfo_t Engine = {"Engine: ", glm::vec3(255.0f, 255.0f, 255.0f), glm::vec2(10.0f, 20.0f), 0.5f};
    textInfo_t renderTime = {"Render time: ", glm::vec3(255.0f, 255.0f, 255.0f), glm::vec2(10.0f, -20.0f), 0.5f};

    while (!glfwWindowShouldClose(_window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.4f, 0.4f, 0.4f, 0.0f);

        glQueryCounter(query[0], GL_TIMESTAMP);
        _voxelRenderer->draw();
        _textRenderer->draw();
        _voxelRenderer->eventHandler();
        glQueryCounter(query[1], GL_TIMESTAMP);

        glGetQueryObjectui64v(query[0], GL_QUERY_RESULT, &start);
        glGetQueryObjectui64v(query[1], GL_QUERY_RESULT, &end);

        renderTime.text = "Render time: " + std::to_string((end - start) / 1000000.0) + " ms";

        frameCount++;
        if (std::chrono::high_resolution_clock::now() - currentFrameTime >= std::chrono::seconds(1)) {
            currentFrameTime = std::chrono::high_resolution_clock::now();
            Engine.text = "Engine: " + std::to_string(frameCount);
            frameCount = 0;
        }
        _textRenderer->renderText(renderTime);
        _textRenderer->renderText(Engine);
        glfwSwapBuffers(_window);
        input();
    }
}

void Engine::input()
{
    if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(_window, true);
}

void Engine::loadVoxFile(std::string path)
{
    _voxLoader->loadVox(path.c_str());
}
