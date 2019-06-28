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
#include "glcTexture.h"
#include "glcWavefrontObject.h"
#define NUM_OBJECTS 7

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


char objectFiles[NUM_OBJECTS][50] =
{
    "../data/texturedObj/littleCow.obj",
    "../data/texturedObj/duck.obj",
    "../data/texturedObj/fish.obj",
    "../data/texturedObj/L200.obj",
    "../data/texturedObj/plane.obj",
    "../data/texturedObj/Florest_Guardian.obj",
    "../data/texturedObj/BlueDragon.obj"
};

char textureFiles[NUM_OBJECTS][50] =
{
    "../data/texturedObj/littleCow.png",
    "../data/texturedObj/duck.png",
    "../data/texturedObj/fish.png",
    "../data/texturedObj/L200.png",
    "../data/texturedObj/plane.png",
    "../data/texturedObj/Florest_Guardian.png",
    "../data/texturedObj/BlueDragon.png"
};

// Aqui é criada a referência ao objeto que gerenciará as texturas
glcTexture *textureManager;
glcTexture *textureManager1;

glcWavefrontObject *objectManager = NULL;

int selected = 0;

/// Globals
float zdist = 5.0;
float rotationX = 0.0, rotationY = 0.0;
float alturaUsuario = 0.2;
int   last_x, last_y;
int   width, height;
float altura = 0;
int grupo = 1, espessura = 1, material = 1; //Variaveis
vertice vetorOrtogonal;
bool fullScreen = false;
bool releaseMouse = false;
bool edicao = true;
std::string nomeArquivo;

//Estrutura Grupos
std::vector< std::vector<triangle> > triangulosGrupos; //vetor para armazenar triângulos
std::vector<int> materiaisGrupos;
std::vector< std::vector<vertice> > vetorGrupos; //Estrutura utilizada para armazenar os vértices

//Camera
Camera g_camera;
bool g_key[256];
float g_translation_speed = 0.005;
float g_rotation_speed = M_PI/180*0.2;

//Cilindro
vertice v[37];

void criaTriangulo(){
    float PI = 3.1415927;
    float raio = 1.0;

    for(int k=0, g=0.0f; g<360; g+=10, k++ ){
        v[k].x = raio * cos(g * PI / 180);
        v[k].y = raio * sin(g * PI / 180);
    }

    v[36] = v[0];
}

void calculaNormalVertice(float x, float y, float z){
    float a = sqrt( pow(x,2) + pow(y,2) );
    glNormal3f(-x/a,-y/a,0);
}

void desenhaPonto3D(float x, float y, float z){
    calculaNormalVertice(x,y,z);
    glVertex3f(x,y,z);
}

void desenhaCilindro(){
    float h = 6*alturaUsuario;
    glBegin(GL_QUAD_STRIP);
    for(int k = 0 ; k < 3 ; k++){
        glTexCoord2f(k/36,0);
        desenhaPonto3D(v[k].x,v[k].y,0);

        glTexCoord2f(k+1/36,0);
        desenhaPonto3D(v[k+1].x,v[k+1].y,0);

        glTexCoord2f(k/36,1);
        desenhaPonto3D(v[k].x,v[k].y, h);

        glTexCoord2f(k+1/36,1);
        desenhaPonto3D(v[k+1].x,v[k+1].y, h);

    }
    glEnd();
}

void desenhaCirculo(){
    glDisable(GL_LIGHTING);
    glPointSize(10.0); //Define o tamanho do ponto
    glColor3f(0.0f, 1.0f, 0.0f); //Define cor do ponto
    glBegin(GL_POINTS);
    for(int k = 0 ; k < 36 ; k++){
        glVertex2f(v[k].x,v[k].y);
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

/// Functions
void init(void)
{
    glShadeModel (GL_SMOOTH);

    float pos[3] = {0.9f, alturaUsuario, 0.0f};
    initLight(width, height); // Função extra para tratar iluminação.
	g_camera.SetPos(pos[0], pos[1], pos[2]);
	g_camera.RotateYaw(16);

	//LOAD TEXTURES WALLS
    textureManager1 = new glcTexture();            // Criação do arquivo que irá gerenciar as texturas
    textureManager1->SetNumberOfTextures(1);       // Estabelece o número de texturas que será utilizado
    textureManager1->CreateTexture("../data/marble.png", 0); // Para testar magnificação, usar a imagem marble128

    // LOAD TEXTURES OBJECTS
    textureManager = new glcTexture();
    textureManager->SetNumberOfTextures(NUM_OBJECTS);
    textureManager->SetWrappingMode(GL_REPEAT);
    for(int i = 0; i < NUM_OBJECTS; i++)
        textureManager->CreateTexture( textureFiles[i], i);

    // LOAD OBJECTS
    objectManager = new glcWavefrontObject();
    objectManager->SetNumberOfObjects(NUM_OBJECTS);
    for(int i = 0; i < NUM_OBJECTS; i++)
    {
        objectManager->SelectObject(i);
        objectManager->ReadObject(objectFiles[i]);
        objectManager->Unitize();
        objectManager->FacetNormal();
        objectManager->VertexNormals(90.0);
        objectManager->Scale(5);
    }

	criaTriangulo();
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

        inFile.close();
        std::cout << "Modelo carregado com sucesso!" << std::endl;
    }
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

void imprimeTitulo(int grupo, float altura, int espessura, int material)
{
    char aux[128];
    static char fpsBuf[256] = {0};
    sprintf(aux, "Grupo: %i, Altura: %.2f, Espessura: %i , Material: %i ", grupo, altura,espessura, material);
    strcpy(fpsBuf, "Trabalho 3 - ");
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
        desenhaCirculo();
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

        //Seleciona a textura da parede
        textureManager1->Bind(0);

        setMaterials(0);
        glPushMatrix(); //Adiciona a matriz em uso no topo da pilha
            glBegin(GL_QUADS);
                glVertex3f(-1,0,1);
                glVertex3f(1,0,1);
                glVertex3f(1,0,-1);
                glVertex3f(-1,0,-1);
            glEnd();
            glPushMatrix();
                glRotatef( -90, 1.0, 0.0, 0.0 ); //Rotaciona o objeto em 3D
                drawObject(); //Desenha o objeto em 3D
                desenhaCilindro();
            glPopMatrix();
        glPopMatrix(); //Descarta a matriz no topo da pilha

        // Seleciona a textura corrente do objeto
        textureManager->Bind(selected);

        // Use selected Object
        objectManager->SelectObject(selected);
        objectManager->SetShadingMode(SMOOTH_SHADING); // Alternative: FLAT_SHADING
        objectManager->SetRenderMode(USE_TEXTURE_AND_MATERIAL);
        objectManager->Unitize();
        glPushMatrix();
            glTranslatef(0.0,0.25,0.0);
            glScalef(0.5,0.5,0.5);
            objectManager->Draw();
        glPopMatrix();

        glutSwapBuffers(); //Troca os buffers

        glutSetWindowTitle("T3 - Ambiente Virtual - Press ESC to exit.");
    }
    // Desabilita o uso de texturas
    textureManager1->Disable();
    textureManager->Disable();

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
            altura += alturaUsuario/2;
            break;
        case GLUT_KEY_DOWN:
            if(altura <= 0)
                altura = 0;
            else
                altura -= alturaUsuario/2;
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
        case GLUT_KEY_RIGHT:
            selected++;
            if(selected >= NUM_OBJECTS)
                selected = 0;
            break;
        case GLUT_KEY_LEFT:
            selected--;
            if(selected < 0)
                selected = NUM_OBJECTS-1;
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
            v.z = (float) altura;
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
