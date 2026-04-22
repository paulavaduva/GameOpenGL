#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <freeglut.h>

void initCamera();

void updateCamera();

void handleMouseMove(int x, int y);

extern float fov;
void handleZoom(int direction);

void handleKeyboard(unsigned char key, int x, int y);
extern float posX, posY, posZ;

#endif