#ifndef PARSER
#define PARSER

#include <stdlib.h>

typedef struct modelo* MODELO;

typedef struct janela* JANELA;

typedef struct camara* CAMARA;

typedef struct array* StringArray;

MODELO getModelo(const char* filePath);

StringArray getFicheiros(MODELO modelo);

JANELA getJanela(MODELO modelo);

CAMARA getCamara(MODELO modelo);

char** getStringsStringArray(StringArray stringArray);

int getSizeStringArray(StringArray stringArray);

int getCapacityStringArray(StringArray stringArray);

int getJanelaX(JANELA janela);

int getJanelaY(JANELA janela);

float getPosCamaraX(CAMARA camara);

float getPosCamaraY(CAMARA camara);

float getPosCamaraZ(CAMARA camara);

float getPosLookAtX(CAMARA camara);

float getPosLookAtY(CAMARA camara);

float getPosLookAtZ(CAMARA camara);

float getCameraUpX(CAMARA camara);

float getCameraUpY(CAMARA camara);

float getCameraUpZ(CAMARA camara);

float getCameraFOV(CAMARA camara);

float getCameraNEAR(CAMARA camara);

float getCameraFAR(CAMARA camara);

#endif