#include <freeglut.h>
#include "Scene.h"
#include "Camera.h"
#include <iostream>

using namespace std;

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // resetare culoare la alb inainte de a desena texturile
    glColor3f(1.0f, 1.0f, 1.0f);

    updateCamera(); 

    drawSkybox(150.0f); 
    drawRelief(150.0f); 

    drawCircuit(40.0f, 55.0f, 500);
    drawStaticObjects();

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

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutPassiveMotionFunc(handleMouseMove); 
    glutSetCursor(GLUT_CURSOR_NONE); // ascunde cursor mouse

    glutMouseWheelFunc(mouseWheel);

    glutMainLoop();
    return 0;
}