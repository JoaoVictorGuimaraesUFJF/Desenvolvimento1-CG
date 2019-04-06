/**************************************************/
/*                                                */
/* RODRIGO LUIS DE SOUZA DA SILVA    28/10/2004   */
/*                                                */
/*  			        circulo.cpp                   */
/*                                                */
/* LAST UPDATE: 28/10/2004                        */
/**************************************************/
/*

Animação de criação pra criar um círculo

*/

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/glut.h>
#include <math.h>

#define NUM 60
#define PI 3.1415927

float ANGLE = 360/ NUM;

float current = 0;
float angle = 0;
float raio = 1.5;
float dir = 1;
bool idleOn = true;

void display(void);
void init (void);
void idle();
void mouse(int button, int state, int x, int y);

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE|GLUT_DEPTH|GLUT_RGB);
   glutInitWindowSize (250, 250); 
   glutInitWindowPosition (100, 100);
   glutCreateWindow ("hello");
	glutMouseFunc( mouse );    
   glutIdleFunc( idle);      
   init ();
   glutDisplayFunc(display); 
   glutMainLoop();

   return 0;
}

void idle()
{
   if(current <= 360)
   {
      angle += ANGLE;
      current++;
   }   
   glutPostRedisplay();   
}   

// Mouse callback
void mouse(int button, int state, int x, int y)
{
   if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
   {
      (idleOn) ? glutIdleFunc( NULL ) : glutIdleFunc( idle );
      idleOn = !idleOn;
   }   
}

void display(void)
{
   // Limpar todos os pixels
	glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

   glColor3f (1.0, 0.0, 0.0);          
   glBegin(GL_LINE_STRIP);
   float g;
   int i;
   for(i = 0, g = 0.0f; i < current, g <= angle; i++, g+=ANGLE)
      glVertex2f (raio * cos(g * PI / 180), raio * sin(g * PI / 180));         

   glEnd();
	glutSwapBuffers ();    
}

void init (void) 
{
  // selecionar cor de fundo (preto) 
  glClearColor (0.0, 0.0, 0.0, 0.0);

  // inicializar sistema de viz.
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-2.0, 2.0, -2.0, 2.0, -2.0, 2.0);
  glLineWidth(3.0);
}

