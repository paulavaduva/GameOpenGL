#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <freeglut.h>

// Ini?ializeaz? parametrii camerei
void initCamera();

// Aplic? transform?rile de vizualizare (gluLookAt)
void updateCamera();

// Gestioneaz? mi?carea mouse-ului pentru rota?ie
void handleMouseMove(int x, int y);

// Adaug? asta la finalul fi?ierului Camera.h, înainte de #endif
extern float fov;
void handleZoom(int direction);

#endif