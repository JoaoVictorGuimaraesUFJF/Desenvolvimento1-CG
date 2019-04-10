#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <stack>
#include <vector>

#include "glcFPSViewer.h"
#include "extras1.h"

/// Estruturas iniciais para armazenar vertices
//  Você poderá utilizá-las adicionando novos métodos (de acesso por exemplo) ou usar suas próprias estruturas.
class vertice
{
public:
    float x,y,z;
};

class triangle
{
public:
    vertice v[3];
};

/// Globals
float zdist = 5.0;
float rotationX = 0.0, rotationY = 0.0;
int   last_x, last_y;
int   width, height;
glcFPSViewer *fpsViewer = new glcFPSViewer((char*) "Desenvolvimento 1 - ", (char*) " - Press ESC to Exit");
int altura = 1, grupo = 1;
std::vector< std::vector<vertice> > vetorVertice;


/// Functions
void init(void)
{
    initLight(width, height); // Função extra para tratar iluminação.
    setMaterials();
}

/* Exemplo de cálculo de vetor normal que são definidos a partir dos vértices do triângulo;
  v_2
  ^
  |\
  | \
  |  \       'vn' é o vetor normal resultante
  |   \
  +----> v_1
  v_0
*/
void CalculaNormal(triangle t, vertice *vn)
{
    vertice v_0 = t.v[0],
            v_1 = t.v[1],
            v_2 = t.v[2];
    vertice v1, v2;
    double len;

    /* Encontra vetor v1 */
    v1.x = v_1.x - v_0.x;
    v1.y = v_1.y - v_0.y;
    v1.z = v_1.z - v_0.z;

    /* Encontra vetor v2 */
    v2.x = v_2.x - v_0.x;
    v2.y = v_2.y - v_0.y;
    v2.z = v_2.z - v_0.z;

    /* Calculo do produto vetorial de v1 e v2 */
    vn->x = (v1.y * v2.z) - (v1.z * v2.y);
    vn->y = (v1.z * v2.x) - (v1.x * v2.z);
    vn->z = (v1.x * v2.y) - (v1.y * v2.x);

    /* normalizacao de n */
    len = sqrt(pow(vn->x,2) + pow(vn->y,2) + pow(vn->z,2));

    vn->x /= len;
    vn->y /= len;
    vn->z /= len;
}

void drawObject()
{
    vertice vetorNormal;
    int i=0;
    /*vertice v[8] = {{-1.0f, -1.0f,  0.0f},
        { 1.0f, -1.0f,  0.0f},
        {-1.0f,  1.0f,  0.0f},
        { 1.0f,  1.0f, -0.5f},
        { 0.3f,  0.3f,  1.5f},
        { -0.3f,  -0.3f,  1.5f},
        { 0.3f,  -0.3f,  1.5f},
        { -0.3f,  0.3f,  2.0f}
    };*/

    std::vector<triangle> triangulos;
    if(triangulos.size()!=0)
        i = vetorVertice.at(grupo).size();
    if(i >= 3){
        triangle tri = {{vetorVertice.at(grupo).at(i-2), vetorVertice.at(grupo).at(i-1), vetorVertice.at(grupo).at(i)}};
        triangulos.push_back(tri);
    }

    /*triangle t[6] = {{v[0], v[1], v[2]},
        {v[1], v[3], v[2]},
        {v[3], v[4], v[2]},
        {v[4], v[5], v[3]},
        {v[5], v[6], v[4]},
        {v[6], v[7], v[5]}
    };*/

    glBegin(GL_TRIANGLES);
    for(int i = 0; i < triangulos.size(); i++) // triangulos
    {
        CalculaNormal(triangulos.at(i), &vetorNormal); // Passa face triangular e endereço do vetor normal de saída
        glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
        for(int j = 0; j < 3; j++) // vertices do triangulo
            glVertex3d(triangulos.at(i).v[j].x,triangulos.at(i).v[j].y, triangulos.at(i).v[j].z);
    }
    glEnd();
}

void desenhaEixos()
{
    glDisable(GL_LIGHTING);
        glBegin(GL_LINES);
            glColor3f(1.0,0.0,0.0);
            glVertex3f(-100.0,0.0,0.0);
            glVertex3f(100.0,0.0,0.0);
        glEnd();

        glBegin(GL_LINES);
            glColor3f(0.0,1.0,0.0);
            glVertex3f(0.0,-100.0,0.0);
            glVertex3f(0.0,100.0,0.0);
        glEnd();
    glEnable(GL_LIGHTING);
}


void desenhaPonto2D(vertice v)
{
    glDisable(GL_LIGHTING);
    float x=(((float)v.x*4)/(float)width)-1;
    float y=(((float)v.y*2)/(float)height)-1;
    //float x=v.x;
    //float y=v.y;
    glPointSize(10.0);
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_POINTS);
        glVertex2f(x,y);
    glEnd();
glEnable(GL_LIGHTING);

}

void desenhaPontos()
{
    for(int i=0; i<vetorVertice.size(); i++)
    {
        if(!vetorVertice.at(i).empty())
        {
            for(int j=0; j<vetorVertice.at(i).size(); j++)
            {
                desenhaPonto2D(vetorVertice.at(i).at(j));
            }
        }
    }
}

void excluirPonto(int grupo)
{
    if(!vetorVertice.at(grupo-1).empty())
    {
        vetorVertice.at(grupo-1).pop_back();
    }
}

void display(void)
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective(60.0, (GLfloat) (width/2)/(GLfloat) height, 0.01, 200.0);
    gluLookAt (0.0, 0.0, zdist, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

///--------------Primeira Viewport----------------------------------------------------
    glViewport (0, 0, (GLsizei) width/2, (GLsizei) height);
    glEnable(GL_SCISSOR_TEST);
    glScissor(0, 0, (GLsizei) width/2, (GLsizei) height);
    glClearColor(1.0,1.0,1.0,0.0);
    glMatrixMode (GL_PROJECTION);
    glOrtho(-1,1,-1,1,0,0);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode (GL_MODELVIEW);

    desenhaEixos();
    desenhaPontos();


///--------------Segunda Viewport-----------------------------------------------------

    glViewport (width/2, 0, (GLsizei) width/2, (GLsizei) height);
    glEnable(GL_SCISSOR_TEST);
    glScissor(width/2, 0, (GLsizei) width/2, (GLsizei) height);
    glClearColor(0.0,0.0,0.0,0.0);

    glPushMatrix();
    glRotatef( rotationY, 0.0, 1.0, 0.0 );
    glRotatef( rotationX, 1.0, 0.0, 0.0 );
    drawObject();
    glPopMatrix();

    glutSwapBuffers();

    fpsViewer->drawFPS(grupo, altura);
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
    case 27:
        exit(0);
        break;
    }
}

// Special Keys callback
void specialKeys(int key, int x, int y)
{
    switch(key)
    {
    case GLUT_KEY_LEFT:
        if(grupo <= 1)
            grupo = 1;
        else
            grupo -= 1;
        break;
    case GLUT_KEY_RIGHT:
        grupo += 1;
        break;
    case GLUT_KEY_UP:
        altura += 1;
        break;
    case GLUT_KEY_DOWN:
        if(altura <= 1)
            altura = 1;
        else
            altura -= 1;
        break;
    }
    glutPostRedisplay();
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
        if(x<=width/2){
            printf("Pressionado em (%d, %d)\n", x,y);
            vertice v;
            v.x=x; v.y=y; v.z= (float) altura;
            if(vetorVertice.size() < grupo)
            {
                vetorVertice.resize(grupo);
            }
            vetorVertice.at(grupo-1).push_back(v);
        }
        last_x = x;
        last_y = y;
        printf("Ponto na posição %d do vetor (%f %f) \n", grupo-1, vetorVertice.at(grupo-1).back().x, vetorVertice.at(grupo-1).back().y);
    }
    if ( button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN )
    {
        excluirPonto(grupo);
    }
    if(button == 3) // Scroll up
    {
        zdist+=1.0f;
    }
    if(button == 4) // Scroll Down
    {
        zdist-=1.0f;
    }
}

/// Main
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize (800, 600);
    glutInitWindowPosition (100, 100);
    glutCreateWindow (argv[0]);
    init ();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc( mouse );
    glutMotionFunc( motion );
    glutKeyboardFunc(keyboard);
    glutSpecialFunc( specialKeys );
    glutIdleFunc(idle);
    glutMainLoop();
    return 0;
}
