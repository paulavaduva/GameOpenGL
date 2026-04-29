#ifndef SCENE_H
#define SCENE_H

#include <freeglut.h> 
#include <vector>

void initTextures();
void drawSkybox(float size);
void drawGround(float size);
void drawRelief(float size);

void drawCircuit(float innerRadius, float outerRadius, int segments);
float getTerrainHeight(float x, float z);

void drawTree(float x, float z);
void drawBuilding(float x, float z, float width, float height, float depth);
void drawStaticObjects();
bool isPointOnCircuit(float x, float z, float innerR, float outerR);

void calculateShadowMatrix(float shadowMat[16], float groundPlane[4], float lightPos[4]);

void drawLightPole(float x, float z);

void drawCar(float x, float z, float angle);

struct StaticObj {
    float x, z;
    int type; // 0=copac, 1=bloc mare, 2=bloc mic
};

extern std::vector<StaticObj> worldObjects;

bool checkCollision(float x, float z);

struct MovingObj {
    float x, z, angle;
    float speed;
    int changeDirTimer;
};

void updateRandomObjects();
void drawMovingObjects();

struct CircuitCarObj {
    float angle;
    float speed;
    float radius; 
    float x, z;
};

void updateCircuitCars();
void drawCircuitCars();

#endif