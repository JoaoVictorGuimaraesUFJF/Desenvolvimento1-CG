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

class objeto
{
public:
    int id, material, orientacao;
    float x,y,z;
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

//Estrutura Grupos
std::vector< std::vector<triangle> > triangulosGrupos; //vetor para armazenar triângulos
std::vector<int> materiaisGrupos;
std::vector< std::vector<vertice> > vetorGrupos; //Estrutura utilizada para armazenar os vértices

//Ler .ply
int quantVerticesPly = 0;
int quantFacesPly = 0;
std::vector< vertice > verticesPly; //Estrutura utilizada para armazenar os vértices
std::vector< std::vector<triangle> > vetorPly; //Estrutura utilizada para armazenar os objetos Ply disponiveis

//Desenhar Ply
std::vector< objeto > vetorObjetos; //Estrutura utilizada para armazenar os objetos Ply disponiveis

//Camera
Camera g_camera;
bool g_key[256];
float g_translation_speed = 0.005;
float g_rotation_speed = M_PI/180*0.2;

/// Functions
void init(void)
{
    float pos[3] = {0.0f, 0.1f, 0.0f};
    initLight(width, height); // Função extra para tratar iluminação.
	g_camera.SetPos(pos[0], pos[1], pos[2]);

}

void salvarModelo(std::string outFileName)
{
    std::ofstream outFile;
    outFile.open("../Modelos/" + outFileName, std::ifstream::out);

    if (!outFile.is_open())
    {
        std::cout << "Falha na criação do arquivo" << std::endl;
    }
    else
    {
        int quantGrupos=0;

        for(int i=0; i<vetorGrupos.size(); i++) //percorre os grupos
        {
            if(!vetorGrupos.at(i).empty())
            {
                quantGrupos++;
            }
        }
        outFile << quantGrupos << std::endl;

        outFile << vetorObjetos.size() << std::endl;

        for(int i=0; i<vetorGrupos.size(); i++) //percorre os grupos
        {
            if(!vetorGrupos.at(i).empty())
            {
                outFile << materiaisGrupos.at(i) << ";";
            }
        }
        outFile << std::endl;

        for(int i=0; i<vetorGrupos.size(); i++) //percorre os grupos
        {
            if(!vetorGrupos.at(i).empty())
            {
                for(int j=0; j<vetorGrupos.at(i).size(); j++) //percorre a partir do segundo vertice e cria dois triangulos(uma face)
                {
                    outFile << vetorGrupos.at(i).at(j).x << ";" << vetorGrupos.at(i).at(j).y << ";" << vetorGrupos.at(i).at(j).z << ";";
                    outFile << vetorGrupos.at(i).at(j).x0 << ";" << vetorGrupos.at(i).at(j).x1 << ";" << vetorGrupos.at(i).at(j).y0 << ";" << vetorGrupos.at(i).at(j).y1 << ";";
                    outFile << std::endl;

                }
                outFile << std::endl;
            }
        }

        for(int i=0; i<vetorObjetos.size(); i++) //percorre os objetos
        {
            outFile << vetorObjetos.at(i).id << ";" << vetorObjetos.at(i).material << ";" << vetorObjetos.at(i).orientacao << ";";
            outFile << vetorObjetos.at(i).x << ";" << vetorObjetos.at(i).y << ";" << vetorObjetos.at(i).z << ";";
            outFile << std::endl;
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
        vetorGrupos.clear();
        materiaisGrupos.clear();
        vetorObjetos.clear();
        std::string line;
        std::vector<std::string> data;
        int quantGrupos=0, grupoAtual=0, quantObjetos=0;
        std::getline(inFile, line);
        quantGrupos = std::stoi(line);
        std::getline(inFile, line);
        quantObjetos = std::stoi(line);
        grupo=quantGrupos+1;
        vetorGrupos.resize(quantGrupos);
        triangulosGrupos.resize(quantGrupos);

        std::getline(inFile, line);
        data = explode(line, ';');
        for(int i=0;i<quantGrupos;i++)
        {
            materiaisGrupos.push_back(std::stoi(data[i]));
        }

        while (grupoAtual<quantGrupos)
        {
            std::getline(inFile, line);
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
                vetorGrupos.at(grupoAtual).push_back(v);
            }
            else
            {
                grupoAtual++;
            }
        }

        int i=0;
        while (i<quantObjetos)
        {
            std::getline(inFile, line);
            if(!line.empty())
            {
                data = explode(line, ';');
                objeto obj;
                obj.id = std::stoi (data[0]);
                obj.material = std::stoi (data[1]);
                obj.orientacao = std::stoi (data[2]);
                obj.x = std::stof (data[3]);
                obj.y = std::stof (data[4]);
                obj.z = std::stof (data[5]);
                vetorObjetos.push_back(obj);
                i++;
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
        verticesPly.clear();
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
                        quantVerticesPly = std::stoi(data[2]);

                    }
                    else if (data[1] == "face")
                    {
                        quantFacesPly = std::stoi(data[2]);
                    }
                }
            }
        }


        for(int i = 0; i < quantVerticesPly; i++)
        {
            std::getline(inFile, line);
            data = explode(line, ' ');
            vertice v;
            v.x = std::stof (data[0]);
            v.y = std::stof (data[1]);
            v.z = std::stof (data[2]);
            verticesPly.push_back(v);
        }

        for(int i = 0; i < quantFacesPly; i++)
        {
            std::getline(inFile, line);
            data = explode(line, ' ');
            triangle tri;
            if(data[0] == "3")
            {
                tri.v[0] = verticesPly.at(std::stoi(data[1]));
                tri.v[1] = verticesPly.at(std::stoi(data[2]));
                tri.v[2] = verticesPly.at(std::stoi(data[3]));
                vetorPly.at(id).push_back(tri);
            }
            else if (data[0] == "4")
            {
                tri.v[0] = verticesPly.at(std::stoi(data[1]));
                tri.v[1] = verticesPly.at(std::stoi(data[2]));
                tri.v[2] = verticesPly.at(std::stoi(data[3]));
                vetorPly.at(id).push_back(tri);

                tri.v[0] = verticesPly.at(std::stoi(data[1]));
                tri.v[1] = verticesPly.at(std::stoi(data[3]));
                tri.v[2] = verticesPly.at(std::stoi(data[4]));
                vetorPly.at(id).push_back(tri);
            }
        }

    }
    inFile.close();
    std::cout << "Ply carregado com sucesso!" << std::endl;
}

//void calculaBoundingBox(int id){
//    //min = Coordenada minima do bounding box
//    //max = Coordenada maxima do bounding box
//    vertice min = vetorPly.at(id).at(0).v[0];
//    vertice max = vetorPly.at(id).at(0).v[0];
//
//    for (int i = 0; i < vetorPly.at(id).size(); ++i)
//    {
//        for(int j = 0; j < 3; j++)
//        {
//            if ( vetorPly.at(id).at(i).v[j].x < min.x ) min.x = vetorPly.at(id).at(i).v[j].x;
//            if ( vetorPly.at(id).at(i).v[j].y < min.y ) min.y = vetorPly.at(id).at(i).v[j].y;
//            if ( vetorPly.at(id).at(i).v[j].z < min.z ) min.z = vetorPly.at(id).at(i).v[j].z;
//            if ( vetorPly.at(id).at(i).v[j].x > max.x ) max.x = vetorPly.at(id).at(i).v[j].x;
//            if ( vetorPly.at(id).at(i).v[j].y > max.y ) max.y = vetorPly.at(id).at(i).v[j].y;
//            if ( vetorPly.at(id).at(i).v[j].z > max.z ) max.z = vetorPly.at(id).at(i).v[j].z;
//        }
//    }
//
//    alturaPly = max.y - min.y;
//}

void adicionaPLY(int id, int material, float x, float y, float z, int orientacao){
    objeto obj;
    obj.id = id;
    obj.material = material;
    obj.x = x;
    obj.y = y;
    obj.z = z;
    obj.orientacao = orientacao;
    vetorObjetos.push_back(obj);
    std::cout << "Ply posicionado!" << std::endl;
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
    for(int i=0; i<vetorGrupos.size(); i++) //percorre os grupos
    {
        if(!vetorGrupos.at(i).empty() && vetorGrupos.at(i).size()>1)//verifica se há mais de 1 ponto para desenhar as faces
        {
            for(int j=1; j<vetorGrupos.at(i).size(); j++) //percorre a partir do segundo vertice e cria dois triangulosGrupos(uma face)
            {
                triangle tri;

                tri.v[0].x=vetorGrupos.at(i).at(j).x0;
                tri.v[0].y=vetorGrupos.at(i).at(j).y0;
                tri.v[0].z=vetorGrupos.at(i).at(j).z;
                tri.v[1].x=vetorGrupos.at(i).at(j-1).x0;
                tri.v[1].y=vetorGrupos.at(i).at(j-1).y0;
                tri.v[1].z=vetorGrupos.at(i).at(j-1).z;
                tri.v[2].x=vetorGrupos.at(i).at(j-1).x0;
                tri.v[2].y=vetorGrupos.at(i).at(j-1).y0;
                tri.v[2].z=0;
                triangulosGrupos.at(i).push_back(tri);
                ///1
                tri.v[0].x=vetorGrupos.at(i).at(j).x0;
                tri.v[0].y=vetorGrupos.at(i).at(j).y0;
                tri.v[0].z=vetorGrupos.at(i).at(j).z;
                tri.v[1].x=vetorGrupos.at(i).at(j-1).x0;
                tri.v[1].y=vetorGrupos.at(i).at(j-1).y0;
                tri.v[1].z=0;
                tri.v[2].x=vetorGrupos.at(i).at(j).x0;
                tri.v[2].y=vetorGrupos.at(i).at(j).y0;
                tri.v[2].z=0;
                triangulosGrupos.at(i).push_back(tri);
                ///2
                tri.v[0].x=vetorGrupos.at(i).at(j).x1;
                tri.v[0].y=vetorGrupos.at(i).at(j).y1;
                tri.v[0].z=vetorGrupos.at(i).at(j).z;
                tri.v[1].x=vetorGrupos.at(i).at(j-1).x1;
                tri.v[1].y=vetorGrupos.at(i).at(j-1).y1;
                tri.v[1].z=vetorGrupos.at(i).at(j-1).z;
                tri.v[2].x=vetorGrupos.at(i).at(j-1).x1;
                tri.v[2].y=vetorGrupos.at(i).at(j-1).y1;
                tri.v[2].z=0;
                triangulosGrupos.at(i).push_back(tri);
                ///3
                tri.v[0].x=vetorGrupos.at(i).at(j).x1;
                tri.v[0].y=vetorGrupos.at(i).at(j).y1;
                tri.v[0].z=vetorGrupos.at(i).at(j).z;
                tri.v[1].x=vetorGrupos.at(i).at(j-1).x1;
                tri.v[1].y=vetorGrupos.at(i).at(j-1).y1;
                tri.v[1].z=0;
                tri.v[2].x=vetorGrupos.at(i).at(j).x1;
                tri.v[2].y=vetorGrupos.at(i).at(j).y1;
                tri.v[2].z=0;
                triangulosGrupos.at(i).push_back(tri);
                ///4

                tri.v[0].x=vetorGrupos.at(i).at(j-1).x0;
                tri.v[0].y=vetorGrupos.at(i).at(j-1).y0;
                tri.v[0].z=vetorGrupos.at(i).at(j-1).z;
                tri.v[1].x=vetorGrupos.at(i).at(j-1).x0;
                tri.v[1].y=vetorGrupos.at(i).at(j-1).y0;
                tri.v[1].z=0;
                tri.v[2].x=vetorGrupos.at(i).at(j-1).x1;
                tri.v[2].y=vetorGrupos.at(i).at(j-1).y1;
                tri.v[2].z=vetorGrupos.at(i).at(j-1).z;
                triangulosGrupos.at(i).push_back(tri);
                ///5
                tri.v[0].x=vetorGrupos.at(i).at(j-1).x0;
                tri.v[0].y=vetorGrupos.at(i).at(j-1).y0;
                tri.v[0].z=0;
                tri.v[1].x=vetorGrupos.at(i).at(j-1).x1;
                tri.v[1].y=vetorGrupos.at(i).at(j-1).y1;
                tri.v[1].z=vetorGrupos.at(i).at(j-1).z;
                tri.v[2].x=vetorGrupos.at(i).at(j-1).x1;
                tri.v[2].y=vetorGrupos.at(i).at(j-1).y1;
                tri.v[2].z=0;
                triangulosGrupos.at(i).push_back(tri);
                ///6
                tri.v[0].x=vetorGrupos.at(i).at(j).x0;
                tri.v[0].y=vetorGrupos.at(i).at(j).y0;
                tri.v[0].z=vetorGrupos.at(i).at(j).z;
                tri.v[1].x=vetorGrupos.at(i).at(j).x0;
                tri.v[1].y=vetorGrupos.at(i).at(j).y0;
                tri.v[1].z=0;
                tri.v[2].x=vetorGrupos.at(i).at(j).x1;
                tri.v[2].y=vetorGrupos.at(i).at(j).y1;
                tri.v[2].z=vetorGrupos.at(i).at(j).z;
                triangulosGrupos.at(i).push_back(tri);
                ///7
                tri.v[0].x=vetorGrupos.at(i).at(j).x1;
                tri.v[0].y=vetorGrupos.at(i).at(j).y1;
                tri.v[0].z=0;
                tri.v[1].x=vetorGrupos.at(i).at(j).x0;
                tri.v[1].y=vetorGrupos.at(i).at(j).y0;
                tri.v[1].z=0;
                tri.v[2].x=vetorGrupos.at(i).at(j).x1;
                tri.v[2].y=vetorGrupos.at(i).at(j).y1;
                tri.v[2].z=vetorGrupos.at(i).at(j).z;
                triangulosGrupos.at(i).push_back(tri);
                ///8
                tri.v[0].x=vetorGrupos.at(i).at(j).x0;
                tri.v[0].y=vetorGrupos.at(i).at(j).y0;
                tri.v[0].z=0;
                tri.v[1].x=vetorGrupos.at(i).at(j-1).x0;
                tri.v[1].y=vetorGrupos.at(i).at(j-1).y0;
                tri.v[1].z=0;
                tri.v[2].x=vetorGrupos.at(i).at(j).x1;
                tri.v[2].y=vetorGrupos.at(i).at(j).y1;
                tri.v[2].z=0;
                triangulosGrupos.at(i).push_back(tri);
                ///9
                tri.v[0].x=vetorGrupos.at(i).at(j-1).x1;
                tri.v[0].y=vetorGrupos.at(i).at(j-1).y1;
                tri.v[0].z=0;
                tri.v[1].x=vetorGrupos.at(i).at(j-1).x0;
                tri.v[1].y=vetorGrupos.at(i).at(j-1).y0;
                tri.v[1].z=0;
                tri.v[2].x=vetorGrupos.at(i).at(j).x1;
                tri.v[2].y=vetorGrupos.at(i).at(j).y1;
                tri.v[2].z=0;
                triangulosGrupos.at(i).push_back(tri);
                ///10
                tri.v[0].x=vetorGrupos.at(i).at(j).x0;
                tri.v[0].y=vetorGrupos.at(i).at(j).y0;
                tri.v[0].z=vetorGrupos.at(i).at(j).z;
                tri.v[1].x=vetorGrupos.at(i).at(j-1).x0;
                tri.v[1].y=vetorGrupos.at(i).at(j-1).y0;
                tri.v[1].z=vetorGrupos.at(i).at(j-1).z;
                tri.v[2].x=vetorGrupos.at(i).at(j).x1;
                tri.v[2].y=vetorGrupos.at(i).at(j).y1;
                tri.v[2].z=vetorGrupos.at(i).at(j).z;
                triangulosGrupos.at(i).push_back(tri);
                ///11
                tri.v[0].x=vetorGrupos.at(i).at(j-1).x1;
                tri.v[0].y=vetorGrupos.at(i).at(j-1).y1;
                tri.v[0].z=vetorGrupos.at(i).at(j-1).z;
                tri.v[1].x=vetorGrupos.at(i).at(j-1).x0;
                tri.v[1].y=vetorGrupos.at(i).at(j-1).y0;
                tri.v[1].z=vetorGrupos.at(i).at(j-1).z;
                tri.v[2].x=vetorGrupos.at(i).at(j).x1;
                tri.v[2].y=vetorGrupos.at(i).at(j).y1;
                tri.v[2].z=vetorGrupos.at(i).at(j).z;
                triangulosGrupos.at(i).push_back(tri);
                ///12
            }
        }
    }
    for(int i=0; i < triangulosGrupos.size(); i++)
    {
        if(!triangulosGrupos.at(i).empty() && triangulosGrupos.at(i).size()>1)//verifica se há mais de 1 ponto para desenhar as faces
        {
            setMaterials(materiaisGrupos.at(i));
            glBegin(GL_TRIANGLES);
            for(int j = 0; j < triangulosGrupos.at(i).size(); j++) // triangulosGrupos
            {
                CalculaNormal(triangulosGrupos.at(i).at(j), &vetorNormal); // Passa face triangular e endereço do vetor normal de saída
                glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
                for(int k = 0; k < 3; k++) // vertices do triangulo
                    glVertex3d(triangulosGrupos.at(i).at(j).v[k].x,triangulosGrupos.at(i).at(j).v[k].y, triangulosGrupos.at(i).at(j).v[k].z);
            }
            glEnd();
        }
        triangulosGrupos.at(i).clear();
    }
}

void drawObjectPly(int id)
{
    vertice vetorNormal;

    if(id == 0){
        glScalef(0.1,0.1,0.1);
    }else if(id == 1){
        glTranslatef(0.0,0.5,0.0);
        glScalef(0.1,0.1,0.1);
    }else if(id == 2){
        glRotatef(-90,1.0,0.0,0.0);
        glTranslatef(0.0,0.0,0.05);
        glScalef(0.001,0.001,0.001);
    }else if(id == 3){
        glScalef(0.2,0.2,0.2);
    }else if(id == 4){
        glScalef(0.04,0.04,0.04);
    }else if(id == 5){
        glRotatef(-90,1.0,0.0,0.0);
        glTranslatef(0.0,0.0,0.1);
        glScalef(0.00001,0.00001,0.00001);
    }
    glBegin(GL_TRIANGLES);
    for(int i = 0; i < vetorPly.at(id).size(); i++) // triangulos
    {
        CalculaNormal(vetorPly.at(id).at(i), &vetorNormal); // Passa face triangular e endereço do vetor normal de saída
        glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
        for(int j = 0; j < 3; j++) // vertices do triangulo
            glVertex3d(vetorPly.at(id).at(i).v[j].x,vetorPly.at(id).at(i).v[j].y, vetorPly.at(id).at(i).v[j].z);
    }
    glEnd();
}

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
    for(int i=0; i<vetorGrupos.size(); i++)
    {
        if(!vetorGrupos.at(i).empty())
        {
            for(int j=0; j<vetorGrupos.at(i).size(); j++)
            {
                desenhaPonto2D(vetorGrupos.at(i).at(j)); //Desenha o ponto em 2D
                if(vetorGrupos.at(i).size()!=1)
                {
                    desenhaPontosOrtogonais(vetorGrupos.at(i).at(j)); //Desenha os pontos ortogonais
                }
            }
        }
    }
}

void excluirPonto(int grupo) //Apaga os pontos
{
    if(!vetorGrupos.at(grupo-1).empty())
    {
        vetorGrupos.at(grupo-1).pop_back(); // Remove o ultimo elemento do vetor
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



        for(int i=0; i<vetorObjetos.size(); i++)
        {
            setMaterials(vetorObjetos.at(i).material);
            glPushMatrix();
                glTranslatef (vetorObjetos.at(i).x, 0.0, vetorObjetos.at(i).z);
                glRotatef(vetorObjetos.at(i).orientacao, 0.0, 1.0, 0.0 ); //Rotaciona o objeto em 3D
                ///glScalef (-3.0, -3.0, -3.0); //arrumar tamanho "real" de cada um
                ///arrumar posição inicial de cada um (posicionar na origem) Ex: a árvore fica metade pra baixo quando carregada diretamente
                drawObjectPly(vetorObjetos.at(i).id); //Desenha o objeto ply
            glPopMatrix();
        }

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
            int id, orientacao;
            char aux;
            printf("Lista de arquivos .ply carregados: \n");
            printf("ID - Figura\n");
//            printf("1 - Maça\n2 - Esfera\n3 - F16\n4 - Arvore\n");
//            printf("5 - Lata de lixo\n6 - Urna\n7 - Coelho\n8 - Vaca\n");
//            printf("9 - Garça\n10 - Ketchup\n11 - Caneca\n12 - Boneco de Neve\n");
            printf("1 - Maça\n2 - Arvore\n");
            printf("3 - Lata de lixo\n4 - Coelho\n5 - Vaca\n");
            printf("6 - Garça\n");
            printf("Digite o ID do arquivo .ply a ser lido: \n");
            std::cin >> id;
            printf("O chão em X é de -10 a 10 e em Z é de -10 a 10: \n");
            printf("Digite a posição em X em que o .ply deve ficar: \n");
            std::cin >> x;
            printf("Digite a posição em Z em que o .ply deve ficar: \n");
            std::cin >> z;
//            printf("O objeto voa? Digite 's' ou 'n'? \n");
//            std::cin >> aux;
//            if(aux == 's'){
//                printf("Digite a posição em Y em que o .ply deve ficar: \n");
//                std::cin >> y;
//            }else
                y = 0;
            printf("Digite a orientação (em ângulos) do .ply: \n");
            std::cin >> orientacao;

            adicionaPLY(id-1,material,x,y,z,orientacao);
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
            if(vetorGrupos.size() < grupo)
            {
                vetorGrupos.resize(grupo);
                materiaisGrupos.resize(grupo);
                triangulosGrupos.resize(grupo);
            }
            materiaisGrupos.at(grupo - 1) = material;
            if(!vetorGrupos.at(grupo - 1).empty())
            {
                CalculaOrtogonal(v, vetorGrupos.at(grupo-1).back(), &vetorOrtogonal);
                v.x0 = v.x + vetorOrtogonal.x*(-espessura)/10;
                v.y0 = v.y + vetorOrtogonal.y*(-espessura)/10;
                v.x1 = v.x + vetorOrtogonal.x*espessura/10;
                v.y1 = v.y + vetorOrtogonal.y*espessura/10;
                if(vetorGrupos.at(grupo-1).size()==1)
                {
                    vetorGrupos.at(grupo-1).front().x0=vetorGrupos.at(grupo-1).front().x + vetorOrtogonal.x*(-espessura)/10;
                    vetorGrupos.at(grupo-1).front().y0=vetorGrupos.at(grupo-1).front().y + vetorOrtogonal.y*(-espessura)/10;
                    vetorGrupos.at(grupo-1).front().x1=vetorGrupos.at(grupo-1).front().x + vetorOrtogonal.x*espessura/10;
                    vetorGrupos.at(grupo-1).front().y1=vetorGrupos.at(grupo-1).front().y + vetorOrtogonal.y*espessura/10;
                }
            }
            vetorGrupos.at(grupo-1).push_back(v);
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

    vetorPly.resize(12);
    carregaPLY("apple.ply", 0);
//    carregaPLY("sphere.ply", 1);
//    carregaPLY("f16.ply", 2);
    carregaPLY("fracttree.ply", 1);
    carregaPLY("trashcan.ply", 2);
//    carregaPLY("urn2.ply", 5);
    carregaPLY("bunny.ply", 3);
    carregaPLY("cow.ply", 4);
    carregaPLY("egret.ply", 5);
//    carregaPLY("ketchup.ply", 9);
//    carregaPLY("mug.ply", 10);
//    carregaPLY("snowman.ply", 11);

//    carregaPLY(".ply", 13);
//    carregaPLY(".ply", 14);
//    carregaPLY(".ply", 15);
//    carregaPLY(".ply", 16);
//    carregaPLY(".ply", 17);
//    carregaPLY(".ply", 18);

    showMenu();

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
