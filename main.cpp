#include <freeglut.h>
#include "Scene.h"
#include "Camera.h"
#include <iostream>

using namespace std;

float shadowMat[16];
float groundPlane[] = { 0.0f, 1.0f, 0.0f, 1.0f };

float moonPos[] = { 100.0f, 100.0f, 50.0f, 1.0f }; // luna 
float polePos[] = { -40.0f, 15.0f, -40.0f, 1.0f }; // stalpul

void initLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0); // luna
    glEnable(GL_LIGHT1); // stalpul
    glEnable(GL_COLOR_MATERIAL);

    float ambient[] = { 0.2f, 0.2f, 0.3f, 1.0f };

    float diffuseMoon[] = { 0.4f, 0.4f, 0.5f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseMoon);
    
    float diffusePole[] = { 1.0f, 1.0f, 0.8f, 1.0f };
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffusePole);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glLoadIdentity();
    updateCamera();

    glLightfv(GL_LIGHT0, GL_POSITION, moonPos);
    glLightfv(GL_LIGHT1, GL_POSITION, polePos);

    glDisable(GL_LIGHTING);
    drawSkybox(150.0f);

    glEnable(GL_LIGHTING);
    drawRelief(150.0f);


    glColor3f(1.0f, 1.0f, 1.0f);
    drawCircuit(40.0f, 55.0f, 500);

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glColor4f(0.05f, 0.05f, 0.1f, 0.15f);


    calculateShadowMatrix(shadowMat, groundPlane, moonPos);
    glPushMatrix();
        glMultMatrixf(shadowMat);
        drawStaticObjects();
        drawLightPole(polePos[0], polePos[2]);
    glPopMatrix();


    float actualPolePos[] = { polePos[0], getTerrainHeight(polePos[0], polePos[2]) + 15.0f, polePos[2], 1.0f };
    calculateShadowMatrix(shadowMat, groundPlane, actualPolePos);
    glPushMatrix();
        glMultMatrixf(shadowMat);
        drawStaticObjects();
        drawLightPole(polePos[0], polePos[2]);
    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    glDisable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);

    drawStaticObjects();
    drawLightPole(polePos[0], polePos[2]);

    glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    float aspect = (float)w / (float)h;

    // setare fereastra de vizualizare
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(fov, (float)w / (float)h, 1.0, 1000.0);

    glMatrixMode(GL_MODELVIEW);
}

void mouseWheel(int wheel, int direction, int x, int y) {
    handleZoom(direction); 

    // recalcularea perspectivei imediat
    reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("GameOpenGL");

    glEnable(GL_DEPTH_TEST);
    initTextures();
    initCamera();
    initLighting();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutPassiveMotionFunc(handleMouseMove); 
    glutSetCursor(GLUT_CURSOR_NONE); // ascunde cursor mouse
    glutKeyboardFunc(handleKeyboard);
    glutMouseWheelFunc(mouseWheel);

    glutMainLoop();
    return 0;
}