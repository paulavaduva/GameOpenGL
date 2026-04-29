#include "Scene.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <ctime>

using namespace std;

unsigned int texGround, texSky;
unsigned int texRoad;
unsigned int texBuilding;

vector<StaticObj> worldObjects; 
static bool generated = false;
extern float polePos[];

vector<MovingObj> randomNPCs;
static bool npcsGenerated = false;

vector<CircuitCarObj> circuitCars;
static bool circuitInit = false;

extern float carX, carZ;

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

    int resolution = 150;
    float step = (size * 2) / resolution;

    for (int i = 0; i < resolution; i++) {
        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= resolution; j++) {
            for (int k = 0; k < 2; k++) {
                float x = -size + (i + k) * step;
                float z = -size + j * step;
                float y = getTerrainHeight(x, z);


                float hL = getTerrainHeight(x - 0.1f, z);
                float hR = getTerrainHeight(x + 0.1f, z);
                float hD = getTerrainHeight(x, z - 0.1f);
                float hU = getTerrainHeight(x, z + 0.1f);

                glNormal3f(hL - hR, 1.0f, hD - hU);

                glTexCoord2f(x * 0.05f, z * 0.05f);
                glVertex3f(x, y, z);
            }
        }
        glEnd();
    }
}

void drawLightPole(float x, float z) {
    float y = getTerrainHeight(x, z);
    bool isShadow = !glIsEnabled(GL_LIGHTING);

    glPushMatrix();
    glTranslatef(x, y, z);

    glDisable(GL_TEXTURE_2D);
    if (!isShadow) glColor3f(0.2f, 0.2f, 0.22f);

    glPushMatrix();
        glRotatef(-90, 1, 0, 0);
        GLUquadric* pole = gluNewQuadric();
        gluCylinder(pole, 0.4, 0.3, 15.0, 12, 1);
    glPopMatrix();

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
        { 0.0f,   -60.0f, 50.0f, 3.0f }, 
        { 80.0f,  20.0f,  40.0f, 3.0f }, 
        {-70.0f,  50.0f,  45.0f, 3.0f },
        { 40.0f,  -90.0f, 30.0f, 3.0f }, 
        {-100.0f, -30.0f, 60.0f, 3.0f }  
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
    if (isShadow) return; 

    glBindTexture(GL_TEXTURE_2D, texRoad);
    glColor3f(1.0f, 1.0f, 1.0f);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.0f, -1.0f);

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * 3.14159f * (float)i / (float)segments;
        float stretchZ = 1.5f;

        float xIn = innerRadius * cos(angle);
        float zIn = innerRadius * sin(angle) * stretchZ;
        float yIn = getTerrainHeight(xIn, zIn) + 0.4f; 

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

bool checkMovingCollision(float x1, float z1, float r1, float x2, float z2, float r2) {
    float dist = sqrt(pow(x1 - x2, 2) + pow(z1 - z2, 2));
    return dist < (r1 + r2);
}
bool checkStaticCollision(float x, float z, float radius) {
    // coliziune cu Obiecte Statice
    for (const auto& obj : worldObjects) {
        float objW, objD;
        if (obj.type == 0) { objW = 2.0f; objD = 2.0f; } // copac
        else if (obj.type == 1) { objW = 20.0f; objD = 20.0f; } // bloc mare
        else if (obj.type == 2) { objW = 15.0f; objD = 15.0f; } // bloc mic
        else continue;

        if (x > (obj.x - objW / 2 - radius) && x < (obj.x + objW / 2 + radius) &&
            z >(obj.z - objD / 2 - radius) && z < (obj.z + objD / 2 + radius)) {
            return true;
        }
    }

    // coliziune cu stalpul
    float poleHitbox = 1.5f;
    if (checkMovingCollision(x, z, radius, polePos[0], polePos[2], poleHitbox)) {
        return true;
    }

    return false;
}

bool checkCollision(float x, float z) {
    float carSize = 2.5f;

    // verificam obiectele fixe
    if (checkStaticCollision(x, z, carSize)) return true;

    // verificam NPC-urile aleatorii
    for (const auto& npc : randomNPCs) {
        if (checkMovingCollision(x, z, carSize, npc.x, npc.z, 2.0f)) return true;
    }

    // verificam masinile de pe circuit
    for (const auto& cCar : circuitCars) {
        if (checkMovingCollision(x, z, carSize, cCar.x, cCar.z, 2.5f)) return true;
    }

    return false;
}

void drawStaticObjects() {
    float innerR = 40.0f;
    float outerR = 55.0f;

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

void drawCar(float x, float z, float angle) {
    float y = getTerrainHeight(x, z);
    bool isShadow = !glIsEnabled(GL_LIGHTING);

    glPushMatrix();
    glTranslatef(x, y + 1.0f, z); 
    glRotatef(angle, 0.0f, 1.0f, 0.0f);

    if (!isShadow) glDisable(GL_TEXTURE_2D);

    if (!isShadow) glColor3f(1.0f, 0.0f, 0.0f);
    glPushMatrix();
    glScalef(4.0f, 2.0f, 8.0f);
    glutSolidCube(1.0);
    glPopMatrix();

    if (!isShadow) {
        glColor3f(0.2f, 0.2f, 0.2f);
        glPushMatrix();
        glTranslatef(0.0f, 1.5f, -1.0f);
        glScalef(3.5f, 1.5f, 4.0f);
        glutSolidCube(1.0);
        glPopMatrix();
    }

    if (!isShadow) {
        glEnable(GL_TEXTURE_2D);
        glColor3f(1.0f, 1.0f, 1.0f);
    }
    glPopMatrix();
}

void updateRandomObjects() {
    if (!npcsGenerated) {
        srand(time(NULL));
        for (int i = 0; i < 5; i++) { // generam 5 obiecte
            MovingObj npc;
            npc.x = (rand() % 100) - 50.0f;
            npc.z = (rand() % 100) - 50.0f;
            npc.angle = (rand() % 360);
            npc.speed = 0.3f;
            npc.changeDirTimer = 0;
            randomNPCs.push_back(npc);
        }
        npcsGenerated = true;
    }

    for (int i = 0; i < randomNPCs.size(); i++) {
        auto& npc = randomNPCs[i];
        // daca timer-ul a expirat, alegem o directie noua aleatorie
        if (npc.changeDirTimer <= 0) {
            npc.angle = (rand() % 360);
            npc.changeDirTimer = 50 + (rand() % 100); // intre 50 si 150 de cadre
        }

        float rad = npc.angle * 3.14159f / 180.0f;
        float nextX = npc.x + sin(rad) * npc.speed;
        float nextZ = npc.z + cos(rad) * npc.speed;
        float npcRad = 1.5f;

        bool collision = false;

        if (checkStaticCollision(nextX, nextZ, npcRad)) collision = true;

        if (checkMovingCollision(nextX, nextZ, npcRad, carX, carZ, 2.5f)) collision = true;

        for (int j = 0; j < randomNPCs.size(); j++) {
            if (i == j) continue; 
            if (checkMovingCollision(nextX, nextZ, npcRad, randomNPCs[j].x, randomNPCs[j].z, npcRad)) {
                collision = true;
                break;
            }
        }

        for (const auto& cCar : circuitCars) {
            if (checkMovingCollision(nextX, nextZ, npcRad, cCar.x, cCar.z, 2.0f)) {
                collision = true;
                break;
            }
        }

        if (!collision) {
            npc.x = nextX;
            npc.z = nextZ;
        }
        else {
            npc.changeDirTimer = 0;
        }

        npc.changeDirTimer--;

        if (npc.x > 150) npc.x = -150; if (npc.x < -150) npc.x = 150;
        if (npc.z > 150) npc.z = -150; if (npc.z < -150) npc.z = 150;

        if (npc.x > 150) npc.x = -150;
        if (npc.x < -150) npc.x = 150;
        if (npc.z > 150) npc.z = -150;
        if (npc.z < -150) npc.z = 150;
    }
}

void drawMovingObjects() {
    bool isShadow = !glIsEnabled(GL_LIGHTING);
    for (const auto& npc : randomNPCs) {
        float y = getTerrainHeight(npc.x, npc.z); 

        glPushMatrix();
        glTranslatef(npc.x, y + 1.5f, npc.z);
        glRotatef(npc.angle, 0, 1, 0);

        if (!isShadow) {
            glDisable(GL_TEXTURE_2D);
            glColor3f(0.0f, 0.7f, 0.8f); 
        }

        GLUquadric* quad = gluNewQuadric();
        glRotatef(-90, 1, 0, 0);
        gluCylinder(quad, 1.0, 1.0, 3.0, 12, 1);

        glTranslatef(0, 0, 3.5f);
        glutSolidSphere(0.8, 10, 10);

        if (!isShadow) glEnable(GL_TEXTURE_2D);
        glPopMatrix();
    }
}

void updateCircuitCars() {
    if (!circuitInit) {
        circuitCars.push_back({ 0.0f, 0.01f, 43.0f }); 
        circuitCars.push_back({ 3.14f, 0.012f, 52.0f }); 
        circuitInit = true;
    }

    float stretchZ = 1.5f;
    for (auto& car : circuitCars) {
        car.angle += car.speed;
        if (car.angle > 6.283f) car.angle -= 6.283f;

        car.x = car.radius * cos(car.angle);
        car.z = car.radius * sin(car.angle) * stretchZ;
    }
}

void drawCircuitCars() {
    bool isShadow = !glIsEnabled(GL_LIGHTING);
    float stretchZ = 1.5f; 

    for (const auto& car : circuitCars) {
        float x = car.radius * cos(car.angle);
        float z = car.radius * sin(car.angle) * stretchZ;

        float y = getTerrainHeight(x, z) + 0.5f;


        float dx = -car.radius * sin(car.angle);
        float dz = car.radius * cos(car.angle) * stretchZ;
        float rotation = atan2(dx, dz) * 180.0f / 3.14159f;

        glPushMatrix();
        glTranslatef(x, y, z);
        glRotatef(rotation, 0, 1, 0);

        if (!isShadow) {
            glDisable(GL_TEXTURE_2D);
            glColor3f(0.0f, 0.5f, 0.0f); 
        }

        glPushMatrix();
        glScalef(3.0f, 1.5f, 6.0f);
        glutSolidCube(1.0);
        glPopMatrix();

        if (!isShadow) glEnable(GL_TEXTURE_2D);
        glPopMatrix();
    }
}