#include "parser.hpp"
#include <string.h>
#include "tinyxml/tinyxml.h"

struct janela{
    int x;
    int y;
};

struct camara{
    float pos[3];
    float lookat[3];
    float up[3];
    float projection[3];
};

struct array{
    char** strings;
    int size;
    int capacity;  
};

struct modelo{
    JANELA janela;
    CAMARA camara;
    StringArray ficheiros;
};

JANELA newJanela() {
    JANELA newJan = (JANELA)malloc(sizeof(struct janela));
    if (!newJan) {
        exit(1);
    } else {
        newJan->x = 0;
        newJan->y = 0;
    }
    return newJan;
}

CAMARA newCamara() {
    CAMARA newCam = (CAMARA)malloc(sizeof(struct camara));
    if (!newCam) {
        exit(1);
    } else {
        newCam->pos[0] = 0.0f;
        newCam->pos[1] = 0.0f;
        newCam->pos[2] = 0.0f;

        newCam->lookat[0] = 0.0f;
        newCam->lookat[1] = 0.0f;
        newCam->lookat[2] = 0.0f;

        newCam->up[0] = 0.0f;
        newCam->up[1] = 0.0f;
        newCam->up[2] = 1.0f;

        newCam->projection[0] = 0.0f;
        newCam->projection[1] = 0.0f;
        newCam->projection[2] = 0.0f;
    }
    return newCam;
}

StringArray createStringArray(int initialCapacity) {
    StringArray arr = (StringArray)malloc(sizeof(struct array));
    if (!arr) {
        exit(1);
    }
    arr->size = 0;
    arr->capacity = initialCapacity;
    arr->strings = (char**)malloc(initialCapacity * sizeof(char*));
    if (!arr->strings) {
        free(arr); 
        exit(1);
    }
    return arr;
}

void augmentStringArray(StringArray arr, int newCapacity) {
    if (newCapacity <= arr->capacity) {
        return; 
    }

    char** newStrings = (char**)realloc(arr->strings, newCapacity * sizeof(char*));
    if (!newStrings) {
        exit(1);
    }

    arr->strings = newStrings;
    arr->capacity = newCapacity;
}

void addString(StringArray arr, const char* str) {
    if (arr->size == arr->capacity) {
        augmentStringArray(arr, arr->capacity * 2);
    }

    arr->strings[arr->size] = (char*)malloc(strlen(str) + 1);
    if (!arr->strings[arr->size]) {
        exit(1);
    }
    strcpy(arr->strings[arr->size], str);
    arr->size++;
}

MODELO newModelo() {
    MODELO newMod = (MODELO)malloc(sizeof(struct modelo));
    if (!newMod) {
        exit(1);
    } else {
        newMod->janela = newJanela();
        newMod->camara = newCamara();
        newMod->ficheiros = createStringArray(10);
    }
    return newMod;
}

MODELO parserXML(const char* filePath) {
    MODELO mod = newModelo();
    if (mod) {
        TiXmlDocument doc;
        if (doc.LoadFile(filePath)) {
            TiXmlElement* root = doc.FirstChildElement("world"); // Adjust the root element to your XML structure

            // Camara
            TiXmlElement* camera = root->FirstChildElement("camera");
            if (camera) {
                TiXmlElement* posCamera = camera->FirstChildElement("position");
                TiXmlElement* lookAtCamera = camera->FirstChildElement("lookAt");
                TiXmlElement* upCamera = camera->FirstChildElement("up");
                TiXmlElement* projectionCamera = camera->FirstChildElement("projection");

                mod->camara->pos[0] = atof(posCamera->Attribute("x"));
                mod->camara->pos[1] = atof(posCamera->Attribute("y"));
                mod->camara->pos[2] = atof(posCamera->Attribute("z"));

                mod->camara->lookat[0] = atof(lookAtCamera->Attribute("x"));
                mod->camara->lookat[1] = atof(lookAtCamera->Attribute("y"));
                mod->camara->lookat[2] = atof(lookAtCamera->Attribute("z"));

                mod->camara->up[0] = atof(upCamera->Attribute("x"));
                mod->camara->up[1] = atof(upCamera->Attribute("y"));
                mod->camara->up[2] = atof(upCamera->Attribute("z"));

                mod->camara->projection[0] = atof(projectionCamera->Attribute("fov"));
                mod->camara->projection[1] = atof(projectionCamera->Attribute("near"));
                mod->camara->projection[2] = atof(projectionCamera->Attribute("far"));
            }
            
            // Janela
            TiXmlElement* window = root->FirstChildElement("window");
            if (window) {
                mod->janela->x = atoi(window->Attribute("width"));
                mod->janela->y = atoi(window->Attribute("height"));
            }

            //Models
            TiXmlElement* group = root->FirstChildElement("group");
            if (group) {
                TiXmlElement* models = group->FirstChildElement("models");
                for (TiXmlElement* model = models->FirstChildElement("model"); model; model = model->NextSiblingElement("model")) {
                    addString(mod->ficheiros, model->Attribute("file"));
                }
            }
        }
    }
    return mod;
}

MODELO getModelo(const char* filePath) {
    return parserXML(filePath);
}

StringArray getFicheiros(MODELO modelo){
    return modelo->ficheiros;
}

JANELA getJanela(MODELO modelo){
    return modelo->janela;
}

CAMARA getCamara(MODELO modelo) {
    return modelo->camara;
}

char** getStringsStringArray(StringArray stringArray) {
    return stringArray->strings;
}

int getSizeStringArray(StringArray stringArray) {
    return stringArray->size;
}

int getCapacityStringArray(StringArray stringArray) {
    return stringArray->capacity;
}

int getJanelaX(JANELA janela) {
    return janela->x;
}

int getJanelaY(JANELA janela) {
    return janela->y;
}

float getPosCamaraX(CAMARA camara) {
    return camara->pos[0];
}

float getPosCamaraY(CAMARA camara) {
    return camara->pos[1];
}

float getPosCamaraZ(CAMARA camara) {
    return camara->pos[2];
}

float getPosLookAtX(CAMARA camara) {
    return camara->lookat[0];
}

float getPosLookAtY(CAMARA camara) {
    return camara->lookat[1];
}

float getPosLookAtZ(CAMARA camara) {
    return camara->lookat[2];
}

float getCameraUpX(CAMARA camara) {
    return camara->up[0];
}

float getCameraUpY(CAMARA camara) {
    return camara->up[1];
}

float getCameraUpZ(CAMARA camara) {
    return camara->up[2];
}

float getCameraFOV(CAMARA camara) {
    return camara->projection[0];
}

float getCameraNEAR(CAMARA camara) {
    return camara->projection[1];
}

float getCameraFAR(CAMARA camara) {
    return camara->projection[2];
}

/*
struct camara{
    float pos[3];
    float lookat[3];
    float up[3];
    float projection[3]; // fov, near and far
};
*/