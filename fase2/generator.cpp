#include <iostream>
#include <fstream>
#include <math.h>
#include <stdlib.h>


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

	return 0;
}