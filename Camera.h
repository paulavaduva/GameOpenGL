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

void handleSpecialKeyboard(int key, int x, int y);
extern float carX, carZ, carAngle;
extern bool isThirdPerson;
void handleSpecialUp(int key, int x, int y);
void updateCarLogic();

#endif