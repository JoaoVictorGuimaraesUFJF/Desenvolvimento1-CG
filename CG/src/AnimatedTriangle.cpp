/*
  Name:         triangle_anim.cpp
  Copyright:    Version 0.1
  Author:       Rodrigo Luis de Souza da Silva
  Last Update:  29/11/2018 (Animation based on FPS)
  Release:      28/10/2004
  Description:  Animação de triângulo usando função idle.
                Update: visualização e escolhe do FPS desejado
*/

#include <iostream>
#include <GL/glut.h>
#include <glcFPSViewer.h>

using namespace std;

float desiredFPS = 60;
float varX1, varX2, varX3, varX4, varX5, varX6, varY1, varY2, varY3, varY4, varY5, varY6 = 0.0f;
float dirX1 = 1.0f, dirY1 = -1.0f, dirX2 = -1.0f, dirY2 = 1.0f, dirX3 = -1.0f, dirY3 = 1.0f,
      dirX4 = 1.0f, dirY4 = 1.0f, dirX5 = 1.0f, dirY5 = 1.0f, dirX6 = 1.0f, dirY6 = 1.0f;
glcFPSViewer *fpsViewer = new glcFPSViewer((char*) "Triangle Animation. ", (char*) " - Press ESC to Exit");
bool fullScreen = false;

void display(void);
void init (void);
void idle();
void mouse(int button, int state, int x, int y);
void keyboard(unsigned char key, int x, int y);

void idle()
{
    float t, desiredFrameTime, frameTime;
    static float tLast = 0.0;

    // Get elapsed time
    t = glutGet(GLUT_ELAPSED_TIME);
    // convert milliseconds to seconds
    t /= 1000.0;

    // Calculate frame time
    frameTime = t - tLast;
    // Calculate desired frame time
    desiredFrameTime = 1.0 / (float) (desiredFPS);

    // Check if the desired frame time was achieved. If not, skip animation.
    if( frameTime <= desiredFrameTime)
        return;

    // **  UPDATE ANIMATION VARIABLES ** //
    float step = 1; // Speed of the animation
    if(1+varX1 < 0.0 || 1+varX1 > 8.0)
        dirX1 *= -1;
    varX1+=dirX1* (step / desiredFPS);

    if(4+varY1 < 0.0 || 4+varY1 >  8.0)
        dirY1 *= -1;
    varY1+=dirY1* (step / desiredFPS);

    if(2+varX2 < 0.0 || 2+varX2 > 8.0)
        dirX2 *= -1;
    varX2+=dirX2* (step / desiredFPS);

    if(1+varY2 < 0.0 || 1+varY2 > 8.0)
        dirY2 *= -1;
    varY2+=dirY2* (step / desiredFPS);

    if(7+varX3 < 0.0 || 7+varX3 > 8.0)
        dirX3 *= -1;
    varX3+=dirX3* (step / desiredFPS);

    if(5+varY3 < 0.0 || 5+varY3 > 8.0)
        dirY3 *= -1;
    varY3+=dirY3* (step / desiredFPS);
    ///---------------------------------------------
    if(6+varX4 < 0.0 || 6+varX4 > 8.0)
        dirX4 *= -1;
    varX4+=dirX4* (step / desiredFPS);

    if(4+varY4 < 0.0 || 34+varY4 >  8.0)
        dirY4 *= -1;
    varY4+=dirY4* (step / desiredFPS);

    if(5+varX5 < 0.0 || 5+varX5 > 8.0)
        dirX5 *= -1;
    varX5+=dirX5* (step / desiredFPS);

    if(3+varY5 < 0.0 || 3+varY5 > 8.0)
        dirY5 *= -1;
    varY5+=dirY5* (step / desiredFPS);

    if(3+varX6 < 0.0 || 3+varX6 > 8.0)
        dirX6 *= -1;
    varX6+=dirX6* (step / desiredFPS);

    if(6+varY6 < 0.0 || 6+varY6 > 8.0)
        dirY6 *= -1;
    varY6+=dirY6* (step / desiredFPS);

    // Update tLast for next time, using static local variable
    tLast = t;

    glutPostRedisplay();

}

void display(void)
{
    // Limpar todos os pixels
    glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glColor3f (1.0, 0.0, 0.0);
    glBegin(GL_LINE_STRIP);
        glVertex2f (1+varX1 , 4+varY1);
        glVertex2f (2+varX2 , 1+varY2);
        glVertex2f (7+varX3 , 5+varY3);
        glVertex2f (1+varX1 , 4+varY1);
    glEnd();

    glColor3f (0.0, 1.0, 0.0);
    glBegin(GL_LINE_STRIP);
        glVertex2f (6+varX4 , 4+varY4);
        glVertex2f (5+varX5 , 3+varY5);
        glVertex2f (3+varX6 , 6+varY6);
        glVertex2f (6+varX4 , 4+varY4);
    glEnd();
    glutSwapBuffers ();

    // Print FPS
    fpsViewer->drawFPS();
}

void init (void)
{
    // selecionar cor de fundo (preto)
    glClearColor (0.0, 0.0, 0.0, 0.0);

    // inicializar sistema de viz.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 8.0, 0.0, 8.0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
        case '1':
            desiredFPS = 60;
            break;
        case '2':
            desiredFPS = 30;
            break;
        case '3':
            desiredFPS = 10;
            break;
        case 27:
            exit(0);
            break;
    }
}

void specialKeys(int key, int x, int y)
{
    switch(key)
    {
        case GLUT_KEY_F11:
            (!fullScreen) ? glutFullScreen() : glutReshapeWindow(800, 600);
            fullScreen = !fullScreen;
            break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);

    cout << "Triangle Animation Example" << endl;
    cout << "--------------------------" << endl;
    cout << "Press 1 to change FPS to ~60" << endl;
    cout << "Press 2 to change FPS to ~30" << endl;
    cout << "Press 3 to change FPS to ~10" << endl;
    cout << "Press ESC to exit" << endl;
    glutInitDisplayMode (GLUT_DOUBLE|GLUT_DEPTH|GLUT_RGB);
    glutInitWindowSize (400, 400);
    glutCreateWindow ("");
    glutKeyboardFunc(keyboard);
    glutIdleFunc( idle);
    init ();
    glutDisplayFunc(display);
    glutSpecialFunc( specialKeys );
    glutMainLoop();

    return 0;
}
