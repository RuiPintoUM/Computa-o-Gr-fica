#include <iostream>
#include <fstream>
#include <math.h>
#include <stdlib.h>
#include <vector>
#include <string.h>

#include <cmath>
#include <cstdio>

void sphere(float radius, int slices, int stacks, const char* filename) {
    FILE* file = fopen(filename, "w");
    
    if (file == NULL) {
        fprintf(stderr, "Unable to open file: %s\n", filename);
        return;
    }

    float new_alfa = (2 * M_PI) / slices, new_beta = M_PI / stacks;
    float x, y, z, x2, z2, prev_x, prev_y, prev_z, prev_x2, prev_z2;

    float norm_x, norm_y, norm_z, norm_x2, norm_z2, norm_prev_x, norm_prev_y, norm_prev_z, norm_prev_x2, norm_prev_z2;

    int num_points = 0;

    char* buffer = NULL;
    size_t buffer_size = 0;
    FILE* temp = open_memstream(&buffer, &buffer_size);

    if (temp == NULL) {
        fprintf(stderr, "Failed to create memory stream\n");
        fclose(file);
        return;
    }

    for (int i = 0; i < stacks; i++) {
        for (int j = 1; j <= slices; j++) {
            // Vertices
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

            // Normais 
            norm_x = x / radius;
            norm_y = y / radius;
            norm_z = z / radius;
            
            norm_x2 = x2 / radius;
            norm_z2 = z2 / radius;
            
            norm_prev_x = prev_x / radius;
            norm_prev_y = prev_y / radius;
            norm_prev_z = prev_z / radius;

            norm_prev_x2 = prev_x2 / radius;
            norm_prev_z2 = prev_z2 / radius;

            // Texturas

            float u0 = 0.25f - (float)j / slices;
            float u1 = 0.25f - (float)(j + 1) / slices;
            float v0 = 1.0f - (float)i / stacks;
            float v1 = 1.0f - (float)(i + 1) / stacks;


            fprintf(temp, "%f %f %f ; %f %f %f ; %f %f\n", x2, y, z2, norm_x2, norm_y, norm_z2, u1, v0);
            fprintf(temp, "%f %f %f ; %f %f %f ; %f %f\n", prev_x2, prev_y, prev_z2, norm_prev_x2, norm_prev_y, norm_prev_z2, u1, v1);
            fprintf(temp, "%f %f %f ; %f %f %f ; %f %f\n", prev_x, prev_y, prev_z, norm_prev_x, norm_prev_y, norm_prev_z, u0, v1);

            fprintf(temp, "%f %f %f ; %f %f %f ; %f %f\n", prev_x, prev_y, prev_z, norm_prev_x, norm_prev_y, norm_prev_z, u0, v1);
            fprintf(temp, "%f %f %f ; %f %f %f ; %f %f\n", x, y, z, norm_x, norm_y, norm_z, u0, v0);
            fprintf(temp, "%f %f %f ; %f %f %f ; %f %f\n", x2, y, z2, norm_x2, norm_y, norm_z2, u1, v0);
            num_points += 6;
        }
    }

    fclose(temp);

    fprintf(file, "%d\n", num_points);
    fwrite(buffer, sizeof(char), buffer_size, file);

    free(buffer);
    fclose(file);
}

void box(float comprimento, int grelha, const char* ficheiro_output) {
    FILE* file = fopen(ficheiro_output, "w");
    
    if (file == NULL) {
        fprintf(stderr, "Unable to open file: %s\n", ficheiro_output);
        return;
    }

    int pontosPorFace = grelha * grelha * 2 * 3; // Dois triângulos por quadrado, 3 pontos por triângulo
    int totalPontos = pontosPorFace * 6;

    fprintf(file, "%d\n", totalPontos);

    float size = comprimento / (float)grelha;
    float h_size = comprimento / 2.0;
    float px, pz, px2, pz2;

    // Normais

    std::vector<float> nomal_x_n = {-1, 0, 0};
    std::vector<float> nomal_x_p = {1, 0, 0};
    
    std::vector<float> nomal_y_p = {0, 1, 0};
    std::vector<float> nomal_y_n = {0, -1, 0};

    std::vector<float> nomal_z_p = {0, 0, 1};
    std::vector<float> nomal_z_n = {0, 0, -1};

    // Texturas
    // (u1,v2)  (u2,v2)
    // (u1,v1)  (u2,v1)

    float u1, v1, u2, v2;
    
    for(int i = 0; i < grelha; i++) {
        for(int j = 0; j < grelha; j++) {
            px = -h_size + (size * j);
            pz = h_size - (size * i);
            px2 = -h_size + (size * (j + 1));
            pz2 = h_size - (size * (i + 1));

            u1 = (float)j / grelha;
            v1 = (float)i / grelha;
            u2 = (float)(j + 1) / grelha;
            v2 = (float)(i + 1) / grelha;

            // Top face
            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", px, h_size, pz2, nomal_y_p[0], nomal_y_p[1], nomal_y_p[2], u1, v2);
            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", px, h_size, pz, nomal_y_p[0], nomal_y_p[1], nomal_y_p[2], u1, v1);
            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", px2, h_size, pz, nomal_y_p[0], nomal_y_p[1], nomal_y_p[2], u2, v1);

            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", px2, h_size, pz, nomal_y_p[0], nomal_y_p[1], nomal_y_p[2], u2, v1);
            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", px2, h_size, pz2, nomal_y_p[0], nomal_y_p[1], nomal_y_p[2], u2, v2);
            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", px, h_size, pz2, nomal_y_p[0], nomal_y_p[1], nomal_y_p[2], u1, v2);

            // Bottom face
            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", px2, -h_size, pz, nomal_y_n[0], nomal_y_n[1], nomal_y_n[2], u2, v1);
            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", px, -h_size, pz, nomal_y_n[0], nomal_y_n[1], nomal_y_n[2], u1, v1); 
            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", px, -h_size, pz2 , nomal_y_n[0], nomal_y_n[1], nomal_y_n[2], u1, v2);

            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", px, -h_size, pz2, nomal_y_n[0], nomal_y_n[1], nomal_y_n[2], u1, v2);
            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", px2, -h_size, pz2, nomal_y_n[0], nomal_y_n[1], nomal_y_n[2], u2, v2); 
            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", px2, -h_size, pz, nomal_y_n[0], nomal_y_n[1], nomal_y_n[2], u2, v1); 

            // Face Frontal
            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", -h_size, px, pz2, nomal_x_n[0], nomal_x_n[1], nomal_x_n[2], v2, u1); // P1
            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", -h_size, px, pz, nomal_x_n[0], nomal_x_n[1], nomal_x_n[2], v1, u1);  // P2
            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", -h_size, px2, pz, nomal_x_n[0], nomal_x_n[1], nomal_x_n[2], v1, u2); // P3

            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", -h_size, px2, pz, nomal_x_n[0], nomal_x_n[1], nomal_x_n[2], v1, u2); // P1
            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", -h_size, px2, pz2, nomal_x_n[0], nomal_x_n[1], nomal_x_n[2], v2, u2); // P2
            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", -h_size, px, pz2, nomal_x_n[0], nomal_x_n[1], nomal_x_n[2], v2, u1); // P3

            // Face Traseira
            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", h_size, px2, pz, nomal_x_p[0], nomal_x_p[1], nomal_x_p[2], v1, u2); // P1
            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", h_size, px, pz, nomal_x_p[0], nomal_x_p[1], nomal_x_p[2], v1, u1);  // P2
            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", h_size, px, pz2, nomal_x_p[0], nomal_x_p[1], nomal_x_p[2], v2, u1); // P3

            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", h_size, px, pz2, nomal_x_p[0], nomal_x_p[1], nomal_x_p[2], v2, u1); // P1
            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", h_size, px2, pz2, nomal_x_p[0], nomal_x_p[1], nomal_x_p[2], v2, u2); // P2
            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", h_size, px2, pz, nomal_x_p[0], nomal_x_p[1], nomal_x_p[2], v1, u2); // P3

            // Left face
            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", px, pz, -h_size, nomal_z_n[0], nomal_z_n[1], nomal_z_n[2], u1, v1);
            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", px2, pz, -h_size, nomal_z_n[0], nomal_z_n[1], nomal_z_n[2], u2, v1);
            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", px2, pz2, -h_size, nomal_z_n[0], nomal_z_n[1], nomal_z_n[2], u2, v2);

            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", px2, pz2, -h_size, nomal_z_n[0], nomal_z_n[1], nomal_z_n[2], u2, v2);
            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", px, pz2, -h_size, nomal_z_n[0], nomal_z_n[1], nomal_z_n[2], u1, v2);
            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", px, pz, -h_size, nomal_z_n[0], nomal_z_n[1], nomal_z_n[2], u1, v1);

            // Right face
            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", px2, pz, h_size, nomal_z_p[0] , nomal_z_p[1], nomal_z_p[2], u2, v1);
            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", px, pz, h_size, nomal_z_p[0] , nomal_z_p[1], nomal_z_p[2], u1, v1);
            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", px, pz2, h_size, nomal_z_p[0] , nomal_z_p[1], nomal_z_p[2], u1, v2);

            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", px, pz2, h_size, nomal_z_p[0] , nomal_z_p[1], nomal_z_p[2], u1, v2);
            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", px2, pz2, h_size, nomal_z_p[0] , nomal_z_p[1], nomal_z_p[2], u2, v2);
            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", px2, pz, h_size, nomal_z_p[0] , nomal_z_p[1], nomal_z_p[2], u2, v1);
        }
    }

    fclose(file);
}

void cone(float radius, float height, int slices, int stacks, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Unable to open file: %s\n", filename);
        return;
    }

    float alphaStep = 2 * M_PI / slices;
    float heightStep = height / stacks;
    float radiusStep = radius / stacks;

    int num_points = 0;
    char* buffer = NULL;
    size_t buffer_size = 0;
    FILE* temp = open_memstream(&buffer, &buffer_size);

    if (temp == NULL) {
        fprintf(stderr, "Failed to create memory stream\n");
        fclose(file);
        return;
    }

    // Generate the base of the cone
    for (int i = 0; i < slices; ++i) {
        float alpha = i * alphaStep;
        float nextAlpha = (i + 1) * alphaStep;

        fprintf(temp, "0.0 0.0 0.0 ; 0.0 -1.0 0.0 ; 0.5 0.5\n");
        fprintf(temp, "%f 0.0 %f ; 0.0 -1.0 0.0 ; %f %f\n",
                radius * cos(nextAlpha), radius * sin(nextAlpha),
                cos(nextAlpha) * 0.5 + 0.5, sin(nextAlpha) * 0.5 + 0.5);
        fprintf(temp, "%f 0.0 %f ; 0.0 -1.0 0.0 ; %f %f\n",
                radius * cos(alpha), radius * sin(alpha),
                cos(alpha) * 0.5 + 0.5, sin(alpha) * 0.5 + 0.5);

        num_points += 3; // each fprintf adds one triangle
    }

    // Generate the lateral surface of the cone
    for (int i = 0; i < slices; ++i) {
        float alpha = i * alphaStep;
        float nextAlpha = (i + 1) * alphaStep;

        for (int j = 0; j < stacks; ++j) {
            float lowerRadius = radius - j * radiusStep;
            float upperRadius = radius - (j + 1) * radiusStep;
            float lowerHeight = j * heightStep;
            float upperHeight = (j + 1) * heightStep;

            fprintf(temp, "%f %f %f ; %f %f %f ; %f %f\n",
                    lowerRadius * cos(alpha), lowerHeight, lowerRadius * sin(alpha),
                    cos(alpha), height / sqrt(height * height + radius * radius), sin(alpha),
                    (float) i / slices, (float) j / stacks);
            fprintf(temp, "%f %f %f ; %f %f %f ; %f %f\n",
                    upperRadius * cos(alpha), upperHeight, upperRadius * sin(alpha),
                    cos(alpha), height / sqrt(height * height + radius * radius), sin(alpha),
                    (float) i / slices, (float) (j + 1) / stacks);
            fprintf(temp, "%f %f %f ; %f %f %f ; %f %f\n",
                    lowerRadius * cos(nextAlpha), lowerHeight, lowerRadius * sin(nextAlpha),
                    cos(nextAlpha), height / sqrt(height * height + radius * radius), sin(nextAlpha),
                    (float) (i + 1) / slices, (float) j / stacks);

            fprintf(temp, "%f %f %f ; %f %f %f ; %f %f\n",
                    lowerRadius * cos(nextAlpha), lowerHeight, lowerRadius * sin(nextAlpha),
                    cos(nextAlpha), height / sqrt(height * height + radius * radius), sin(nextAlpha),
                    (float) (i + 1) / slices, (float) j / stacks);
            fprintf(temp, "%f %f %f ; %f %f %f ; %f %f\n",
                    upperRadius * cos(alpha), upperHeight, upperRadius * sin(alpha),
                    cos(alpha), height / sqrt(height * height + radius * radius), sin(alpha),
                    (float) i / slices, (float) (j + 1) / stacks);
            fprintf(temp, "%f %f %f ; %f %f %f ; %f %f\n",
                    upperRadius * cos(nextAlpha), upperHeight, upperRadius * sin(nextAlpha),
                    cos(nextAlpha), height / sqrt(height * height + radius * radius), sin(nextAlpha),
                    (float) (i + 1) / slices, (float) (j + 1) / stacks);

            num_points += 6; // each fprintf adds one triangle
        }
    }

    fclose(temp);

    fprintf(file, "%d\n", num_points);
    fwrite(buffer, sizeof(char), buffer_size, file);

    free(buffer);
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

    float u, v, u2, v2;

    // normal (0,1,0)
    float normal_x = 0.0f, normal_y = 1.0f, normal_z = 0.0f;

    int totalPontos = (divisoes) * (divisoes) * 6;
    fprintf(file, "%d\n", totalPontos);
    
    for(int i = 0; i < divisoes; i++) {
        for(int j = 0; j < divisoes; j++) {
            px = -h_size + (size * j);
            pz = h_size - (size * i);
            px2 = -h_size + (size * (j + 1));
            pz2 = h_size - (size * (i + 1));

            u = (float)j / divisoes;
            v = (float)i / divisoes;
            u2 = (float)(j + 1) / divisoes;
            v2 = (float)(i + 1) / divisoes;
                                                                                                                   
            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", px, 0.0, pz2, normal_x, normal_y, normal_z, u, v2);
            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", px, 0.0, pz, normal_x, normal_y, normal_z, u, v);
            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", px2, 0.0, pz, normal_x, normal_y, normal_z, u2, v);

            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", px2, 0.0, pz, normal_x, normal_y, normal_z, u2, v);
            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", px2, 0.0, pz2, normal_x, normal_y, normal_z, u2, v2);
            fprintf(file, "%f %f %f ; %f %f %f ; %f %f\n", px, 0.0, pz2, normal_x, normal_y, normal_z, u, v2);

        }
    }

    fclose(file);
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

void calculatePoints(float u, float v, std::vector<std::vector<float>> patch, float* res, float* normal) {
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

    float DER_U[4] = {3.0f*u*u,2.0f*u,1.0f,0.0f}, DER_V[4] = {3.0f*v*v,2.0f*v,1.0f,0.0f};
    float DuM[4]; multiplyMatrices(1,4,DER_U,4,4,M,DuM); // DuM: 1x4, U' x M
	float MDv[4]; multiplyMatrices(4,4,M,4,1,DER_V,MDv); // MDv: 4x1, M x V' 

    float Du[3]; 
	float Dv[3];

    for(int i = 0; i < 3; i++){
			float DuMP[4]; // DuMP: 1x4
			multiplyMatrices(1,4,DuM,4,4,P[i],DuMP); // U' x M x P
			multiplyMatrices(1,4,DuMP,4,1,MV,Du+i); // U' x M x P x M x V
			float UMP[4]; // UMP: 1x4 
			multiplyMatrices(1,4,UM,4,4,P[i],UMP); // U x M x P
			multiplyMatrices(1,4,UMP,4,1,MDv,Dv+i); // U x M x P x M x V'
		}

		// N = u x v
        normal[0] = Dv[1]*Du[2] - Dv[2]*Du[1];
	    normal[1] = Dv[2]*Du[0] - Dv[0]*Du[2];
    	normal[2] = Dv[0]*Du[1] - Dv[1]*Du[0];
    

}


void buildPatches(const char* ficheiroPath,int tessellation,const char* ficheiro_output){
    FILE* file = fopen(ficheiro_output, "w");

    float u = 0.0f, v = 0.0f, delta = 1.0f/tessellation;
    float P1[3], P2[3], P3[3], P4[3], normal1[3], normal2[3], normal3[3], normal4[3];

    int num_points = 0;
    char* buffer = NULL;
    size_t buffer_size = 0;
    FILE* temp = open_memstream(&buffer, &buffer_size);

    if(temp == NULL) {
        fprintf(stderr, "Failed to create memory stream\n");
        fclose(file);
        return;
    }
    std:: vector< std:: vector< std:: vector<float>>> patches = readPatchesFile(ficheiroPath);

    for(std::vector<std::vector<float>> patch : patches){ 
        for(int i = 0; i < tessellation; i++, u += delta){
            for(int j = 0; j < tessellation; j++, v += delta){

                // Cálculo dos pontos
                calculatePoints(u, v, patch, P1, normal1);
                calculatePoints(u, v + delta, patch, P2, normal2);
                calculatePoints(u + delta, v, patch, P3, normal3);
                calculatePoints(u + delta, v + delta, patch, P4, normal4);
         
                // Coordenadas de textura
                float texCoord1[2] = {u, v};
                float texCoord2[2] = {u, v + delta};
                float texCoord3[2] = {u + delta, v};
                float texCoord4[2] = {u + delta, v + delta};

                // Triangulação com normais e texturas
                fprintf(temp, "%f %f %f ; %f %f %f ; %f %f\n", P3[0], P3[1], P3[2], normal3[0], normal3[1], normal3[2], texCoord3[0], texCoord3[1]);
                fprintf(temp, "%f %f %f ; %f %f %f ; %f %f\n", P1[0], P1[1], P1[2], normal1[0], normal2[1], normal2[2], texCoord1[0], texCoord1[1]);
                fprintf(temp, "%f %f %f ; %f %f %f ; %f %f\n", P2[0], P2[1], P2[2], normal2[0], normal1[1], normal1[2], texCoord2[0], texCoord2[1]);

                fprintf(temp, "%f %f %f ; %f %f %f ; %f %f\n", P2[0], P2[1], P2[2], normal2[0], normal2[1], normal2[2], texCoord2[0], texCoord2[1]);
                fprintf(temp, "%f %f %f ; %f %f %f ; %f %f\n", P4[0], P4[1], P4[2], normal4[0], normal4[1], normal4[2], texCoord4[0], texCoord4[1]);
                fprintf(temp, "%f %f %f ; %f %f %f ; %f %f\n", P3[0], P3[1], P3[2], normal3[0], normal3[1], normal3[2], texCoord3[0], texCoord3[1]);
                num_points += 6;
            }
            v = 0.0f;
        }
        u = v = 0.0f;
    }
    fclose(temp);
    fprintf(file, "%d\n", num_points);
    fwrite(buffer, sizeof(char), buffer_size, file);
    free(buffer);
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