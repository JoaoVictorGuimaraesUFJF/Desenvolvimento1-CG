#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stack>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "extras.h"

/// Estruturas iniciais para armazenar vertices
//  Você poderá utilizá-las adicionando novos métodos (de acesso por exemplo) ou usar suas próprias estruturas.
class vertice
{
public:
    float x,y,z,x0=0,x1=0,y0=0,y1=0;
    int espessura;
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
int altura = 1, grupo = 1, espessura = 1; //Variaveis
vertice vetorOrtogonal;
bool fullScreen = false;
std::vector< std::vector<vertice> > vetorVertice; //Estrutura utilizada para armazenar os vértices

/// Functions
void init(void)
{
    initLight(width, height); // Função extra para tratar iluminação.
    setMaterials();
}

void salvarModelo(std::string outFileName)
{
    //std::ofstream outFile(outFileName, std::ifstream::out);

    std::ofstream outFile;
    outFile.open(outFileName, std::ifstream::out);

    int quantidadeGrupos=0;

    for(int i=0; i<vetorVertice.size(); i++) //percorre os grupos
    {
        if(!vetorVertice.at(i).empty())
        {
            quantidadeGrupos++;
        }
    }

    outFile << quantidadeGrupos << std::endl;

    for(int i=0; i<vetorVertice.size(); i++) //percorre os grupos
    {
        if(!vetorVertice.at(i).empty() && vetorVertice.at(i).size()>1)//verifica se há mais de 1 ponto para desenhar as faces
        {
            for(int j=0; j<vetorVertice.at(i).size(); j++) //percorre a partir do segundo vertice e cria dois triangulos(uma face)
            {
                outFile << vetorVertice.at(i).at(j).x << ";" << vetorVertice.at(i).at(j).y << ";" << vetorVertice.at(i).at(j).z << ";";
                outFile << vetorVertice.at(i).at(j).x0 << ";" << vetorVertice.at(i).at(j).x1 << ";" << vetorVertice.at(i).at(j).y0 << ";" << vetorVertice.at(i).at(j).y1 << ";";
                outFile << vetorVertice.at(i).at(j).espessura << std::endl;

            }
            outFile << std::endl;
        }
    }
    outFile.close();
}

std::vector<std::string> explode(std::string const & string, char delimiter)
  {
    std::vector<std::string> result;
    std::istringstream iss(string);

    for (std::string token; std::getline(iss, token, delimiter); )
      result.push_back(std::move(token));

    return result;
  }

void carregarModelo(std::string inFileName)
{
    //std::ofstream outFile(outFileName, std::ifstream::out);

    std::ifstream inFile(inFileName);

    if (!inFile.is_open())
    {
      std::cout << "Falha na leitura do arquivo" << std::endl;
    }
    else
    {
        vetorVertice.clear();
        std::string line;
        std::vector<std::string> data;
        int quantidadeGrupos=0, grupoAtual=0;
        std::getline(inFile, line);
        quantidadeGrupos = std::stoi(line);
        grupo=quantidadeGrupos+1;
        vetorVertice.resize(quantidadeGrupos);
        while (std::getline(inFile, line) && grupoAtual<quantidadeGrupos)
        {
            if(!line.empty())
            {
                data = explode(line, ';');
                vertice v;
                v.x = std::stof (data[0]);
                v.y = std::stof (data[1]);
                v.z = std::stof (data[2]);
                v.x0 = std::stof (data[3]);
                v.x1 = std::stof (data[4]);
                v.y0 = std::stof (data[5]);
                v.y1 = std::stof (data[6]);
                v.espessura = std::stof (data[7]);
                vetorVertice.at(grupoAtual).push_back(v);
            }
            else
            {
                grupoAtual++;
            }
        }
    }
    inFile.close();
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

void CalculaOrtogonal(vertice v0, vertice v1, vertice *vo)
{
    vertice temp;

    /* Encontra vetor v */
    temp.x = v1.x - v0.x;
    temp.y = v1.y - v0.y;

    /* Calculo do produto vetorial de v1 e v2 */
    vo->x = -temp.y;
    vo->y = temp.x;

    // Normaliza vetor
    float tam = sqrt( pow( (v0.x-v1.x)/2 - vo->x,2) + pow( (v0.y-v1.y)/2 - vo->y,2) );
    vo->x /= tam;
    vo->y /= tam;

}

void showMenu(){
    printf("Trabalho 1 - João Victor Guimarães e Thaynara Ferreira\n");
    printf("Use as setas DIREITA/ESQUERDA para alterar o grupo.\n");
    printf("Use as setas CIMA/BAIXO para alterar a altura.\n");
    printf("Use '.' ou ',' para alterar a espessura.\n");
    printf("Use 's' para salvar o modelo.\n");
    printf("Use 'l' para carregar um modelo.\n");
    printf("Use F12 para colocar em fullscreen.\n");
    printf("Use o scroll do mouse para zoom.\n");
    printf("Use o botão ESQUERDO do mouse para ADICIONAR pontos no 2D.\n");
    printf("Use o botão DIREITO do mouse para REMOVER pontos no 2D.\n");
    printf("Clique e arraste o mouse para rotacionar o modelo gerado em 3D.\n");
    printf("Use ESC para sair.\n");
}

void drawObject()
{
    vertice vetorNormal;
    std::vector<triangle> triangulos; //vetor para armazenar triângulos

    for(int i=0; i<vetorVertice.size(); i++) //percorre os grupos
    {
        if(!vetorVertice.at(i).empty() && vetorVertice.at(i).size()>1)//verifica se há mais de 1 ponto para desenhar as faces
        {
            for(int j=1; j<vetorVertice.at(i).size(); j++) //percorre a partir do segundo vertice e cria dois triangulos(uma face)
            {
                triangle tri;
                tri.v[0]=vetorVertice.at(i).at(j);
                tri.v[0]=vetorVertice.at(i).at(j);
                tri.v[0]=vetorVertice.at(i).at(j);
                tri.v[1]=vetorVertice.at(i).at(j-1);
                tri.v[1]=vetorVertice.at(i).at(j-1);
                tri.v[1]=vetorVertice.at(i).at(j-1);
                tri.v[2]=vetorVertice.at(i).at(j-1);
                tri.v[2]=vetorVertice.at(i).at(j-1);
                tri.v[2].z=0;
                triangulos.push_back(tri);
                ///1
                tri.v[0]=vetorVertice.at(i).at(j);
                tri.v[0]=vetorVertice.at(i).at(j);
                tri.v[0]=vetorVertice.at(i).at(j);
                tri.v[1]=vetorVertice.at(i).at(j-1);
                tri.v[1]=vetorVertice.at(i).at(j-1);
                tri.v[1].z=0;
                tri.v[2]=vetorVertice.at(i).at(j);
                tri.v[2]=vetorVertice.at(i).at(j);
                tri.v[2].z=0;
                triangulos.push_back(tri);
                ///2
                tri.v[0]=vetorVertice.at(i).at(j);
                tri.v[0]=vetorVertice.at(i).at(j);
                tri.v[0]=vetorVertice.at(i).at(j);
                tri.v[1]=vetorVertice.at(i).at(j-1);
                tri.v[1]=vetorVertice.at(i).at(j-1);
                tri.v[1]=vetorVertice.at(i).at(j-1);
                tri.v[2]=vetorVertice.at(i).at(j-1);
                tri.v[2]=vetorVertice.at(i).at(j-1);
                tri.v[2].z=0;
                triangulos.push_back(tri);
                ///3
                tri.v[0]=vetorVertice.at(i).at(j);
                tri.v[0]=vetorVertice.at(i).at(j);
                tri.v[0]=vetorVertice.at(i).at(j);
                tri.v[1]=vetorVertice.at(i).at(j-1);
                tri.v[1]=vetorVertice.at(i).at(j-1);
                tri.v[1].z=0;
                tri.v[2]=vetorVertice.at(i).at(j);
                tri.v[2]=vetorVertice.at(i).at(j);
                tri.v[2].z=0;
                triangulos.push_back(tri);
                ///4

                /*tri.v[0].x=vetorVertice.at(i).at(j).x0;
                tri.v[0].y=vetorVertice.at(i).at(j).y0;
                tri.v[0].z=vetorVertice.at(i).at(j).z;
                tri.v[1].x=vetorVertice.at(i).at(j-1).x0;
                tri.v[1].y=vetorVertice.at(i).at(j-1).y0;
                tri.v[1].z=vetorVertice.at(i).at(j-1).z;
                tri.v[2].x=vetorVertice.at(i).at(j-1).x0;
                tri.v[2].y=vetorVertice.at(i).at(j-1).y0;
                tri.v[2].z=0;
                triangulos.push_back(tri);
                ///1
                tri.v[0].x=vetorVertice.at(i).at(j).x0;
                tri.v[0].y=vetorVertice.at(i).at(j).y0;
                tri.v[0].z=vetorVertice.at(i).at(j).z;
                tri.v[1].x=vetorVertice.at(i).at(j-1).x0;
                tri.v[1].y=vetorVertice.at(i).at(j-1).y0;
                tri.v[1].z=0;
                tri.v[2].x=vetorVertice.at(i).at(j).x0;
                tri.v[2].y=vetorVertice.at(i).at(j).y0;
                tri.v[2].z=0;
                triangulos.push_back(tri);
                ///2
                tri.v[0].x=vetorVertice.at(i).at(j).x1;
                tri.v[0].y=vetorVertice.at(i).at(j).y1;
                tri.v[0].z=vetorVertice.at(i).at(j).z;
                tri.v[1].x=vetorVertice.at(i).at(j-1).x1;
                tri.v[1].y=vetorVertice.at(i).at(j-1).y1;
                tri.v[1].z=vetorVertice.at(i).at(j-1).z;
                tri.v[2].x=vetorVertice.at(i).at(j-1).x1;
                tri.v[2].y=vetorVertice.at(i).at(j-1).y1;
                tri.v[2].z=0;
                triangulos.push_back(tri);
                ///3
                tri.v[0].x=vetorVertice.at(i).at(j).x1;
                tri.v[0].y=vetorVertice.at(i).at(j).y1;
                tri.v[0].z=vetorVertice.at(i).at(j).z;
                tri.v[1].x=vetorVertice.at(i).at(j-1).x1;
                tri.v[1].y=vetorVertice.at(i).at(j-1).y1;
                tri.v[1].z=0;
                tri.v[2].x=vetorVertice.at(i).at(j).x1;
                tri.v[2].y=vetorVertice.at(i).at(j).y1;
                tri.v[2].z=0;
                triangulos.push_back(tri);
                ///4*/

                /*tri.v[0]=vetorVertice.at(i).at(j);
                tri.v[1]=vetorVertice.at(i).at(j-1);
                tri.v[2]=vetorVertice.at(i).at(j-1);
                tri.v[2].z=0;
                triangulos.push_back(tri);
                ///5
                tri.v[0]=vetorVertice.at(i).at(j);
                tri.v[1]=vetorVertice.at(i).at(j-1);
                tri.v[1].z=0;
                tri.v[2]=vetorVertice.at(i).at(j);
                tri.v[2].z=0;
                triangulos.push_back(tri);
                ///6
                tri.v[0]=vetorVertice.at(i).at(j);
                tri.v[1]=vetorVertice.at(i).at(j-1);
                tri.v[2]=vetorVertice.at(i).at(j-1);
                tri.v[2].z=0;
                triangulos.push_back(tri);
                ///7
                tri.v[0]=vetorVertice.at(i).at(j);
                tri.v[1]=vetorVertice.at(i).at(j-1);
                tri.v[1].z=0;
                tri.v[2]=vetorVertice.at(i).at(j);
                tri.v[2].z=0;
                triangulos.push_back(tri);
                ///8
                tri.v[0]=vetorVertice.at(i).at(j);
                tri.v[1]=vetorVertice.at(i).at(j-1);
                tri.v[2]=vetorVertice.at(i).at(j-1);
                tri.v[2].z=0;
                triangulos.push_back(tri);
                ///9
                tri.v[0]=vetorVertice.at(i).at(j);
                tri.v[1]=vetorVertice.at(i).at(j-1);
                tri.v[1].z=0;
                tri.v[2]=vetorVertice.at(i).at(j);
                tri.v[2].z=0;
                triangulos.push_back(tri);
                ///10
                tri.v[0]=vetorVertice.at(i).at(j);
                tri.v[1]=vetorVertice.at(i).at(j-1);
                tri.v[2]=vetorVertice.at(i).at(j-1);
                tri.v[2].z=0;
                triangulos.push_back(tri);
                ///11
                tri.v[0]=vetorVertice.at(i).at(j);
                tri.v[1]=vetorVertice.at(i).at(j-1);
                tri.v[1].z=0;
                tri.v[2]=vetorVertice.at(i).at(j);
                tri.v[2].z=0;
                triangulos.push_back(tri);
                ///12*/
            }
        }
    }

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
        float x=v.x;
        float y=v.y;
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

void imprimeTitulo(int grupo, int altura, int espessura)
{
    char aux[32];
    static char fpsBuf[256] = {0};
    sprintf(aux, "Grupo: %i, Altura: %i, Espessura: %i ", grupo, altura,espessura);
    strcpy(fpsBuf, "Desenvolvimento 1 - ");
    strcat(fpsBuf, aux);
    strcat(fpsBuf, "- Press ESC to exit.");
    glutSetWindowTitle(fpsBuf);
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

    imprimeTitulo(grupo, altura, espessura);
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
    case '.':
        espessura += 1;
        break;
    case ',':
        if(espessura <= 1)
            espessura = 1;
        else
            espessura -= 1;
        break;
    case 'l':
        //carregar
        carregarModelo("testeteste.txt");
        break;
    case 's':
        //salvar
        std::string nomeArquivo="testeteste.txt";
        printf("Digite o nome do modelo a ser salvo: \n");
        //scanf("%s", nomeArquivo);
        salvarModelo(nomeArquivo);
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
    case GLUT_KEY_F12:
        (!fullScreen) ? glutFullScreen() : glutReshapeWindow(800, 400);
        fullScreen = !fullScreen;
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
            vertice v;
            v.x=(((float)x*4)/(float)width)-1; //Normalização da coordenada X
            v.y=((((float)y*2)/(float)height)-1)*-1; //Normalização da coordenada Y
            v.z = (float) altura;
            v.espessura = espessura;
            if(vetorVertice.size() < grupo)
            {
                vetorVertice.resize(grupo);
            }

            if(!vetorVertice.at(grupo-1).empty()){
                CalculaOrtogonal(v, vetorVertice.at(grupo-1).back(), &vetorOrtogonal);
                v.x0 = v.x - vetorOrtogonal.x*espessura;
                v.y0 = v.y - vetorOrtogonal.y*espessura;
                v.x1 = v.x + vetorOrtogonal.x*espessura;
                v.y1 = v.y + vetorOrtogonal.y*espessura;
                if(vetorVertice.at(grupo-1).size()==1)
                {
                    vetorVertice.at(grupo-1).at(0).x0=v.x0;
                    vetorVertice.at(grupo-1).at(0).y0=v.y0;
                    vetorVertice.at(grupo-1).at(0).x1=v.x1;
                    vetorVertice.at(grupo-1).at(0).y1=v.y1;
                }
            }

            vetorVertice.at(grupo-1).push_back(v);
        }
        last_x = x;
        last_y = y;
    }
    if ( button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN )
    {
        if(vetorVertice.size() < grupo)
        {
            vetorVertice.resize(grupo);
        }
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
    showMenu();
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
