#include "Camera.h"
#include <cmath>

// pozitie fixa a camerei
float posX = 0.0f, posY = 10.0f, posZ = 50.0f;

// unghiurile de rotatie
float yaw = -90.0f;
float pitch = 0.0f;

// directie
float dirX, dirY, dirZ;

float fov = 90.0f;

void initCamera() {
    // calcul directie initiala
    dirX = cos(yaw * 3.1415 / 180.0) * cos(pitch * 3.1415 / 180.0);
    dirY = sin(pitch * 3.1415 / 180.0);
    dirZ = sin(yaw * 3.1415 / 180.0) * cos(pitch * 3.1415 / 180.0);
}

void handleMouseMove(int x, int y) {
    // centrul ferestrei
    int centerX = 400;
    int centerY = 300;

    // daca mouseul este deja in centru nu se intampla nimic
    if (x == centerX && y == centerY) return;

    // calculare diferenta fata de centru
    float xOffset = (float)x - centerX;
    float yOffset = (float)centerY - y; 

    float sensitivity = 0.15f;
    yaw += xOffset * sensitivity;
    pitch += yOffset * sensitivity;

    // limitare la 89 de grade pentru evitare gimbal lock
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    if (yaw > 360.0f) yaw -= 360.0f;
    if (yaw < 0.0f) yaw += 360.0f;

    // recalculare directie privire
    dirX = cos(yaw * 3.1415 / 180.0) * cos(pitch * 3.1415 / 180.0);
    dirY = sin(pitch * 3.1415 / 180.0);
    dirZ = sin(yaw * 3.1415 / 180.0) * cos(pitch * 3.1415 / 180.0);

    // resetare mouse in centru
    glutWarpPointer(centerX, centerY);

    glutPostRedisplay();
}

void handleZoom(int direction) {
    if (direction > 0) fov -= 2.0f; // zoom in
    else fov += 2.0f;              // zoom out

    // limite
    if (fov < 1.0f) fov = 1.0f;
    if (fov > 90.0f) fov = 90.0f;
}

void updateCamera() {
    gluLookAt(posX, posY, posZ,
        posX + dirX, posY + dirY, posZ + dirZ,
        0.0f, 1.0f, 0.0f);
}

void handleKeyboard(unsigned char key, int x, int y) {
    float speed = 1.5f;

    float horizontalLength = sqrt(dirX * dirX + dirZ * dirZ);
    float fwdX = dirX / horizontalLength;
    float fwdZ = dirZ / horizontalLength;

    float rightX = -fwdZ;
    float rightZ = fwdX;

    switch (key) {
    case 'w': case 'W': 
        posX += fwdX * speed;
        posZ += fwdZ * speed;
        break;
    case 's': case 'S': 
        posX -= fwdX * speed;
        posZ -= fwdZ * speed;
        break;
    case 'a': case 'A': 
        posX -= rightX * speed;
        posZ -= rightZ * speed;
        break;
    case 'd': case 'D': 
        posX += rightX * speed;
        posZ += rightZ * speed;
        break;
    case 'q': case 'Q': 
        posY -= speed;
        break;
    case 'e': case 'E':
        posY += speed;
        break;
    }

    glutPostRedisplay();
}