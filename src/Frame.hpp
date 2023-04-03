/*
** EPITECH PROJECT, 2023
** voxel opengl
** File description:
** Frame
*/

#pragma once
#include "VoxelRenderer.hpp"
#include "TextRenderer.hpp"
#include <chrono>


#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

class Frame {
    public:
        Frame();
        ~Frame();
        void init();
        void run();
        void input();

    private:
        GLFWwindow* _window;
        TextRenderer _textRenderer;
        VoxelRenderer _voxelRenderer;

};
