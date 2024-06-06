#include <iostream>
#include <fstream>
#include <math.h>
#include <stdlib.h>
#include <vector>
#include <string.h>

void sphere(float radius, int slices, int stacks, const char* filename) {
    FILE* file = fopen(filename, "w");
    
    if (file == NULL) {
        fprintf(stderr, "Unable to open file: %s\n", filename);
        return;
    }

    float new_alfa = (2 * M_PI) / slices, new_beta = M_PI / stacks;
    float x, y, z, x2, z2, prev_x, prev_y, prev_z, prev_x2, prev_z2;

    for (int i = 0; i < stacks; i++) {
        for (int j = 1; j <= slices; j++) {
            x = radius * sin(new_beta * i) * cos(new_alfa * j);
            y = radius * cos(new_beta * i);
            z = radius * sin(new_beta * i) * sin(new_alfa * j);
            x2 = radius * sin(new_beta * i) * cos(new_alfa * (j + 1));
            z2 = radius * sin(new_beta * i) * sin(new_alfa * (j + 1));
            prev_x = radius * sin(new_beta * (i + 1)) * cos(new_alfa * j);
            prev_y = radius * cos(new_beta * (i + 1));
            prev_z = radius * sin(new_beta * (i + 1)) * sin(new_alfa * j);
            prev_x2 = radius * sin(new_beta * (i + 1)) * cos(new_alfa * (j + 1));
            prev_z2 = radius * sin(new_beta * (i + 1)) * sin(new_alfa * (j + 1));

            fprintf(file, "%f %f %f\n%f %f %f\n%f %f %f\n", x2, y, z2, prev_x2, prev_y, prev_z2, prev_x, prev_y, prev_z);
            fprintf(file, "%f %f %f\n%f %f %f\n%f %f %f\n", prev_x, prev_y, prev_z, x, y, z, x2, y, z2);
        }
    }

    fclose(file);
}

void box(float comprimento, int grelha, const char* ficheiro_output) {
    FILE* file = fopen(ficheiro_output, "w");
    
    if (file == NULL) {
        fprintf(stderr, "Unable to open file: %s\n", ficheiro_output);
        return;
    }

    float size = comprimento / (float)grelha;
    float h_size = comprimento / 2.0;
    float px, pz, px2, pz2;

    for(int i = 0; i < grelha; i++) {
        for(int j = 0; j < grelha; j++) {
            px = -h_size + (size * j);
            pz = h_size - (size * i);
            px2 = -h_size + (size * (j + 1));
            pz2 = h_size - (size * (i + 1));

            // Top face
            fprintf(file, "%f %f %f\n%f %f %f\n%f %f %f\n", px, h_size, pz2, px, h_size, pz, px2, h_size, pz);
            fprintf(file, "%f %f %f\n%f %f %f\n%f %f %f\n", px2, h_size, pz, px2, h_size, pz2, px, h_size, pz2);

            // Bottom face
            fprintf(file, "%f %f %f\n%f %f %f\n%f %f %f\n", px2, -h_size, pz, px, -h_size, pz, px, -h_size, pz2);
            fprintf(file, "%f %f %f\n%f %f %f\n%f %f %f\n", px, -h_size, pz2, px2, -h_size, pz2, px2, -h_size, pz);

            // Front face
            fprintf(file, "%f %f %f\n%f %f %f\n%f %f %f\n", -h_size, px, pz2, -h_size, px, pz, -h_size, px2, pz);
            fprintf(file, "%f %f %f\n%f %f %f\n%f %f %f\n", -h_size, px2, pz, -h_size, px2, pz2, -h_size, px, pz2);

            // Back face
            fprintf(file, "%f %f %f\n%f %f %f\n%f %f %f\n", h_size, px2, pz, h_size, px, pz, h_size, px, pz2);
            fprintf(file, "%f %f %f\n%f %f %f\n%f %f %f\n", h_size, px, pz2, h_size, px2, pz2, h_size, px2, pz);

            // Left face
            fprintf(file, "%f %f %f\n%f %f %f\n%f %f %f\n", px, pz2, -h_size, px, pz, -h_size, px2, pz, -h_size);
            fprintf(file, "%f %f %f\n%f %f %f\n%f %f %f\n", px2, pz, -h_size, px2, pz2, -h_size, px, pz2, -h_size);

            // Right face
            fprintf(file, "%f %f %f\n%f %f %f\n%f %f %f\n", px2, pz, h_size, px, pz, h_size, px, pz2, h_size);
            fprintf(file, "%f %f %f\n%f %f %f\n%f %f %f\n", px, pz2, h_size, px2, pz2, h_size, px2, pz, h_size);
        }
    }

    fclose(file);
}

void cone(float raio, float altura, int fatias, int stacks, const char *ficheiro_output) {
    FILE* file = fopen(ficheiro_output, "w");
    
    if (file == NULL) {
        fprintf(stderr, "Unable to open file: %s\n", ficheiro_output);
        return;
    }

    float stack_height = altura / stacks;
    float angle = (2 * M_PI) / fatias;
    float px = 0, pz = 0;
    float p2x = 0, p2z = 0;
    float p3x = 0, p3z = 0;
    float p4x = 0, p4z = 0;

    // Draw base and sides
    for (int i = 0; i < fatias; i++) {
        px = raio * sin(angle * i);
        pz = raio * cos(angle * i);
        p2x = raio * sin(angle * (i + 1));
        p2z = raio * cos(angle * (i + 1));

        // Base
        fprintf(file, "%f %f %f\n%f %f %f\n%f %f %f\n", 0.0f, 0.0f, 0.0f, p2x, 0.0f, p2z, px, 0.0f, pz);

        // Side
        fprintf(file, "%f %f %f\n%f %f %f\n%f %f %f\n", 0.0f, altura, 0.0f, px, 0.0f, pz, p2x, 0.0f, p2z);
    }

    // Draw stacks
    for (int i = 1; i < stacks; i++) {
        for (int j = 0; j < fatias; j++) {
            px = (raio - (raio / stacks) * (i - 1)) * sin(angle * (j + 1));
            pz = (raio - (raio / stacks) * (i - 1)) * cos(angle * (j + 1));

            p3x = (raio - (raio / stacks) * i) * sin(angle * j);
            p3z = (raio - (raio / stacks) * i) * cos(angle * j);
            p4x = (raio - (raio / stacks) * i) * sin(angle * (j + 1));
            p4z = (raio - (raio / stacks) * i) * cos(angle * (j + 1));

            fprintf(file, "%f %f %f\n%f %f %f\n%f %f %f\n", px, stack_height * (i - 1), pz, p4x, stack_height * i, p4z, p3x, stack_height * i, p3z);
        }
    }

    fclose(file);
}


void plane(float comprimento, int divisoes, const char* ficheiro_output) {
    FILE* file = fopen(ficheiro_output, "w");
    
    if (file == NULL) {
        fprintf(stderr, "Unable to open file: %s\n", ficheiro_output);
        return;
    }

    float size = comprimento / (float)divisoes;
    float h_size = comprimento / 2.0;
    float px, pz, px2, pz2;

    for(int i = 0; i < divisoes; i++) {
        for(int j = 0; j < divisoes; j++) {
            px = -h_size + (size * j);
            pz = h_size - (size * i);
            px2 = -h_size + (size * (j + 1));
            pz2 = h_size - (size * (i + 1));

            // Top face of the plane
            fprintf(file, "%f %f %f\n%f %f %f\n%f %f %f\n", px, 0.0, pz2, px, 0.0, pz, px2, 0.0, pz);
            fprintf(file, "%f %f %f\n%f %f %f\n%f %f %f\n", px2, 0.0, pz, px2, 0.0, pz2, px, 0.0, pz2);

            // Bottom face of the plane
            fprintf(file, "%f %f %f\n%f %f %f\n%f %f %f\n", px2, 0.0, pz, px, 0.0, pz, px, 0.0, pz2);
            fprintf(file, "%f %f %f\n%f %f %f\n%f %f %f\n", px, 0.0, pz2, px2, 0.0, pz2, px2, 0.0, pz);
        }
    }

    fclose(file);
}


void generator(const char* xmlstring) {
    std::string figura, ficheiro_output;
    float raio, altura, comprimento;
    int fatias, stacks, grelha, divisoes;
    std::string input(xmlstring);

    // Find the position of the first underscore, which separates the figure name from the parameters
    size_t pos = input.find('_');
	figura = input.substr(0, pos);

    size_t start = 0, end;
    std::string token;

    // Extract parameters based on the figure type
    if (figura == "sphere") {
        start = pos + 1;
        end = input.find('_', start);
        raio = atof(input.substr(start, end - start).c_str());

        start = end + 1;
        end = input.find('_', start);
        fatias = atoi(input.substr(start, end - start).c_str());

        start = end + 1;
        end = input.find('.', start);
        stacks = atoi(input.substr(start, end - start).c_str());

        ficheiro_output = "sphere.3d";
        // Call the function to generate the sphere
        sphere(raio, fatias, stacks, ficheiro_output.c_str());
    } else if (figura == "box") {
        start = pos + 1;
        end = input.find('_', start);
        comprimento = atof(input.substr(start, end - start).c_str());

        start = end + 1;
        end = input.find('.', start);
        grelha = atoi(input.substr(start, end - start).c_str());

        ficheiro_output = "box.3d";
        // Call the function to generate the box
        box(comprimento, grelha, ficheiro_output.c_str());
    } else if (figura == "cone") {
        start = pos + 1;
        end = input.find('_', start);
        raio = atof(input.substr(start, end - start).c_str());

        start = end + 1;
        end = input.find('_', start);
        altura = atof(input.substr(start, end - start).c_str());

        start = end + 1;
        end = input.find('_', start);
        fatias = atoi(input.substr(start, end - start).c_str());

        start = end + 1;
        end = input.find('.', start);
        stacks = atoi(input.substr(start, end - start).c_str());

        ficheiro_output = "cone.3d";
        // Call the function to generate the cone
        cone(raio, altura, fatias, stacks, ficheiro_output.c_str());
    } else if (figura == "plane") {
        start = pos + 1;
        end = input.find('_', start);
        comprimento = atof(input.substr(start, end - start).c_str());

        start = end + 1;
        end = input.find('.', start);
        divisoes = atoi(input.substr(start, end - start).c_str());

        ficheiro_output = "plane.3d";
        // Call the function to generate the plane
        plane(comprimento, divisoes, ficheiro_output.c_str());
    }
}

// PATCHES

std::vector<std::vector<std::vector<float>>> readPatchesFile(const char* filePath){
    FILE* file = fopen(filePath,"r");
    std:: vector<std:: vector<std:: vector<float>>> result;
    if(file){
        char buffer[2048];
        // Obtenção do número de patches
        if(!fgets(buffer,2047,file)) return result;
        int numPatches = atoi(buffer);
        std:: vector<std:: vector<int>> indicesPerPatch; // vector de vectores, cada vector tem tamanho 16

        // Obtenção dos índices de cada patch
        for(int i = 0; i < numPatches; i++){
            if(!fgets(buffer,2047,file)) return result;
            std:: vector<int> indices;
            for(char* token = strtok(buffer,","); token; token = strtok(NULL,",")){
                indices.push_back(atoi(token));
            }
            indicesPerPatch.push_back(indices);
        }

        // Obtenção do número de pontos de controlo
        if(!fgets(buffer,2047,file)) return result;
        int numControlPoints = atoi(buffer);

        // Obtenção dos pontos de controlo
        std::vector< std::vector<float>> controlPoints;
        for(int i = 0; i < numControlPoints; i++){
            if(!fgets(buffer,2047,file)) return result;
            std:: vector<float> point;
            for(char* token = strtok(buffer,","); token; token = strtok(NULL,",")){
                point.push_back(atof(token));
            }
            controlPoints.push_back(point);
        }

        // Construção dos patches
        for(std:: vector<int> indices : indicesPerPatch){
            std:: vector< std::vector<float>> patch; 
            for(int indice : indices){
                std:: vector<float> point;
                point.push_back(controlPoints[indice][0]);
                point.push_back(controlPoints[indice][1]);

                point.push_back(controlPoints[indice][2]);
                patch.push_back(point);
            }
            result.push_back(patch);
        }
        fclose(file);
    }

    return result;
}

void multiplyMatrices(int la, int ca, const float* A, int lb, int cb, const float* B, float* R, int* lr = NULL, int* cr = NULL){ 
    if (ca == lb) {
		if (lr) *lr = ca; 
		if (cr) *cr = lb; 
		for (int i = 0; i < la; i++) {
			for (int j = 0; j < cb; j++) {
				R[i*cb+j] = 0;
				for (int k = 0; k < ca; k++) {
					R[i*cb+j] += A[i*ca+k] * B[k*cb+j];
				}
			}
		}
    }
}

void calculatePoints(float u, float v, std::vector<std::vector<float>> patch, float* res) {
    float M[16] = {-1.0f,  3.0f, -3.0f, 1.0f,
                   3.0f, -6.0f,  3.0f, 0.0f,
                  -3.0f,  3.0f,  0.0f, 0.0f,
                   1.0f,  0.0f,  0.0f, 0.0f}; 
	float U[4] = {u*u*u,u*u,u,1.0f}, V[4] = {v*v*v,v*v,v,1.0f}; 
	float UM[4]; multiplyMatrices(1,4,U,4,4,M,UM); 
	float MV[4]; multiplyMatrices(4,4,M,4,1,V,MV); 

    float P[3][16] = {{patch[0][0],patch[1][0],patch[2][0],patch[3][0],
					   patch[4][0],patch[5][0],patch[6][0],patch[7][0],
					   patch[8][0],patch[9][0],patch[10][0],patch[11][0],
					   patch[12][0],patch[13][0],patch[14][0],patch[15][0]},
					  {patch[0][1],patch[1][1],patch[2][1],patch[3][1],
					   patch[4][1],patch[5][1],patch[6][1],patch[7][1],
					   patch[8][1],patch[9][1],patch[10][1],patch[11][1],
					   patch[12][1],patch[13][1],patch[14][1],patch[15][1]},
					  {patch[0][2],patch[1][2],patch[2][2],patch[3][2],
					   patch[4][2],patch[5][2],patch[6][2],patch[7][2],
					   patch[8][2],patch[9][2],patch[10][2],patch[11][2],
					   patch[12][2],patch[13][2],patch[14][2],patch[15][2]}};

	for(int i = 0; i < 3; i++){ 
		float UMP[4]; 
		multiplyMatrices(1,4,UM,4,4,P[i],UMP);
		multiplyMatrices(1,4,UMP,4,1,MV,&res[i]);
	}

}

void buildPatches(const char* ficheiroPath,int tessellation,const char* ficheiro_output){
    FILE* file = fopen(ficheiro_output, "w");

    float u = 0.0f, v = 0.0f, delta = 1.0f/tessellation;
    float P1[3], P2[3], P3[3], P4[3];

    std:: vector< std:: vector< std:: vector<float>>> patches = readPatchesFile(ficheiroPath);

    for(std::vector<std::vector<float>> patch : patches){ 
        for(int i = 0; i < tessellation; i++, u += delta){
            for(int j = 0; j < tessellation; j++, v += delta){

                // Cálculo dos pontos
                calculatePoints(u,v,patch,P1);
                calculatePoints(u,v+delta,patch,P2);
                calculatePoints(u+delta,v,patch,P3);
                calculatePoints(u+delta,v+delta,patch,P4);

                // Triangulação
                fprintf(file, "%f %f %f\n", P3[0], P3[1], P3[2]);
                fprintf(file, "%f %f %f\n", P1[0], P1[1], P1[2]);
                fprintf(file, "%f %f %f\n", P2[0], P2[1], P2[2]);

                fprintf(file, "%f %f %f\n", P2[0], P2[1], P2[2]);
                fprintf(file, "%f %f %f\n", P4[0], P4[1], P4[2]);
                fprintf(file, "%f %f %f\n", P3[0], P3[1], P3[2]);
            }
            v = 0.0f;
        }
        u = v = 0.0f;
    }
    
    fclose(file);
}




//main
int main (int argc, char *argv[])
{
	if (argc<2)	return 101;

	//Variável auxiliar que transporta o comando da figura a ser desenhada
	std::string figura;
	//Variável auxiliar que transporta o nome do ficheiro que conterá os vértices da figura
	std::string ficheiro_output;
	//Variáveis que levam os parâmetros das figuras: (1) sphere, e (2) cone
	float raio, altura;
	int fatias, stacks;
	////Variáveis que levam os parâmetros das figuras: (1) box, e (2) plano
	float comprimento;
	int grelha, divisoes;

	/* Interpretar comando */

	//Atribuír a 'string' do pedido acerca da figura a ser "desenhada"
	figura = argv[1];
	//Fluxo de cálculo e exportação das coordenadas de uma esfera -sphere-
	if (figura == "sphere")
	{
		//Nomear parâmetros de desenho da esfera -sphere-
		raio = (float) atof (argv[2]);
		fatias = atoi (argv[3]);
		stacks =  atoi (argv[4]);
		//Nomear o parâmetro que transporta o nome do ficheiro a exportar
		ficheiro_output = argv[5];

		//Calcular pontos e exportar pontos da esfera -sphere-
		sphere (raio, fatias, stacks, ficheiro_output.c_str());
	} 
	//Fluxo de cálculo e exportação das coordenadas de uma caixa -box-
	else if (figura == "box")
	{
		//Nomear parâmetros do desenho da caixa -box-
		comprimento = (float) atof (argv[2]);
		grelha = atoi (argv[3]);
		//Nomear o parâmetro que transporta o nome do ficheiro a exportar
		ficheiro_output = argv[4];

		//Calcular pontos e exportar pontos da caixa -box-
		box (comprimento, grelha, ficheiro_output.c_str());
	} 
	/* Cone ainda não desenvolvido */
	else if (figura == "cone")
	{
		raio = (float) atof (argv[2]);
		altura = (float) atof (argv[3]);
		fatias = atoi (argv[4]);
		stacks = atoi (argv[5]);
		ficheiro_output = argv[6];

		//fazer cone
		cone (raio, altura, fatias, stacks, ficheiro_output.c_str());
	}
	//Fluxo de cálculo e exportação das coordenadas de um plano -plane-
	else if (figura == "plane")
	{	
		//Nomear parâmetros do desenho do plano -plane-
		comprimento = (float) atof (argv[2]);
		divisoes = atoi (argv[3]);
		//Nomear o parâmetro que transporta o nome do ficheiro a exportar
		ficheiro_output = argv[4];

		//Calcular pontos e exportar pontos do plano -plane-
		plane (comprimento, divisoes, ficheiro_output.c_str());
	}
    else if (figura == "patch"){
        const char* ficheiroPath = argv[2];
         int tessellation = atoi(argv[3]);
         const char* ficheiro_output = argv[4];

        
        buildPatches(ficheiroPath, tessellation, ficheiro_output);
    }

	return 0;
}
