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

#include "camera.h"
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
int altura = 1, grupo = 1, espessura = 1, material = 1; //Variaveis
vertice vetorOrtogonal;
bool fullScreen = false;
bool releaseMouse = false;
bool edicao = true;
std::string nomeArquivo;
std::vector< std::vector<triangle> > triangulos; //vetor para armazenar triângulos
std::vector<int> vetorMateriais;
std::vector< std::vector<vertice> > vetorVertice; //Estrutura utilizada para armazenar os vértices

//Ler .ply
int tamanhoVetorVertices = 0;
int tamanhoVetorObjeto = 0;
std::vector< vertice > vetorVertices; //Estrutura utilizada para armazenar os vértices
std::vector< std::vector<triangle> > vetorObjetos; //Estrutura utilizada para armazenar os objetos

//Camera
Camera g_camera;
bool g_key[256];
float g_translation_speed = 0.005;
float g_rotation_speed = M_PI/180*0.2;

/// Functions
void init(void)
{
    float pos[3] = {0.0f, 0.1f, -0.1f};
    initLight(width, height); // Função extra para tratar iluminação.
	g_camera.SetPos(pos[0], pos[1], pos[2]);

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

void carregaPLY(std::string inFileName, int id)
{
    std::ifstream inFile("../plyObjects/" + inFileName);

    if (!inFile.is_open())
    {
        std::cout << "Falha na leitura do arquivo" << std::endl;
    }
    else
    {
        vetorVertices.clear();
        std::string line;
        std::vector<std::string> data;

        while (std::getline(inFile, line) && line!="end_header") //Lê cabeçalho
        {
            if(!line.empty())
            {
                data = explode(line, ' ');
                if (data[0] == "element")
                {
                    if (data[1] == "vertex")
                    {
                        tamanhoVetorVertices = std::stoi(data[2]);

                    }
                    else if (data[1] == "face")
                    {
                        tamanhoVetorObjeto = std::stoi(data[2]);
                    }
                }
            }
        }


        for(int i = 0; i < tamanhoVetorVertices; i++)
        {
            std::getline(inFile, line);
            data = explode(line, ' ');
            vertice v;
            v.x = std::stof (data[0]);
            v.y = std::stof (data[1]);
            v.z = std::stof (data[2]);
            vetorVertices.push_back(v);
        }

        for(int i = 0; i < tamanhoVetorObjeto; i++)
        {
            std::getline(inFile, line);
            data = explode(line, ' ');
            triangle tri;
            if(data[0] == "3")
            {
                tri.v[0] = vetorVertices.at(std::stoi(data[1]));
                tri.v[1] = vetorVertices.at(std::stoi(data[2]));
                tri.v[2] = vetorVertices.at(std::stoi(data[3]));
                vetorObjetos.at(id).push_back(tri);
            }
            else if (data[0] == "4")
            {
                tri.v[0] = vetorVertices.at(std::stoi(data[1]));
                tri.v[1] = vetorVertices.at(std::stoi(data[2]));
                tri.v[2] = vetorVertices.at(std::stoi(data[3]));
                vetorObjetos.at(id).push_back(tri);

                tri.v[0] = vetorVertices.at(std::stoi(data[1]));
                tri.v[1] = vetorVertices.at(std::stoi(data[3]));
                tri.v[2] = vetorVertices.at(std::stoi(data[4]));
                vetorObjetos.at(id).push_back(tri);
            }
        }

    }
    inFile.close();
    std::cout << "Ply carregado com sucesso!" << std::endl;
}

void adicionaPLY(int id, float x, float y, float z){

    ///ADICIONA O .PLY NO AMBIENTE NA POSIÇÃO X, Y, Z PASSADA POR PARAMETRO PELO USUARIO
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
    printf("Trabalho 2 - João Victor Guimarães e Thaynara Ferreira\n");
    printf("Use as setas DIREITA/ESQUERDA para alterar o grupo.\n");
    printf("Use as setas CIMA/BAIXO para alterar a altura.\n");
    printf("Use '.' ou ',' para alterar a espessura.\n");
    printf("Use 'z' ou 'x' para alterar o material do grupo.\n");
    printf("Use 's' para salvar o modelo.\n");
    printf("Use 'l' para carregar um modelo.\n");
    printf("Use 'p' para carregar um arquivo ply.\n");
    printf("Use F12 para colocar em fullscreen.\n");
    printf("Use 'm' para ativar/desativar para o modo de navegação.\n");
    printf("Use o scroll do mouse para zoom no modo de edição.\n");
    printf("Use o botão ESQUERDO do mouse para ADICIONAR pontos no 2D no modo de edição.\n");
    printf("Use o botão DIREITO do mouse para REMOVER pontos no 2D no modo de edição.\n");
    printf("Clique e arraste o mouse para rotacionar o modelo gerado em 3D no modo de edição\n");
    printf("Clique e arraste o mouse para rotacionar a câmera no modo de navegação.\n");
    printf("Use ESC para sair.\n");
}

void drawObject()
{
    vertice vetorNormal;
    for(int i=0; i<vetorVertice.size(); i++) //percorre os grupos
    {
        if(!vetorVertice.at(i).empty() && vetorVertice.at(i).size()>1)//verifica se há mais de 1 ponto para desenhar as faces
        {
            for(int j=1; j<vetorVertice.at(i).size(); j++) //percorre a partir do segundo vertice e cria dois triangulos(uma face)
            {
                triangle tri;

                tri.v[0].x=vetorVertice.at(i).at(j).x0;
                tri.v[0].y=vetorVertice.at(i).at(j).y0;
                tri.v[0].z=vetorVertice.at(i).at(j).z;
                tri.v[1].x=vetorVertice.at(i).at(j-1).x0;
                tri.v[1].y=vetorVertice.at(i).at(j-1).y0;
                tri.v[1].z=vetorVertice.at(i).at(j-1).z;
                tri.v[2].x=vetorVertice.at(i).at(j-1).x0;
                tri.v[2].y=vetorVertice.at(i).at(j-1).y0;
                tri.v[2].z=0;
                triangulos.at(i).push_back(tri);
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
                triangulos.at(i).push_back(tri);
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
                triangulos.at(i).push_back(tri);
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
                triangulos.at(i).push_back(tri);
                ///4

                tri.v[0].x=vetorVertice.at(i).at(j-1).x0;
                tri.v[0].y=vetorVertice.at(i).at(j-1).y0;
                tri.v[0].z=vetorVertice.at(i).at(j-1).z;
                tri.v[1].x=vetorVertice.at(i).at(j-1).x0;
                tri.v[1].y=vetorVertice.at(i).at(j-1).y0;
                tri.v[1].z=0;
                tri.v[2].x=vetorVertice.at(i).at(j-1).x1;
                tri.v[2].y=vetorVertice.at(i).at(j-1).y1;
                tri.v[2].z=vetorVertice.at(i).at(j-1).z;
                triangulos.at(i).push_back(tri);
                ///5
                tri.v[0].x=vetorVertice.at(i).at(j-1).x0;
                tri.v[0].y=vetorVertice.at(i).at(j-1).y0;
                tri.v[0].z=0;
                tri.v[1].x=vetorVertice.at(i).at(j-1).x1;
                tri.v[1].y=vetorVertice.at(i).at(j-1).y1;
                tri.v[1].z=vetorVertice.at(i).at(j-1).z;
                tri.v[2].x=vetorVertice.at(i).at(j-1).x1;
                tri.v[2].y=vetorVertice.at(i).at(j-1).y1;
                tri.v[2].z=0;
                triangulos.at(i).push_back(tri);
                ///6
                tri.v[0].x=vetorVertice.at(i).at(j).x0;
                tri.v[0].y=vetorVertice.at(i).at(j).y0;
                tri.v[0].z=vetorVertice.at(i).at(j).z;
                tri.v[1].x=vetorVertice.at(i).at(j).x0;
                tri.v[1].y=vetorVertice.at(i).at(j).y0;
                tri.v[1].z=0;
                tri.v[2].x=vetorVertice.at(i).at(j).x1;
                tri.v[2].y=vetorVertice.at(i).at(j).y1;
                tri.v[2].z=vetorVertice.at(i).at(j).z;
                triangulos.at(i).push_back(tri);
                ///7
                tri.v[0].x=vetorVertice.at(i).at(j).x1;
                tri.v[0].y=vetorVertice.at(i).at(j).y1;
                tri.v[0].z=0;
                tri.v[1].x=vetorVertice.at(i).at(j).x0;
                tri.v[1].y=vetorVertice.at(i).at(j).y0;
                tri.v[1].z=0;
                tri.v[2].x=vetorVertice.at(i).at(j).x1;
                tri.v[2].y=vetorVertice.at(i).at(j).y1;
                tri.v[2].z=vetorVertice.at(i).at(j).z;
                triangulos.at(i).push_back(tri);
                ///8
                tri.v[0].x=vetorVertice.at(i).at(j).x0;
                tri.v[0].y=vetorVertice.at(i).at(j).y0;
                tri.v[0].z=0;
                tri.v[1].x=vetorVertice.at(i).at(j-1).x0;
                tri.v[1].y=vetorVertice.at(i).at(j-1).y0;
                tri.v[1].z=0;
                tri.v[2].x=vetorVertice.at(i).at(j).x1;
                tri.v[2].y=vetorVertice.at(i).at(j).y1;
                tri.v[2].z=0;
                triangulos.at(i).push_back(tri);
                ///9
                tri.v[0].x=vetorVertice.at(i).at(j-1).x1;
                tri.v[0].y=vetorVertice.at(i).at(j-1).y1;
                tri.v[0].z=0;
                tri.v[1].x=vetorVertice.at(i).at(j-1).x0;
                tri.v[1].y=vetorVertice.at(i).at(j-1).y0;
                tri.v[1].z=0;
                tri.v[2].x=vetorVertice.at(i).at(j).x1;
                tri.v[2].y=vetorVertice.at(i).at(j).y1;
                tri.v[2].z=0;
                triangulos.at(i).push_back(tri);
                ///10
                tri.v[0].x=vetorVertice.at(i).at(j).x0;
                tri.v[0].y=vetorVertice.at(i).at(j).y0;
                tri.v[0].z=vetorVertice.at(i).at(j).z;
                tri.v[1].x=vetorVertice.at(i).at(j-1).x0;
                tri.v[1].y=vetorVertice.at(i).at(j-1).y0;
                tri.v[1].z=vetorVertice.at(i).at(j-1).z;
                tri.v[2].x=vetorVertice.at(i).at(j).x1;
                tri.v[2].y=vetorVertice.at(i).at(j).y1;
                tri.v[2].z=vetorVertice.at(i).at(j).z;
                triangulos.at(i).push_back(tri);
                ///11
                tri.v[0].x=vetorVertice.at(i).at(j-1).x1;
                tri.v[0].y=vetorVertice.at(i).at(j-1).y1;
                tri.v[0].z=vetorVertice.at(i).at(j-1).z;
                tri.v[1].x=vetorVertice.at(i).at(j-1).x0;
                tri.v[1].y=vetorVertice.at(i).at(j-1).y0;
                tri.v[1].z=vetorVertice.at(i).at(j-1).z;
                tri.v[2].x=vetorVertice.at(i).at(j).x1;
                tri.v[2].y=vetorVertice.at(i).at(j).y1;
                tri.v[2].z=vetorVertice.at(i).at(j).z;
                triangulos.at(i).push_back(tri);
                ///12
            }
        }
    }
    for(int i=0; i < triangulos.size(); i++)
    {
        if(!triangulos.at(i).empty() && triangulos.at(i).size()>1)//verifica se há mais de 1 ponto para desenhar as faces
        {
            setMaterials(vetorMateriais.at(i));
            glBegin(GL_TRIANGLES);
            for(int j = 0; j < triangulos.at(i).size(); j++) // triangulos
            {
                CalculaNormal(triangulos.at(i).at(j), &vetorNormal); // Passa face triangular e endereço do vetor normal de saída
                glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
                for(int k = 0; k < 3; k++) // vertices do triangulo
                    glVertex3d(triangulos.at(i).at(j).v[k].x,triangulos.at(i).at(j).v[k].y, triangulos.at(i).at(j).v[k].z);
            }
            glEnd();
        }
        triangulos.at(i).clear();
    }
}

//void calculaBoundingBox(int numObjeto, vertice *verticeMin, vertice *verticeMax)
//{
//    // min = Coordenada minima do bounding box
//    // max = Coordenada maxima do bounding box
//    vertice min = vetorObjetos.at(numObjeto).at(0).v[0];
//    vertice max = vetorObjetos.at(numObjeto).at(0).v[0];
//    for (int i = 0; i < vetorObjetos.at(numObjeto).size(); ++i)
//    {
//        for(int j = 0; j < 3; j++)
//        {
//            if ( vetorObjetos.at(numObjeto).at(i).v[j].x < min.x ) min.x = vetorObjetos.at(numObjeto).at(i).v[j].x;
//            if ( vetorObjetos.at(numObjeto).at(i).v[j].y < min.y ) min.y = vetorObjetos.at(numObjeto).at(i).v[j].y;
//            if ( vetorObjetos.at(numObjeto).at(i).v[j].z < min.z ) min.z = vetorObjetos.at(numObjeto).at(i).v[j].z;
//            if ( vetorObjetos.at(numObjeto).at(i).v[j].x > max.x ) max.x = vetorObjetos.at(numObjeto).at(i).v[j].x;
//            if ( vetorObjetos.at(numObjeto).at(i).v[j].y > max.y ) max.y = vetorObjetos.at(numObjeto).at(i).v[j].y;
//            if ( vetorObjetos.at(numObjeto).at(i).v[j].z > max.z ) max.z = vetorObjetos.at(numObjeto).at(i).v[j].z;
//        }
//    }
//    verticeMin->x = min.x;
//    verticeMin->y = min.y;
//    verticeMin->z = min.z;
//    verticeMax->x = max.x;
//    verticeMax->y = max.y;
//    verticeMax->z = max.z;
//}

void desenhaEixos() //Desenha os eixos
{
    glDisable(GL_LIGHTING); ///X VERMELHO
    glBegin(GL_LINES);
    glColor3f(1.0,0.0,0.0);
    glVertex3f(-100.0,0.0,0.0);
    glVertex3f(100.0,0.0,0.0);
    glEnd();

    glBegin(GL_LINES); ///Y VERDE
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

void imprimeTitulo(int grupo, int altura, int espessura, int material)
{
    char aux[128];
    static char fpsBuf[256] = {0};
    sprintf(aux, "Grupo: %i, Altura: %i, Espessura: %i , Material: %i ", grupo, altura,espessura, material);
    strcpy(fpsBuf, "Desenvolvimento 1 - ");
    strcat(fpsBuf, aux);
    strcat(fpsBuf, "- Press ESC to exit.");
    glutSetWindowTitle(fpsBuf);
}

void display(void)
{
    if(edicao)
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

        imprimeTitulo(grupo, altura, espessura, material);
    }
    else
    {
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Limpa o buffer de cor
        glDisable(GL_SCISSOR_TEST);
        glViewport (0, 0, (GLsizei) width, (GLsizei) height); //Define a visualização na janela da aplicação
        glClearColor(0.0,0.0,0.0,0.0); //Define cor de fundo da viewport

        glMatrixMode (GL_PROJECTION); //Matriz de projeção
        glLoadIdentity (); //Matriz identidade
        gluPerspective(60.0, (GLfloat) width/(GLfloat) height, 0.01, 200.0); //Define projeções perspectivas

        glMatrixMode (GL_MODELVIEW); //Matriz de Desenho
        glLoadIdentity(); //Matriz identidade
        g_camera.Refresh();

        setMaterials(0);
        glPushMatrix(); //Adiciona a matriz em uso no topo da pilha
            glBegin(GL_QUADS);
                glVertex3f(-10,0,10);
                glVertex3f(10,0,10);
                glVertex3f(10,0,-10);
                glVertex3f(-10,0,-10);
            glEnd();
            glPushMatrix();
                glRotatef( -90, 1.0, 0.0, 0.0 ); //Rotaciona o objeto em 3D
                drawObject(); //Desenha o objeto em 3D
            glPopMatrix();
        glPopMatrix(); //Descarta a matriz no topo da pilha

        glutSwapBuffers(); //Troca os buffers

        glutSetWindowTitle("T2 - Ambiente Virtual - Press ESC to exit.");
    }
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
    if(edicao)
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
        case 'm':
            if(edicao)
                edicao = false;
            else
                edicao = true;
            break;
        case 'p':
            float x, y, z;
            int id;
            char aux;
            printf("Lista de arquivos .ply carregados: \n");
            printf("ID - Figura\n");
            printf("1 - Maça\n2 - Esfera\n3 - F16\n4 - Arvore\n");
            printf("5 - Lata de lixo\n6 - Urna\n7 - Coelho\n8 - Vaca\n");
            printf("9 - Garça\n10 - Ketchup\n11 - Caneca\n12 - Boneco de Neve\n");
            printf("Digite o ID do arquivo .ply a ser lido: \n");
            std::cin >> id;
            printf("O chão em X é de -10 a 10 e em Z é de -10 a 10: \n");
            printf("Digite a posição em X em que o .ply deve ficar: \n");
            std::cin >> x;
            printf("Digite a posição em Z em que o .ply deve ficar: \n");
            std::cin >> z;
            printf("O objeto voa? Digite 's' ou 'n'? \n");
            std::cin >> aux;
            if(aux == 's'){
                printf("Digite a posição em Y em que o .ply deve ficar: \n");
                std::cin >> y;
            }else
                y = 0;

            exit(0);
            //adicionaPLY(id,x,y,z);
            break;
        case 'x':
            if(material >= 6)
                material = 6;
            else
                material += 1;
            break;
        case 'z':
            if(material <= 1)
                material = 1;
            else
                material -= 1;
            break;
        }
    }
    else
    {
        switch (tolower(key))
        {
        case 27:
            exit(0);
            break;
        case 'm':
            if(edicao)
                edicao = false;
            else
                edicao = true;
            break;
        }
    }
    g_key[key] = true;
}

// Special Keys callback
void specialKeys(int key, int x, int y)
{
    if(edicao)
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
    }
    else
    {
        switch(key)
        {
        case GLUT_KEY_F12:
            (!fullScreen) ? glutFullScreen() : glutReshapeWindow(800, 400);
            fullScreen = !fullScreen;
            break;
        }
    }
    glutPostRedisplay();
}

void KeyboardUp(unsigned char key, int x, int y)
{
    g_key[key] = false;
}

void Timer(int value)
{
    float speed = g_translation_speed;

    if(g_key['w'] || g_key['W'])
    {
        g_camera.Move(speed);
    }
    else if(g_key['s'] || g_key['S'])
    {
        g_camera.Move(-speed);
    }
    else if(g_key['a'] || g_key['A'])
    {
        g_camera.Strafe(speed);
    }
    else if(g_key['d'] || g_key['D'])
    {
        g_camera.Strafe(-speed);
    }

    glutTimerFunc(1, Timer, 0);
}

// Motion callback
void motion(int x, int y )
{
    if(edicao)
    {
        rotationX += (float) (y - last_y);
        rotationY += (float) (x - last_x);

        last_x = x;
        last_y = y;
    }
    else
    {
        static bool just_warped = false;

        if(just_warped)
        {
            just_warped = false;
            return;
        }

        int dx = x - width/2;
        int dy = height/2-y;

        if(dx) g_camera.RotateYaw(g_rotation_speed*dx);
        if(dy) g_camera.RotatePitch(g_rotation_speed*dy);

        if(!releaseMouse)	glutWarpPointer(width/2, height/2);

        just_warped = true;
    }
}

// Mouse callback
void mouse(int button, int state, int x, int y)
{
    if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
    {
        if(edicao == true && x<=width/2)
        {
            vertice v;
            v.x=(((float)x*4)/(float)width)-1; //Normalização da coordenada X
            v.y=((((float)y*2)/(float)height)-1)*-1; //Normalização da coordenada Y
            v.z = (float) altura/5;
            if(vetorVertice.size() < grupo)
            {
                vetorVertice.resize(grupo);
                vetorMateriais.resize(grupo);
                triangulos.resize(grupo);
            }
            vetorMateriais.at(grupo - 1) = material;
            if(!vetorVertice.at(grupo - 1).empty())
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

    vetorObjetos.resize(13);
    carregaPLY("apple.ply", 1);
    carregaPLY("sphere.ply", 2);
    carregaPLY("f16.ply", 3);
    carregaPLY("fracttree.ply", 4);
    carregaPLY("trashcan.ply", 5);
    carregaPLY("urn2.ply", 6);
    carregaPLY("bunny.ply", 7);
    carregaPLY("cow.ply", 8);
    carregaPLY("egret.ply", 9);
    carregaPLY("ketchup.ply", 10);
    carregaPLY("mug.ply", 11);
    carregaPLY("snowman.ply", 12);

//    carregaPLY(".ply", 13);
//    carregaPLY(".ply", 14);
//    carregaPLY(".ply", 15);
//    carregaPLY(".ply", 16);
//    carregaPLY(".ply", 17);
//    carregaPLY(".ply", 18);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc( mouse );
    glutMotionFunc( motion );
    glutKeyboardFunc(keyboard);
    glutSpecialFunc( specialKeys );
    glutIdleFunc(idle);
    glutKeyboardUpFunc(KeyboardUp);
    glutTimerFunc(1, Timer, 0);
    glutMainLoop();
    return 0;
}
