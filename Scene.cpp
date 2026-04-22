#include "Scene.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <cmath>
#include <vector>

using namespace std;

unsigned int texGround, texSky;
unsigned int texRoad;
unsigned int texBuilding;

unsigned int loadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;

    stbi_set_flip_vertically_on_load(true); // intoarce corect imaginea

    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

        glBindTexture(GL_TEXTURE_2D, textureID);

        gluBuild2DMipmaps(GL_TEXTURE_2D, format, width, height, format, GL_UNSIGNED_BYTE, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        stbi_image_free(data);
        cout << "Textura incarcata cu succes: " << path << endl;
    }
    else {
        cout << "Eroare incarcare textura: " << path << endl;
        textureID = 0; 
    }
    return textureID;
}

void initTextures() {
    glEnable(GL_TEXTURE_2D);
    texGround = loadTexture("ground.jpg"); 
    texSky = loadTexture("sky2.jpg");      
    texRoad = loadTexture("road.jpg");
    texBuilding = loadTexture("windows.jpg");
}

void drawGround(float size) {
    glBindTexture(GL_TEXTURE_2D, texGround);
    //glNormal3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, -1.0f, -size);
    glTexCoord2f(10.0f, 0.0f); glVertex3f(size, -1.0f, -size);
    glTexCoord2f(10.0f, 10.0f); glVertex3f(size, -1.0f, size);
    glTexCoord2f(0.0f, 10.0f); glVertex3f(-size, -1.0f, size);
    glEnd();
}

void drawSkybox(float size) {
    glBindTexture(GL_TEXTURE_2D, texSky);
    glBegin(GL_QUADS);
    // spate
    glTexCoord2f(0, 0); glVertex3f(-size, -1.0f, -size);
    glTexCoord2f(1, 0); glVertex3f(size, -1.0f, -size);
    glTexCoord2f(1, 1); glVertex3f(size, size, -size);
    glTexCoord2f(0, 1); glVertex3f(-size, size, -size);

    // fata
    glTexCoord2f(0, 0); glVertex3f(size, -1.0f, size);
    glTexCoord2f(1, 0); glVertex3f(-size, -1.0f, size);
    glTexCoord2f(1, 1); glVertex3f(-size, size, size);
    glTexCoord2f(0, 1); glVertex3f(size, size, size);

    // dreapta
    glTexCoord2f(0, 0); glVertex3f(size, -1.0f, -size);
    glTexCoord2f(1, 0); glVertex3f(size, -1.0f, size);
    glTexCoord2f(1, 1); glVertex3f(size, size, size);
    glTexCoord2f(0, 1); glVertex3f(size, size, -size);

    // stanga
    glTexCoord2f(0, 0); glVertex3f(-size, -1.0f, size);
    glTexCoord2f(1, 0); glVertex3f(-size, -1.0f, -size);
    glTexCoord2f(1, 1); glVertex3f(-size, size, -size);
    glTexCoord2f(0, 1); glVertex3f(-size, size, size);

    // sus
    glTexCoord2f(0, 0); glVertex3f(-size, size, -size);
    glTexCoord2f(1, 0); glVertex3f(size, size, -size);
    glTexCoord2f(1, 1); glVertex3f(size, size, size);
    glTexCoord2f(0, 1); glVertex3f(-size, size, size);

    // jos
    glTexCoord2f(0, 0); glVertex3f(-size, -1.1f, -size);
    glTexCoord2f(1, 0); glVertex3f(size, -1.1f, -size);
    glTexCoord2f(1, 1); glVertex3f(size, -1.1f, size);
    glTexCoord2f(0, 1); glVertex3f(-size, -1.1f, size);

    glEnd();
}

struct Hill {
    float x, z, radius, height;
};

void drawRelief(float size) {
    glBindTexture(GL_TEXTURE_2D, texGround);
    glColor3f(1.0f, 1.0f, 1.0f);

    // PASUL 1: Cre?tem rezolu?ia la 120 pentru ca dealurile s? fie mai fine
    int resolution = 150;
    float step = (size * 2) / resolution;

    for (int i = 0; i < resolution; i++) {
        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= resolution; j++) {
            for (int k = 0; k < 2; k++) {
                float x = -size + (i + k) * step;
                float z = -size + j * step;
                float y = getTerrainHeight(x, z);

                // PASUL 2: Calcul?m umbra dealului (Normalele)
                // Calcul?m panta dealului în acel punct
                float hL = getTerrainHeight(x - 0.1f, z);
                float hR = getTerrainHeight(x + 0.1f, z);
                float hD = getTerrainHeight(x, z - 0.1f);
                float hU = getTerrainHeight(x, z + 0.1f);
                // Normala aproximativ?: (-pantaX, 1, -pantaZ)
                glNormal3f(hL - hR, 1.0f, hD - hU);

                glTexCoord2f(x * 0.05f, z * 0.05f);
                glVertex3f(x, y, z);
            }
        }
        glEnd();
    }
}
// Func?ie pentru stâlpi de iluminat
void drawLightPole(float x, float z) {
    float y = getTerrainHeight(x, z);
    bool isShadow = !glIsEnabled(GL_LIGHTING);

    glPushMatrix();
    glTranslatef(x, y, z);

    // Stâlpul (nu are textur?)
    glDisable(GL_TEXTURE_2D);
    if (!isShadow) glColor3f(0.2f, 0.2f, 0.22f);

    glPushMatrix();
        glRotatef(-90, 1, 0, 0);
        GLUquadric* pole = gluNewQuadric();
        gluCylinder(pole, 0.4, 0.3, 15.0, 12, 1);
    glPopMatrix();

    // Becul (Sursa de lumin? vizual?)
    if (!isShadow) {
        glTranslatef(0, 15.0, 0);
        glColor3f(1.0f, 1.0f, 0.7f);
        glutSolidSphere(0.8, 12, 12);
    }
    if (!isShadow) glEnable(GL_TEXTURE_2D);
    glPopMatrix();
}

// calculeaza inaltimea in orice punct 
float getTerrainHeight(float x, float z) {
    float y = -1.0f;
    // lista de dealuri
    static vector<Hill> hills = {
        { 0.0f,   -60.0f, 50.0f, 3.0f }, // În?l?ime 10
        { 80.0f,  20.0f,  40.0f, 3.0f }, // În?l?ime 10
        {-70.0f,  50.0f,  45.0f, 3.0f }, // În?l?ime 10
        { 40.0f,  -90.0f, 30.0f, 3.0f }, // În?l?ime 10
        {-100.0f, -30.0f, 60.0f, 3.0f }  // În?l?ime 10
    };

    for (const auto& h : hills) {
        float dist = sqrt(pow(x - h.x, 2) + pow(z - h.z, 2));
        if (dist < h.radius) {
            y += h.height * 0.5f * (1.0f + cos(3.1415f * dist / h.radius));
        }
    }
    return y;
}


void drawCircuit(float innerRadius, float outerRadius, int segments) {
    bool isShadow = !glIsEnabled(GL_LIGHTING);
    if (isShadow) return; // Drumul nu trebuie s? lase umbr?, cum ai cerut

    glBindTexture(GL_TEXTURE_2D, texRoad);
    glColor3f(1.0f, 1.0f, 1.0f);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.0f, -1.0f); // "Tragem" drumul spre camer?

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * 3.14159f * (float)i / (float)segments;
        float stretchZ = 1.5f;

        float xIn = innerRadius * cos(angle);
        float zIn = innerRadius * sin(angle) * stretchZ;
        float yIn = getTerrainHeight(xIn, zIn) + 0.4f; // Ridic?m drumul cu 0.6

        float xOut = outerRadius * cos(angle);
        float zOut = outerRadius * sin(angle) * stretchZ;
        float yOut = getTerrainHeight(xOut, zOut) + 0.4f;

        glTexCoord2f((float)i * 0.5f, 0.0f); glVertex3f(xIn, yIn, zIn);
        glTexCoord2f((float)i * 0.5f, 1.0f); glVertex3f(xOut, yOut, zOut);
    }
    glEnd();
    glDisable(GL_POLYGON_OFFSET_FILL);
}

void drawTree(float x, float z) {
    float y = getTerrainHeight(x, z);

    glDisable(GL_TEXTURE_2D);

    glPushMatrix();
    glTranslatef(x, y, z);

    bool isShadow = !glIsEnabled(GL_LIGHTING);

    // trunchiul (maro)
    if (!isShadow) glColor3f(0.3f, 0.15f, 0.05f);
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    GLUquadric* trunk = gluNewQuadric();
    gluCylinder(trunk, 1.2, 1.0, 7.0, 16, 1);
    glPopMatrix();

    // coroana (verde)
    if (!isShadow) glColor3f(0.0f, 0.35f, 0.0f);
    glPushMatrix();
    glTranslatef(0, 7.0, 0);
    glRotatef(-90, 1, 0, 0);
    glutSolidCone(8.0, 20.0, 16, 1);
    GLUquadric* disk = gluNewQuadric();
    gluDisk(disk, 0, 8.0, 16, 1);
    glPopMatrix();

    glPopMatrix();

    if (!isShadow) {
        glEnable(GL_TEXTURE_2D);
        glColor3f(1.0f, 1.0f, 1.0f);
    }
}

void drawBuilding(float x, float z, float width, float height, float depth) {
    float y = getTerrainHeight(x, z);
    bool isShadow = !glIsEnabled(GL_LIGHTING);

    //float foundation = 20.0f; //

    glPushMatrix();
    glTranslatef(x, y, z); 

    if (!isShadow) 
    {
        glBindTexture(GL_TEXTURE_2D, texBuilding);
        glColor3f(1.0f, 1.0f, 1.0f);
    }

    float w = width / 2.0f;
    float d = depth / 2.0f;
    float h = height;
    float b = isShadow ? 0.0f : -10.0f;

    // texturare
    /*float repeatH = (height + foundation) / 20.0f;
    float repeatW = width / 20.0f;*/

    glBegin(GL_QUADS);
    // fata
    glTexCoord2f(0, 0); glVertex3f(-w, b, d);
    glTexCoord2f(1, 0); glVertex3f(w, b, d);
    glTexCoord2f(1, 1); glVertex3f(w, h, d);
    glTexCoord2f(0, 1); glVertex3f(-w, h, d);

    // spate
    glTexCoord2f(0, 0); glVertex3f(-w, b, -d);
    glTexCoord2f(1, 0); glVertex3f(w, b, -d);
    glTexCoord2f(1, 1); glVertex3f(w, h, -d);
    glTexCoord2f(0, 1); glVertex3f(-w, h, -d);

    // dreapta
    glTexCoord2f(0, 0); glVertex3f(w, b, -d);
    glTexCoord2f(1, 0); glVertex3f(w, b, d);
    glTexCoord2f(1, 1); glVertex3f(w, h, d);
    glTexCoord2f(0, 1); glVertex3f(w, h, -d);

    // stanga
    glTexCoord2f(0, 0); glVertex3f(-w, b, -d);
    glTexCoord2f(1, 0); glVertex3f(-w, b, d);
    glTexCoord2f(1, 1); glVertex3f(-w, h, d);
    glTexCoord2f(0, 1); glVertex3f(-w, h, -d);

    // sus 
    glTexCoord2f(0, 0); glVertex3f(-w, h, -d);
    glTexCoord2f(1, 0); glVertex3f(w, h, -d);
    glTexCoord2f(1, 1); glVertex3f(w, h, d);
    glTexCoord2f(0, 1); glVertex3f(-w, h, d);
    glEnd();

    glPopMatrix();
}

// verifica daca un punct se afla pe drum
bool isPointOnCircuit(float x, float z, float innerR, float outerR) {
    float stretchZ = 1.5f;
    // calculam distanta fata de centru in contextul ovalului
    float dist = sqrt(pow(x, 2) + pow(z / stretchZ, 2));

    float safetyMargin = 15.0f;

    if (dist >= (innerR - safetyMargin) && dist <= (outerR + safetyMargin)) {
        return true; 
    }
    return false;
}

struct StaticObj {
    float x, z;
    int type;
};

void drawStaticObjects() {
    float innerR = 40.0f;
    float outerR = 55.0f;
    static vector<StaticObj> worldObjects;
    static bool generated = false;

    if (!generated) {
        float mapSize = 180.0f;
        float step = 40.0f;

        for (float x = -mapSize; x <= mapSize; x += step) {
            for (float z = -mapSize; z <= mapSize; z += step) {
                float finalX = x + (rand() % 14 - 7);
                float finalZ = z + (rand() % 14 - 7);

                if (!isPointOnCircuit(finalX, finalZ, innerR, outerR)) {
                    StaticObj obj;
                    obj.x = finalX;
                    obj.z = finalZ;
                    // distributie: 40% cladiri mari, 40% mici, 20% copaci
                    int r = rand() % 10;
                    if (r < 2) obj.type = 0;      // copac
                    else if (r < 6) obj.type = 1; // bloc mare
                    else obj.type = 2;            // bloc mic

                    worldObjects.push_back(obj);
                }
            }
        }
        generated = true;
    }

    for (const auto& obj : worldObjects) {
        if (obj.type == 0) drawTree(obj.x, obj.z);
        else if (obj.type == 1) drawBuilding(obj.x, obj.z, 20.0f, 65.0f, 20.0f);
        else drawBuilding(obj.x, obj.z, 15.0f, 25.0f, 15.0f);
    }
}


void calculateShadowMatrix(float shadowMat[16], float groundPlane[4], float lightPos[4]) {
    float dot = groundPlane[0] * lightPos[0] +
        groundPlane[1] * lightPos[1] +
        groundPlane[2] * lightPos[2] +
        groundPlane[3] * lightPos[3];

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            shadowMat[j * 4 + i] = -lightPos[i] * groundPlane[j];
            if (i == j) shadowMat[j * 4 + i] += dot;
        }
    }
}