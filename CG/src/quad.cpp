/*
  Name:        quad.cpp
  Copyright:   Version 0.1
  Author:      Rodrigo Luis de Souza da Silva
  Last Update: 03/09/2014
  Release:     18/09/2004
  Description: Simple opengl program
*/

#include <stdio.h>
#include <GL/glut.h>

void display(void);
void init (void);
void keyboard(unsigned char key, int x, int y);

int var = 1;

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize (250, 250);
    glutInitWindowPosition (100, 100);
    glutCreateWindow ("Quad Test");
    init ();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);

    printf("Pressione ESC para fechar.\n");
    printf("Pressione A para a imagem 1.\n");
    printf("Pressione B para a imagem 2.\n");
    printf("Pressione C para a imagem 3.\n");
    printf("Pressione D para a imagem 4.\n");

    glutMainLoop();

    return 0;
}

void desenha()
{
    glColor3f (0.0, 1.0, 0.0);
    glBegin(GL_LINES);
        glVertex3f (0.5, 0.0, 0.0);
        glVertex3f (0.5, 1.0, 0.0);
    glEnd();

    glBegin(GL_LINES);
        glVertex3f (0.0, 0.5, 0.0);
        glVertex3f (1.0, 0.5, 0.0);
    glEnd();

    if(var == 1){
        glColor3f(1.0,0.0,0.0);
        glBegin(GL_TRIANGLES);
            glVertex3f (0.5, 0.5, 0.0);
            glVertex3f (0.95, 0.5, 0.0);
            glVertex3f (0.725, 0.90, 0.0);
        glEnd();
    }


    if(var == 2){
        glColor3f(1.0,0.0,0.0);
        glBegin(GL_LINE_LOOP);
            glVertex3f (0.5, 0.5, 0.0);
            glVertex3f (0.95, 0.5, 0.0);
            glVertex3f (0.725, 0.90, 0.0);
        glEnd();
    }

    if(var == 3){
        glColor3f(1.0,0.0,0.0);
        glBegin(GL_TRIANGLES);
            glVertex3f (0.5, 0.5, 0.0);
            glVertex3f (0.95, 0.5, 0.0);
            glVertex3f (0.725, 0.90, 0.0);
        glEnd();

        glColor3f(1.0,0.0,0.0);
        glBegin(GL_LINE_LOOP);
            glVertex3f (0.0, 0.5, 0.0);
            glVertex3f (0.95, 0.5, 0.0);
            glVertex3f (0.725, 0.90, 0.0);
        glEnd();
    }

    if(var == 4){

    }
}

void display(void)
{
    // Limpar todos os pixels
    glClear (GL_COLOR_BUFFER_BIT);

    // Desenhar um polígono branco (retângulo)
    desenha();

    glutSwapBuffers ();
}


void init (void)
{
    // selecionar cor de fundo (preto)
    glClearColor (1.0, 1.0, 1.0, 0.0);

    // inicializar sistema de viz.
    glMatrixMode(GL_PROJECTION);              // Seleciona Matriz de Projeção
    glLoadIdentity();
    gluOrtho2D(0.0, 1.0, 0.0, 1.0);
    if(var = 4){
        //gluOrtho2D(0.0, 1.0, 0.0, 1.0);
    }

    glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
    glLoadIdentity();           // Inicializa com matriz identidade
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27:
            exit(0);
            break;
        case 'a': var = 1;  break;
        case 'b': var = 2;  break;
        case 'c': var = 3;  break;
        case 'd': var = 4;  break;
    }
    glutPostRedisplay();
}
