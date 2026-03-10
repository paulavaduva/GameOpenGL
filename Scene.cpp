#include "Scene.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <cmath>
#include <vector>

using namespace std;

unsigned int texGround, texSky;

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
}

void drawGround(float size) {
    glBindTexture(GL_TEXTURE_2D, texGround);
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

    // lista de dealuri
    static vector<Hill> hills = {
        { 0.0f,   -60.0f, 50.0f, 15.0f }, // centru fata
        { 80.0f,  20.0f,  40.0f, 10.0f },  // dreapta spate
        {-70.0f,  50.0f,  45.0f, 12.0f },  // stanga spate
        { 40.0f,  -90.0f, 30.0f, 7.0f  },  // dreapta fata departare
        {-100.0f, -30.0f, 60.0f, 18.0f }   // stanga lateral
    };

    int resolution = 60; // rezolutie mai mare pentru a acoperi o zona mai mare
    float step = (size * 2) / resolution;

    for (int i = 0; i < resolution; i++) {
        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= resolution; j++) {
            for (int k = 0; k < 2; k++) {
                float x = -size + (i + k) * step;
                float z = -size + j * step;

                // calcularea inaltimii cumulate din toate dealurile
                float y = -1.0f;
                for (const auto& h : hills) {
                    float dist = sqrt(pow(x - h.x, 2) + pow(z - h.z, 2));
                    if (dist < h.radius) {
                        // netezire 
                        y += h.height * 0.5f * (1.0f + cos(3.1415f * dist / h.radius));
                    }
                }

                // acelasi ground pe tot terenul
                glTexCoord2f(x * 0.05f, z * 0.05f);
                glVertex3f(x, y, z);
            }
        }
        glEnd();
    }
}