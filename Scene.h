#ifndef SCENE_H
#define SCENE_H

#include <freeglut.h> 

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

#endif