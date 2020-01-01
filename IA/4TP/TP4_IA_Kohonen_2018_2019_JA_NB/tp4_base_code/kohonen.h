#ifndef KOHONEN_H
#define KOHONEN_H

typedef struct kohonen {
   float **weight;
   float *input;
   int sizeX, sizeY;
   int sizeInput;
   int winner;
   float (*phi)(float);
   float (*topDist)(int, int, int, int);
} KOHONEN;

typedef struct training_data {
   float** input;
   int sizeInput;
   int numInput;
} TRAINING_DATA;

KOHONEN *initKohonen(int, int, int, float (*func)(float), float (*topDist)(int, int, int, int));
void freeKohonen(KOHONEN**);
void updateKohonen(KOHONEN* map, float* input, float EPSILON);
void trainKohonen(KOHONEN* map, TRAINING_DATA* data, int num_iter, float EPSILON);

float potential(float *v1, float *v2, int size);

/* For topological distance */
float loopTopologicalDistance(int row_size, int column_size, int a, int b);
float topologicalDistance(int row_size, int column_size, int a, int b);

TRAINING_DATA* initTrainingData(int sizeInput, int numInput);
void freeTrainingData(TRAINING_DATA** data);

float manhattanDistance(float *v1, float *v2, int size);
float euclidianDistance(float *v1, float *v2, int size);
#endif
