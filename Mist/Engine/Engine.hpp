#pragma once
#include "VoxelRenderer.hpp"
#include "TextRenderer.hpp"
#include "VoxLoader.hpp"
#include <chrono>
#include <memory>


#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

class Engine {
    public:
        Engine();
        ~Engine();
        void init(int chunkSize, int debug);
        void run();
        void input();
        void loadVoxFile(std::string path);

    private:
        GLFWwindow* _window;
        std::unique_ptr<TextRenderer> _textRenderer;
        std::unique_ptr<VoxelRenderer> _voxelRenderer;
        std::unique_ptr<VoxLoader> _voxLoader;
        int _chunkSize;
};
