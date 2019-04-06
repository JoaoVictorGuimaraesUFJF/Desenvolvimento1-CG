/*
  Name:        3D View.cpp
  Copyright:   Version 0.1
  Author:      Rodrigo Luis de Souza da Silva
  Date:        14/11/2013
  Description: C�digo base para exerc�cios em 3D.
*/

#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

float rotationX = 25.0, rotationY = 0.0;
int   last_x, last_y;
int width = 640, height = 480;
float dx,dy=0;
float distOrigem = 2.5;

void init(void)
{
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glShadeModel (GL_SMOOTH);

    glEnable(GL_LIGHT0);                   // habilita luz 0
    glEnable(GL_COLOR_MATERIAL);           // Utiliza cor do objeto como material
    glColorMaterial(GL_FRONT, GL_DIFFUSE);

    glEnable(GL_LIGHTING);                 // Habilita luz
    glEnable(GL_DEPTH_TEST);               // Habilita Z-buffer
    glEnable(GL_CULL_FACE);                // Habilita Backface-Culling
}

// Special Keys callback
void specialKeysPress(int key, int x, int y)
{
    switch(key)
    {
        case GLUT_KEY_UP:
            if (dy<4.0)
                dy += 1.0;
            break;
        case GLUT_KEY_DOWN:
            dy -= 1.0;
            break;
        case GLUT_KEY_RIGHT:
            dx += 1.0;
            break;
        case GLUT_KEY_LEFT:
            dx -= 1.0;
            break;
        default:
            printf("\nPressionou outra tecla especial n�o mapeada!");
            break;
    }
    glutPostRedisplay();
}

void desenhaGrid()
{
    glBegin(GL_LINES);
        glColor3f(1.0, 1.0, 1.0);
        float dy,dx=0;
        for(int i=0;i<6;i++){
            /// Desenha em x
            glVertex3f (-2.5, -2.5 + dy, 0.0);
            glVertex3f ( 2.5, -2.5 + dy, 0.0);
            /// Desenha em y
            glVertex3f (-2.5 + dx, 2.5, 0.0);
            glVertex3f (-2.5 + dx, -2.5, 0.0);
            dx += 1.0;
            dy += 1.0;
        }
    glEnd();
}

void display(void)
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective(60.0, (GLfloat) width/(GLfloat) height, 1.0, 200.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity ();
    gluLookAt (0.0, -5.0, distOrigem, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    glDisable(GL_LIGHTING);
    desenhaGrid();
    glPushMatrix();
        glColor3f (1.0f, 0.0f, 0.0);
        //glutSolidTorus(5.0f, 10.0f, 40, 40);
        glTranslatef(-2.0 + dx,-2.0 + dy, 0.5);
        glutSolidCube(1.0f);
    glPopMatrix();

    glEnable(GL_LIGHTING);
    glutSwapBuffers();
}


void idle ()
{
    glutPostRedisplay();
}

void reshape (int w, int h)
{
    width = w;
    height = h;
    glViewport (0, 0, (GLsizei) w, (GLsizei) h);
}

void keyboard (unsigned char key, int x, int y)
{
    switch (tolower(key))
    {
        case '+' :
            distOrigem--;
            if(distOrigem<20) distOrigem=20;
            break;
        case '-' :
            distOrigem++;
            if(distOrigem>180) distOrigem=180;
            break;
        case 27:
            exit(0);
            break;
    }
}

// Motion callback
void motion(int x, int y )
{
    rotationX += (float) (y - last_y);
    rotationY += (float) (x - last_x);

    last_x = x;
    last_y = y;
}

// Mouse callback
void mouse(int button, int state, int x, int y)
{
    if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
    {
        last_x = x;
        last_y = y;
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize (width, height);
    glutInitWindowPosition (100, 100);
    glutCreateWindow (argv[0]);
    init ();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc( mouse );
    glutMotionFunc( motion );
    glutKeyboardFunc(keyboard);
    glutSpecialFunc( specialKeysPress );
    glutIdleFunc(idle);
    glutMainLoop();
    return 0;
}
