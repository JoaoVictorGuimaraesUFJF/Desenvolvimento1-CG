
void initLight(int width, int height)
{
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_LIGHTING);                 // Habilita luz
   glEnable(GL_LIGHT0);                   // habilita luz 0

   // Cor da fonte de luz (RGBA)
   GLfloat cor_luz[]     = { 1.0, 1.0, 1.0, 1.0};
   // Posicao da fonte de luz. Ultimo parametro define se a luz sera direcional (0.0) ou tera uma posicional (1.0)
   GLfloat posicao_luz[] = { (float) width, (float)height, 1000.0, 1.0};

   // Define parametros da luz
   glLightfv(GL_LIGHT0, GL_AMBIENT, cor_luz);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, cor_luz);
   glLightfv(GL_LIGHT0, GL_SPECULAR, cor_luz);
   glLightfv(GL_LIGHT0, GL_POSITION, posicao_luz);

    // Quando a opção "two_side lighting" está ativada, o opengl inverte a orientação das normais
    // permitindo que tanto as faces externas quanto internas sejam iluminadas.
    // Essa opção é mais lenta se comparada a renderização de apenas um dos lados
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
}


void setMaterials(int numMaterial)
{
    if(numMaterial==0){
        GLfloat objeto_ambient[]   = { 0.25, 0.21, 0.21, 0.92 };
        GLfloat objeto_difusa[]    = { 1.0, 0.83, 0.83, 0.92 };
        GLfloat objeto_especular[] = { 0.30, 0.30, 0.30, 0.92 };
        GLfloat objeto_brilho[]    = { 11.0f };
        glMaterialfv(GL_FRONT, GL_AMBIENT, objeto_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, objeto_difusa);
        glMaterialfv(GL_FRONT, GL_SPECULAR, objeto_especular);
        glMaterialfv(GL_FRONT, GL_SHININESS, objeto_brilho);

        glMaterialfv(GL_BACK, GL_AMBIENT, objeto_ambient);
        glMaterialfv(GL_BACK, GL_DIFFUSE, objeto_difusa);
        glMaterialfv(GL_BACK, GL_SPECULAR, objeto_especular);
        glMaterialfv(GL_BACK, GL_SHININESS, objeto_brilho);
    }
    else if(numMaterial==1){
        GLfloat objeto_ambient[]   = { 0.10, 0.19, 0.17, 0.8 };
        GLfloat objeto_difusa[]    = { 0.4, 0.74, 0.69, 0.8 };
        GLfloat objeto_especular[] = { 0.3, 0.31, 0.31, 0.8 };
        GLfloat objeto_brilho[]    = { 13.0f };

        glMaterialfv(GL_FRONT, GL_AMBIENT, objeto_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, objeto_difusa);
        glMaterialfv(GL_FRONT, GL_SPECULAR, objeto_especular);
        glMaterialfv(GL_FRONT, GL_SHININESS, objeto_brilho);

        glMaterialfv(GL_BACK, GL_AMBIENT, objeto_ambient);
        glMaterialfv(GL_BACK, GL_DIFFUSE, objeto_difusa);
        glMaterialfv(GL_BACK, GL_SPECULAR, objeto_especular);
        glMaterialfv(GL_BACK, GL_SHININESS, objeto_brilho);
    }
    else if(numMaterial==2){
        GLfloat objeto_ambient[]   = { 0.23, 0.09, 0.03, 1.0 };
        GLfloat objeto_difusa[]    = { 0.55, 0.21, 0.07, 1.0 };
        GLfloat objeto_especular[] = { 0.58, 0.22, 0.07, 1.0 };
        GLfloat objeto_brilho[]    = { 51.0f };

        glMaterialfv(GL_FRONT, GL_AMBIENT, objeto_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, objeto_difusa);
        glMaterialfv(GL_FRONT, GL_SPECULAR, objeto_especular);
        glMaterialfv(GL_FRONT, GL_SHININESS, objeto_brilho);

        glMaterialfv(GL_BACK, GL_AMBIENT, objeto_ambient);
        glMaterialfv(GL_BACK, GL_DIFFUSE, objeto_difusa);
        glMaterialfv(GL_BACK, GL_SPECULAR, objeto_especular);
        glMaterialfv(GL_BACK, GL_SHININESS, objeto_brilho);
    }
    else if(numMaterial==3){
        GLfloat objeto_ambient[]   = { 0.02, 0.17, 0.02, 0.55 };
        GLfloat objeto_difusa[]    = { 0.08, 0.61, 0.08, 0.55 };
        GLfloat objeto_especular[] = { 0.63, 0.73, 0.63, 0.55 };
        GLfloat objeto_brilho[]    = { 77.0f };

        glMaterialfv(GL_FRONT, GL_AMBIENT, objeto_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, objeto_difusa);
        glMaterialfv(GL_FRONT, GL_SPECULAR, objeto_especular);
        glMaterialfv(GL_FRONT, GL_SHININESS, objeto_brilho);

        glMaterialfv(GL_BACK, GL_AMBIENT, objeto_ambient);
        glMaterialfv(GL_BACK, GL_DIFFUSE, objeto_difusa);
        glMaterialfv(GL_BACK, GL_SPECULAR, objeto_especular);
        glMaterialfv(GL_BACK, GL_SHININESS, objeto_brilho);
    }
    else if(numMaterial==4){
        GLfloat objeto_ambient[]   = { 0.17, 0.01, 0.01, 0.55 };
        GLfloat objeto_difusa[]    = { 0.61, 0.04, 0.04, 0.55 };
        GLfloat objeto_especular[] = { 0.73, 0.63, 0.63, 0.55 };
        GLfloat objeto_brilho[]    = { 77.0f };

        glMaterialfv(GL_FRONT, GL_AMBIENT, objeto_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, objeto_difusa);
        glMaterialfv(GL_FRONT, GL_SPECULAR, objeto_especular);
        glMaterialfv(GL_FRONT, GL_SHININESS, objeto_brilho);

        glMaterialfv(GL_BACK, GL_AMBIENT, objeto_ambient);
        glMaterialfv(GL_BACK, GL_DIFFUSE, objeto_difusa);
        glMaterialfv(GL_BACK, GL_SPECULAR, objeto_especular);
        glMaterialfv(GL_BACK, GL_SHININESS, objeto_brilho);
    }
    else if(numMaterial==5){
        GLfloat objeto_ambient[]   = { 0.14, 0.22, 0.16, 0.95 };
        GLfloat objeto_difusa[]    = { 0.54, 0.89, 0.63, 0.95 };
        GLfloat objeto_especular[] = { 0.32, 0.32, 0.32, 0.95 };
        GLfloat objeto_brilho[]    = { 13.0f };

        glMaterialfv(GL_FRONT, GL_AMBIENT, objeto_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, objeto_difusa);
        glMaterialfv(GL_FRONT, GL_SPECULAR, objeto_especular);
        glMaterialfv(GL_FRONT, GL_SHININESS, objeto_brilho);

        glMaterialfv(GL_BACK, GL_AMBIENT, objeto_ambient);
        glMaterialfv(GL_BACK, GL_DIFFUSE, objeto_difusa);
        glMaterialfv(GL_BACK, GL_SPECULAR, objeto_especular);
        glMaterialfv(GL_BACK, GL_SHININESS, objeto_brilho);
    }
    else if(numMaterial==6){
        GLfloat objeto_ambient[]   = { 0.33, 0.22, 0.03, 1.0 };
        GLfloat objeto_difusa[]    = { 0.78, 0.57, 0.11, 1.0 };
        GLfloat objeto_especular[] = { 0.99, 0.94, 0.81, 1.0 };
        GLfloat objeto_brilho[]    = { 28.0f };

        glMaterialfv(GL_FRONT, GL_AMBIENT, objeto_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, objeto_difusa);
        glMaterialfv(GL_FRONT, GL_SPECULAR, objeto_especular);
        glMaterialfv(GL_FRONT, GL_SHININESS, objeto_brilho);

        glMaterialfv(GL_BACK, GL_AMBIENT, objeto_ambient);
        glMaterialfv(GL_BACK, GL_DIFFUSE, objeto_difusa);
        glMaterialfv(GL_BACK, GL_SPECULAR, objeto_especular);
        glMaterialfv(GL_BACK, GL_SHININESS, objeto_brilho);
    }
}
