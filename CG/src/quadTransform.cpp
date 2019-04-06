/*
  Name:        quad_transform.cpp
  Copyright:   Version 0.1
  Author:      Rodrigo Luis de Souza da Silva
  Date:        16/09/2004
  Description: Transformations using of OpenGL commands
*/

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>

float angle = 0, scale = 1.0;
float xtrans = 0, ytrans = 0, ztrans = 0;
int enableMenu = 0;

void display(void);
void init (void);
void desenhaEixos();
void showMenu();
void mouse(int button, int state, int x, int y);

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE|GLUT_DEPTH|GLUT_RGB);
    glutInitWindowSize (300, 300);
    glutInitWindowPosition (100, 100);
    glutCreateWindow ("hello");
    glutMouseFunc( mouse );
    init ();
    printf("Posicione as janelas e clique na janela do OpenGL para habilitar o menu.\n");
    glutDisplayFunc(display);
    glutMainLoop();

    return 0;
}

// Mouse callback
void mouse(int button, int state, int x, int y)
{
    if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        enableMenu = 1;
}

void showMenu()
{
    int op;
    system("cls");
    printf("\n=== MENU ===");
    printf("\n1 - Translacao");
    printf("\n2 - Rotacao");
    printf("\n3 - Escala");
    printf("\n4 - Sair");
    printf("\nOpcao: ");

    scanf("%d", &op);
    switch(op)
    {
        case 1:
            printf("\n\nInforme o vetor de translacao (entre -100.0 e 100.0)");
            printf("\nX : ");
            scanf("%f", &xtrans);
            printf("Y : ");
            scanf("%f", &ytrans);
            break;
        case 2:
            printf("\n\nInforme o angulo de rotacao (em graus): ");
            scanf("%f", &angle);
            break;
        case 3:
            printf("\n\nInforme o fator de escala: ");
            scanf("%f", &scale);
            break;
        case 4:
            exit(1);
            break;
        default:
            printf("\n\nOpcao invalida\n\n");
            break;
    }
}

void desenhaEixos()
{
    glColor3f (0.0, 1.0, 0.0);
    glBegin(GL_LINES);
    glVertex2f (0.0, -100.0);
    glVertex2f (0.0, 100.0);
    glVertex2f (-100.0, 0.0);
    glVertex2f (100.0, 0.0);
    glEnd();
}

void display(void)
{
    // Limpar todos os pixels
    glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glLoadIdentity(); // Inicializa com matriz identidade

    desenhaEixos();

    glColor3f (1.0, 0.0, 0.0);

    glPushMatrix();
    glTranslatef(xtrans, ytrans, 0.0);
    glRotatef(angle, 0.0, 0.0, 1.0);
    glScalef(scale, scale, scale);
    glutWireCube(10);
    glPopMatrix();

    glutSwapBuffers ();
    glutPostRedisplay();

    if(enableMenu)
        showMenu();
}

void init (void)
{
    // selecionar cor de fundo (preto)
    glClearColor (0.0, 0.0, 0.0, 0.0);

    // inicializar sistema de viz.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-100.0, 100.0, -100.0, 100.0, -100.0, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
