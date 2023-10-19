/*
** EPITECH PROJECT, 2023
** arcade
** File description:
** logger
*/

#pragma once

#include "colors.hpp"

#define LOG(COLOR, WHAT, MSG) "[" << COLOR << #WHAT << RESET << "] " << MSG << std::endl

#define LOG_FRAME(MSG) LOG(CYAN, frame, MSG)
#define LOG_ERR_FRAME(MSG) LOG(RED, frame, MSG)

#define LOG_TEXT(MSG) LOG(GREEN, text, MSG)
#define LOG_ERR_TEXT(MSG) LOG(RED, text, MSG)

#define LOG_VOXEL(MSG) LOG(BLUE, voxel, MSG)
#define LOG_ERR_VOXEL(MSG) LOG(RED, voxel, MSG)
