#include "Camera.h"
#include "Scene.h"
#include <cmath>

// pozitie fixa a camerei
float posX = 0.0f, posY = 10.0f, posZ = 50.0f;

// unghiurile de rotatie
float yaw = -90.0f;
float pitch = 0.0f;

// directie
float dirX, dirY, dirZ;

float fov = 90.0f;

float carX = 0.0f, carZ = 0.0f, carAngle = 0.0f;
bool isThirdPerson = false;

bool specialKeys[256] = { false };

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
    if (isThirdPerson)
    {
        float rad = carAngle * 3.14159f / 180.0f;
        float camDist = 25.0f;
        float camHeight = 10.0f;

        // calculam pozitia camerei relativ la masina
        float cX = carX - sin(rad) * camDist;
        float cZ = carZ - cos(rad) * camDist;
        float cY = getTerrainHeight(cX, cZ) + camHeight;

        gluLookAt(cX, cY, cZ,
            carX, getTerrainHeight(carX, carZ) + 2.0f, carZ,
            0.0f, 1.0f, 0.0f);
    }
    else {

        gluLookAt(posX, posY, posZ,
            posX + dirX, posY + dirY, posZ + dirZ,
            0.0f, 1.0f, 0.0f);
    }
}

void handleKeyboard(unsigned char key, int x, int y) {
    if (key == 'c' || key == 'C')
    {
        isThirdPerson = !isThirdPerson;
    }
    
    if (!isThirdPerson)
    {
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
    }
    glutPostRedisplay();
}

//void handleSpecialKeyboard(int key, int x, int y) {
//    float carSpeed = 1.2f;
//    float rotationSpeed = 3.0f;
//    float rad = carAngle * 3.14159f / 180.0f;
//
//    switch (key) {
//    case GLUT_KEY_UP:
//        carX += sin(rad) * carSpeed;
//        carZ += cos(rad) * carSpeed;
//        break;
//    case GLUT_KEY_DOWN:
//        carX -= sin(rad) * carSpeed;
//        carZ -= cos(rad) * carSpeed;
//        break;
//    case GLUT_KEY_LEFT:
//        carAngle += rotationSpeed;
//        break;
//    case GLUT_KEY_RIGHT:
//        carAngle -= rotationSpeed;
//        break;
//    }
//    glutPostRedisplay();
//}

void updateCarLogic() {
    float carSpeed = 1.5f;      // viteza de deplasare
    float rotationSpeed = 2.5f; // viteza de rotatie
    float rad = carAngle * 3.14159f / 180.0f;

    float nextX = carX;
    float nextZ = carZ;


    if (specialKeys[GLUT_KEY_UP]) {
        nextX += sin(rad) * carSpeed;
        nextZ += cos(rad) * carSpeed;
    }
    if (specialKeys[GLUT_KEY_DOWN]) {
        nextX -= sin(rad) * carSpeed;
        nextZ -= cos(rad) * carSpeed;
    }
    if (specialKeys[GLUT_KEY_LEFT]) {
        carAngle += rotationSpeed;
    }
    if (specialKeys[GLUT_KEY_RIGHT]) {
        carAngle -= rotationSpeed;
    }

    if (!checkCollision(nextX, nextZ)) {
        carX = nextX;
        carZ = nextZ;
    }

    if (isThirdPerson) glutPostRedisplay();
}

void handleSpecialKeyboard(int key, int x, int y) {
    if (key >= 0 && key < 256) specialKeys[key] = true;
}

void handleSpecialUp(int key, int x, int y) {
    if (key >= 0 && key < 256) specialKeys[key] = false;
}