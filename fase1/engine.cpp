#ifdef __APPLE__
#include <GLUT/glut.h>
#else 
#include <GL/glut.h>
#endif

#include <vector>
#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
#include "engine.hpp"
#include <math.h>

// std::list<triangulo> triangulos; // Variável global para armazenar os triângulos

using namespace std;

float pos_x = 5.0, pos_y = 5.0, pos_z = 5.0; 
float look_x = 0, look_y = 0, look_z = 0;    
float up_x = 0, up_y = 1.0, up_z = 0;      
float alfa = M_PI / 9, beta2 = M_PI / 9, cradius = 7.0f;
int dxz = 1, dy = 1;
float fov = 45, near = 1, far = 1000; 
int num_files = 0;

vector<vector<Triangulo>> triangulos;

struct triangulo
{
    float pontos[3];
};

list<Triangulo> parserFigurasGeradas(string figura) {
    list<Triangulo> resp;
    ifstream f(figura);


    if(f.is_open()) {
        string linha;

        while (getline(f, linha)){
            string str_aux;
            stringstream ss(linha);
            int i = 0; 
            triangulo v;

            while(getline(ss, str_aux, ' ')){
                v.pontos[i] = stof(str_aux);
                i++;
            }

            resp.push_back(v);
        }

        f.close();
    }
    else {
        cerr << "Error: Couldn't open file: " << figura << endl;
    }
    return resp;
}

void processKeys(unsigned char c, int xx, int yy) {

// put code to process regular keys in here

}

void processSpecialKeys(int key, int xx, int yy) {

// put code to process special keys in here
	switch(key) {
		case GLUT_KEY_UP: 
			if (beta2 * (dy+1) < M_PI / 2 && beta2 * (dy+1) > -M_PI / 2 ) 
				dy += 1; 
			break;
		case GLUT_KEY_DOWN: 
			if (beta2 * (dy-1) < M_PI / 2 && beta2 * (dy-1) > -M_PI / 2 ) 
				dy -= 1;
			break;
		case GLUT_KEY_LEFT: dxz += 1; break;
		case GLUT_KEY_RIGHT: dxz -= 1; break;
		default: break;
	}
	glutPostRedisplay();
}

void desenha(vector<Triangulo> triang){
    printf("entrei em desenha()\n");
    glColor3f(1,1,1);
    glBegin(GL_TRIANGLES);

    for(vector<Triangulo>::iterator i = triang.begin(); i != triang.end(); i++) {
        float x = i->pontos[0];
        float y = i->pontos[1];
        float z = i->pontos[2];
        glVertex3f(x, y, z);
    }
    glEnd();
}

void changeSize(int w, int h) {
    if(h == 0) h = 1;

    float ratio = w * 1.0 / h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glViewport(0, 0, w, h);
    printf("[FOV]: %f, [RATIO]: %f, [NEAR]: %f, [FAR]: %f\n", fov, ratio, near, far);
    gluPerspective(fov, ratio, near, far);
    glMatrixMode(GL_MODELVIEW);
}

void renderScene(void) {
    printf("Entrei em renderScene\n");
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(pos_x, pos_y, pos_z,
                look_x, look_y, look_z,
                up_x, up_y, up_z);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin(GL_LINES);
    // x axis ir red
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(-100.0f, 0.0f, 0.0f);
        glVertex3f(100.0f, 0.0f, 0.0f);
        // y axis is green
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, -100.0f, 0.0f);
        glVertex3f(0.0f, 100.0f, 0.0f);
        // z axis is blue
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 0.0f, -100.0f);
        glVertex3f(0.0f, 0.0f, 100.0f);
    glEnd();
    // Chama função que desenha os triângulos
    for(int j = 0; j < num_files; j++) {
        desenha(triangulos[j]);
    }
    glutSwapBuffers();
}


int main(int argc, char **argv) {
    if (argc == 2) {
        list<list<Triangulo>> pontosFiguras;

        MODELO modelo = getModelo(argv[1]);

        StringArray ficheiros = getFicheiros(modelo);

        JANELA janela = getJanela(modelo);

        CAMARA camara = getCamara(modelo);

        char** arrayFicheiros = getStringsStringArray(ficheiros);
        
        int arraySize = getSizeStringArray(ficheiros);
        num_files = arraySize;
        //float fov = 45, near = 1, far = 1000; 
        fov = getCameraFOV(camara), near = getCameraNEAR(camara), far = getCameraFAR(camara);
        up_x = getCameraUpX(camara), up_y = getCameraUpY(camara), up_z = getCameraUpZ(camara);
        look_x = getPosLookAtX(camara), look_y = getPosLookAtY(camara), look_z = getPosLookAtZ(camara);
        pos_x = getPosCamaraX(camara), pos_y = getPosCamaraY(camara), pos_z = getPosCamaraZ(camara);

        list<Triangulo> triangulos_list;

        for (int i = 0; i < arraySize; ++i) {
            triangulos_list = parserFigurasGeradas(arrayFicheiros[i]);;
            pontosFiguras.push_back(triangulos_list);
        }
        
        for(const auto& lista : pontosFiguras) {
            vector<Triangulo> vector(lista.begin(), lista.end());
            triangulos.push_back(vector);
        }

        pontosFiguras.push_back(triangulos_list);
        
        // init GLUT and the window 
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
        glutInitWindowPosition(100, 100); // alterar com os do xml
        glutInitWindowSize(800, 800); // alterar para os do xml
        glutCreateWindow("PROJETOCG_GRUPO36@UM");

        // Configuração da função de render
        glutDisplayFunc(renderScene);
        glutReshapeFunc(changeSize);
            
        //  OpenGL settings
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        // glCullFace(GL_BACK);
            
        // Callback registration for keyboard processing
        glutKeyboardFunc(processKeys);
        glutSpecialFunc(processSpecialKeys);


        // enter GLUT's main cycle
        glutMainLoop();

    } else {
        cout << "Numero de argumentos inválido!" << endl;
    }
	
    return 0;
}