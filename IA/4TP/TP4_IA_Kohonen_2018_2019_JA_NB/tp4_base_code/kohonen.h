#ifndef KOHONEN_H
#define KOHONEN_H

#define EPSILON .5

typedef struct kohonen {
   float **weight;
   float *input;
   int sizeX, sizeY;
   int sizeInput;
   int winner;
   float (*phi)(float);
} KOHONEN;

typedef struct training_data {
   float** input;
   int sizeInput;
   int numInput;
} TRAINING_DATA;

KOHONEN *initKohonen(int, int, int, float (*func)(float));
void freeKohonen(KOHONEN**);
void updateKohonen(KOHONEN* map, float* input);
void trainKohonen(KOHONEN* map, TRAINING_DATA* data);
float potential(float *v1, float *v2, int size);
TRAINING_DATA* initTrainingData(int sizeInput, int numInput);
void freeTrainingData(TRAINING_DATA** data);

float manhattanDistance(float *v1, float *v2, int size);
float euclidianDistance(float *v1, float *v2, int size);
#endif
