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
std::string nomeArquivo;
int objetoAtual = 0;
std::vector<vertice> vetorVertice; //Estrutura utilizada para armazenar os vértices
std::vector< std::vector<triangle> > vetorObjetos;

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
    outFile.open("../Modelos/" + outFileName, std::ifstream::out);

    if (!outFile.is_open())
    {
        std::cout << "Falha na criação do arquivo" << std::endl;
    }
    else
    {
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
                    outFile << std::endl;

                }
                outFile << std::endl;
            }
        }
        outFile.close();
        std::cout << "Arquivo salvo com sucesso na pasta Modelos!" << std::endl;
    }
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

    std::ifstream inFile("../Modelos/" + inFileName);

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
                vetorVertice.at(grupoAtual).push_back(v);
            }
            else
            {
                grupoAtual++;
            }
        }
        inFile.close();
        std::cout << "Modelo carregado com sucesso!" << std::endl;
    }
}

void carregaPLY(std::string inFileName, int numObjeto)
{
    std::ifstream inFile("../plyObjects/" + inFileName);

    if (!inFile.is_open())
    {
        std::cout << "Falha na leitura do arquivo" << std::endl;
    }
    else
    {
        vetorVertice.clear();
        std::string line;
        std::vector<std::string> data;

        while (std::getline(inFile, line) && line!="end_header") //Lê cabeçalho
        {
            if(!line.empty())
            {
                data = explode(line, ' ');
                if (equal_strings (data[0], "element"))
                {
                    if (equal_strings (data[1], "vertex"))
                    {
                        vetorVertice.resize(std::stoi(data[2]));
                    }
                    else if (equal_strings (data[1], "face"))
                    {
                        vetorObjetos.at(numObjeto).resize(std::stoi(data[2]));
                    }
                }
            }
        }

        for(int i = 0; i < vetorVertice.size(); i++){
            std::getline(inFile, line);
            data = explode(line, ' ');
            vertice v;
            v.x = std::stof (data[0]);
            v.y = std::stof (data[1]);
            v.z = std::stof (data[2]);
            vetorVertice.push_back(v);
        }

        for(int i = 0; i < vetorObjetos.at(numObjeto).size(); i++){
            std::getline(inFile, line);
            data = explode(line, ' ');
            triangle tri;
            if(data[0] == 3){
                tri.x = vetorVertice.at(std::stoi(data[1]));
                tri.y = vetorVertice.at(std::stoi(data[2]));
                tri.z = vetorVertice.at(std::stoi(data[3]));
                vetorObjetos.at(numObjeto).push_back(tri);
            }
            else if (data[0] == 4){
                tri.x = vetorVertice.at(std::stoi(data[1]));
                tri.y = vetorVertice.at(std::stoi(data[2]));
                tri.z = vetorVertice.at(std::stoi(data[3]));
                vetorObjetos.at(numObjeto).push_back(tri);

                tri.x = vetorVertice.at(std::stoi(data[2]));
                tri.y = vetorVertice.at(std::stoi(data[3]));
                tri.z = vetorVertice.at(std::stoi(data[4]));
                vetorObjetos.at(numObjeto).push_back(tri);

            }
        }

    }
    inFile.close();
    std::cout << "Modelo carregado com sucesso!" << std::endl;
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

void showMenu()
{
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

void drawObject(int numObjeto)
{
    vertice vetorNormal;

    glBegin(GL_TRIANGLES);
    for(int i = 0; i < vetorObjetos.at(numObjeto).size(); i++) // triangulos
    {
        CalculaNormal(vetorObjetos.at(numObjeto).at(i), &vetorNormal); // Passa face triangular e endereço do vetor normal de saída
        glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
        for(int j = 0; j < 3; j++) // vertices do triangulo
            glVertex3d(vetorObjetos.at(numObjeto).at(i).v[j].x,vetorObjetos.at(numObjeto).at(i).v[j].y, vetorObjetos.at(numObjeto).at(i).v[j].z);
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

void desenhaPontosOrtogonais(vertice v) //Função auxiliar de desenho
{
    glDisable(GL_LIGHTING);
    glPointSize(10.0); //Define o tamanho do ponto
    glColor3f(0.0f, 1.0f, 1.0f); //Define cor do ponto
    float x=v.x0;
    float y=v.y0;
    glBegin(GL_POINTS);
    glVertex2f(x,y); //Cria o ponto nas coordenadas X e Y
    glEnd();
    x=v.x1;
    y=v.y1;
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
                if(vetorVertice.at(i).size()!=1)
                {
                    desenhaPontosOrtogonais(vetorVertice.at(i).at(j)); //Desenha os pontos ortogonais
                }
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

    glViewport (0, 0, (GLsizei) width, (GLsizei) height); //Define a visualização na janela da aplicação
    glClearColor(0.0,0.0,0.0,0.0); //Define cor de fundo da viewport

    glMatrixMode (GL_PROJECTION); //Matriz de projeção
    glLoadIdentity (); //Matriz identidade
    gluPerspective(60.0, (GLfloat) width/(GLfloat) height, 0.01, 200.0); //Define projeções perspectivas

    glMatrixMode (GL_MODELVIEW); //Matriz de Desenho
    glLoadIdentity(); //Matriz identidade
    gluLookAt (0.0, 0.0, zdist, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); //Define a camera com olho, foco e orientação(up)



    glPushMatrix(); //Adiciona a matriz em uso no topo da pilha
        glRotatef( rotationY, 0.0, 1.0, 0.0 ); //Rotaciona o objeto em 3D
        glRotatef( rotationX, 1.0, 0.0, 0.0 ); //Rotaciona o objeto em 3D
        drawObject(objetoAtual); //Desenha o objeto em 3D
    glPopMatrix(); //Descarta a matriz no topo da pilha

    glutSwapBuffers(); //Troca os buffers
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
        printf("Digite o nome do modelo a ser carregado: \n");
        std::cin >> nomeArquivo;
        carregarModelo(nomeArquivo);
        break;
    case 's':
        //salvar
        printf("Digite o nome do modelo a ser salvo: \n");
        std::cin >> nomeArquivo;
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
        if(x<=width/2)
        {
            vertice v;
            v.x=(((float)x*4)/(float)width)-1; //Normalização da coordenada X
            v.y=((((float)y*2)/(float)height)-1)*-1; //Normalização da coordenada Y
            v.z = (float) altura/5;
            if(vetorVertice.size() < grupo)
            {
                vetorVertice.resize(grupo);
            }

            if(!vetorVertice.at(grupo-1).empty())
            {
                CalculaOrtogonal(v, vetorVertice.at(grupo-1).back(), &vetorOrtogonal);
                v.x0 = v.x + vetorOrtogonal.x*(-espessura)/10;
                v.y0 = v.y + vetorOrtogonal.y*(-espessura)/10;
                v.x1 = v.x + vetorOrtogonal.x*espessura/10;
                v.y1 = v.y + vetorOrtogonal.y*espessura/10;
                if(vetorVertice.at(grupo-1).size()==1)
                {
                    vetorVertice.at(grupo-1).front().x0=vetorVertice.at(grupo-1).front().x + vetorOrtogonal.x*(-espessura)/10;
                    vetorVertice.at(grupo-1).front().y0=vetorVertice.at(grupo-1).front().y + vetorOrtogonal.y*(-espessura)/10;
                    vetorVertice.at(grupo-1).front().x1=vetorVertice.at(grupo-1).front().x + vetorOrtogonal.x*espessura/10;
                    vetorVertice.at(grupo-1).front().y1=vetorVertice.at(grupo-1).front().y + vetorOrtogonal.y*espessura/10;
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

    vetorObjetos.resize(6);
    carregaPLY("budda.ply", 0);
    carregaPLY("bunny.ply", 1);
    carregaPLY("cow.ply", 2);
    carregaPLY("dragon.ply", 3);
    carregaPLY("dragon_full.ply", 4);
    carregaPLY("snowman.ply", 5);

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
