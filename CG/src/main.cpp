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
glcFPSViewer *fpsViewer = new glcFPSViewer((char*) "Desenvolvimento 1 - ", (char*) " - Press ESC to Exit"); //Titulo da Janela
int altura = 1, grupo = 1; //Variaveis
std::vector< std::vector<vertice> > vetorVertice; //Estrutura utilizada para armazenar os vértices

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
    vertice v[8] = {{-1.0f, -1.0f,  0.0f},
        { 1.0f, -1.0f,  0.0f},
        {-1.0f,  1.0f,  0.0f},
        { 1.0f,  1.0f, -0.5f},
        { 0.3f,  0.3f,  1.5f},
        { -0.3f,  -0.3f,  1.5f},
        { 0.3f,  -0.3f,  1.5f},
        { -0.3f,  0.3f,  2.0f}
    };

//    std::vector<triangle> triangulos;
//
//    for(int i=0; i<vetorVertice.size(); i++)
//    {
//        if(!vetorVertice.at(i).empty())
//        {
//            for(int j=3; j<vetorVertice.at(i).size(); j++){
//
//                if(vetorVertice.at(i).size() > 3){
//                    triangle tri = {{vetorVertice.at(i).at(j-2), vetorVertice.at(i).at(j-1), vetorVertice.at(i).at(j)}};
//                    triangulos.push_back(tri);
//                }
//            }
//        }
//    }

    triangle t[6] = {{v[0], v[1], v[2]},
        {v[1], v[3], v[2]},
        {v[3], v[4], v[2]},
        {v[4], v[5], v[3]},
        {v[5], v[6], v[4]},
        {v[6], v[7], v[5]}
    };
//
//    glBegin(GL_TRIANGLES);
//    for(int i = 0; i < triangulos.size(); i++) // triangulos
//    {
//        CalculaNormal(triangulos.at(i), &vetorNormal); // Passa face triangular e endereço do vetor normal de saída
//        glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
//        for(int j = 0; j < 3; j++) // vertices do triangulo
//            glVertex3d(triangulos.at(i).v[j].x,triangulos.at(i).v[j].y, triangulos.at(i).v[j].z);
//    }
//    glEnd();

    glBegin(GL_TRIANGLES);
    for(int i = 0; i < 6; i++) // triangulos
    {
        CalculaNormal(t[i], &vetorNormal); // Passa face triangular e endereço do vetor normal de saída
        glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
        for(int j = 0; j < 3; j++) // vertices do triangulo
            glVertex3d(t[i].v[j].x,t[i].v[j].y, t[i].v[j].z);
    }
    glEnd();
}

void desenhaEixos() //Desenha os eixos
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


void desenhaPonto2D(vertice v) //Função auxiliar de desenho
{
    glDisable(GL_LIGHTING);
        float x=(((float)v.x*4)/(float)width)-1; //Normalização da coordenada X
        float y=((((float)v.y*2)/(float)height)-1)*-1; //Normalização da coordenada Y
        //printf("Valor renderizado (%.1f, %.1f)\n", x,y);
        glPointSize(10.0); //Define o tamanho do ponto
        glColor3f(1.0f, 0.0f, 0.0f); //Define cor do ponto
        glBegin(GL_POINTS);
            glVertex2f(x,y); //Cria o ponto nas coordenadas X e Y
        glEnd();
    glEnable(GL_LIGHTING);

}

void desenhaPontos() //Desenha os pontos
{
    for(int i=0; i<vetorVertice.size(); i++)
    {
        if(!vetorVertice.at(i).empty())
        {
            for(int j=0; j<vetorVertice.at(i).size(); j++)
            {
                desenhaPonto2D(vetorVertice.at(i).at(j)); //Desenha o ponto em 2D
            }
        }
    }
}

void excluirPonto(int grupo) //Apaga os pontos
{
    if(!vetorVertice.at(grupo-1).empty())
    {
        vetorVertice.at(grupo-1).pop_back(); // Remove o ultimo elemento do vetor
    }
}

void display(void)
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Limpa o buffer de cor
///--------------Primeira Viewport----------------------------------------------------
    glViewport (0, 0, (GLsizei) width/2, (GLsizei) height); //Define a visualização na janela da aplicação
    glEnable(GL_SCISSOR_TEST); //Para 'recortar' a janela
    glScissor(0, 0, (GLsizei) width/2, (GLsizei) height); //Define area de recorte
    glClearColor(1.0,1.0,1.0,0.0); //Define cor de fundo da viewport
    glMatrixMode (GL_PROJECTION); //Matriz de projeção
    glLoadIdentity (); //Matriz identidade
    glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f); //Define uma região de visualização ortogonal
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Limpa o buffer de cor
    glMatrixMode (GL_MODELVIEW); //Matriz de Desenho
    glLoadIdentity(); //Matriz identidade

    desenhaEixos(); //Desenha os eixos em 2D
    glEnable(GL_POINT_SMOOTH); //Para suavizar os pontos
    desenhaPontos(); //Desenha os pontos no 2D


///--------------Segunda Viewport-----------------------------------------------------

    glViewport (width/2, 0, (GLsizei) width/2, (GLsizei) height); //Define a visualização na janela da aplicação
    glEnable(GL_SCISSOR_TEST); //Para 'recortar' a janela
    glScissor(width/2, 0, (GLsizei) width/2, (GLsizei) height); //Define area de recorte
    glClearColor(0.0,0.0,0.0,0.0); //Define cor de fundo da viewport

    glMatrixMode (GL_PROJECTION); //Matriz de projeção
    glLoadIdentity (); //Matriz identidade
    gluPerspective(60.0, (GLfloat) (width/2)/(GLfloat) height, 0.01, 200.0); //Define projeções perspectivas

    glMatrixMode (GL_MODELVIEW); //Matriz de Desenho
    glLoadIdentity(); //Matriz identidade
    gluLookAt (0.0, 0.0, zdist, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); //Define a camera com olho, foco e orientação(up)



    glPushMatrix(); //Adiciona a matriz em uso no topo da pilha
        glRotatef( rotationY, 0.0, 1.0, 0.0 ); //Rotaciona o objeto em 3D
        glRotatef( rotationX, 1.0, 0.0, 0.0 ); //Rotaciona o objeto em 3D
        drawObject(); //Desenha o objeto em 3D
    glPopMatrix(); //Descarta a matriz no topo da pilha

    glutSwapBuffers(); //Troca os buffers

    fpsViewer->drawFPS(grupo, altura); //Define o titulo na janela
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
            v.x=x; v.y=y; v.z = (float) altura;
            if(vetorVertice.size() < grupo)
            {
                vetorVertice.resize(grupo);
            }
            vetorVertice.at(grupo-1).push_back(v);
        }
        last_x = x;
        last_y = y;
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
    glutInitWindowSize (800, 400);
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
