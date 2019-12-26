#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "kohonen.h"

KOHONEN *initKohonen(int sizeX, int sizeY, int sizeInput, float (*distfunc)(float))
{
   KOHONEN *map = malloc(sizeof(KOHONEN));

   map->sizeX = sizeX;
   map->sizeY = sizeY;
   map->sizeInput = sizeInput;
   map->phi = distfunc;

   map->weight = malloc(sizeof(float*) * sizeX * sizeY);

   for (int i = 0; i < sizeX * sizeY; i++) {
//      map->weight[i] = malloc(sizeof(float) * sizeInput);
      map->weight[i] = calloc(sizeInput, sizeof(float));

/*      for (int k = 0; k < sizeInput; k++) {
         map->weight[i][k] = rand() / RAND_MAX;
      }
*/
      }

   map->input = calloc(sizeof(float), sizeInput);

   return map;
}

void freeKohonen(KOHONEN **map)
{
   int total_size = ((*map)->sizeX) * ((*map)->sizeY);

   for (int i = 0; i < total_size; i++) {
      free((*map)->weight[i]);
      (*map)->weight[i] = NULL;
   }

   free((*map)->weight);
   (*map)->weight = NULL;

   free((*map)->input);
   (*map)->input = NULL;

   free(*map);
   *map = NULL;
}

float topologicalDistance(int row_size, int a, int b)
{
   int aX =  a % row_size;
   int aY = ((int) a / row_size);

   int bX = b % row_size;
   int bY = ((int) b / row_size);

   return fabs(aX - bX) + fabs(aY - bY);
}

float activation(float x)
{
   return 1 / (1 + x);
}

float manhattanDistance(float *v1, float *v2, int size)
{
   float total_dist = 0.0;
   for (int i = 0; i < size; i++) {
      total_dist += fabs(v1[i] - v2[i]);
   }
   return total_dist;
}

float euclidianDistance(float *v1, float *v2, int size)
{
   float total_dist = 0.0;
   for (int i = 0; i < size; i++) {
      total_dist += (v1[i] - v2[i]) * (v1[i] - v2[i]);
   }
   return sqrt(total_dist);
}

float potential(float *v1, float *v2, int size)
{
   return activation(euclidianDistance(v1, v2, size));
}

void findWinner(KOHONEN* map, float* input)
{
   int total_size = map->sizeX * map->sizeY;

   memcpy(map->input, input, sizeof(float) * map->sizeInput);

   float cur_max = -1;
   for (int i = 0; i < total_size; i++) {
      float pot = potential(map->weight[i], input, map->sizeInput);
      if(pot > cur_max) {
         cur_max = pot;
         map->winner = i;
      }
   }
}

void updateKohonen(KOHONEN* map, float* input)
{
   findWinner(map, input);

   int total_size = map->sizeX * map->sizeY;

   for (int i = 0; i < total_size; i++) {
      float dist = topologicalDistance(map->sizeX, i, map->winner);

      for (int j = 0; j < map->sizeInput; j++) {
         map->weight[i][j] += EPSILON * (map->input[j] - map->weight[i][j]) * map->phi(dist);
      }
   }
}

void trainKohonen(KOHONEN* map, TRAINING_DATA* data)
{
   int num_iter = 1000;
   for (int i = 0; i < num_iter; i++) {
      // choose an input randomly
      int choice = rand() % data->numInput;

      // find the winner then update the neurones
      updateKohonen(map, data->input[choice]);
   }   
}


TRAINING_DATA* initTrainingData(int numInput, int sizeInput)
{
   TRAINING_DATA* data = malloc(sizeof(TRAINING_DATA));

   data->sizeInput = sizeInput;
   data->numInput = numInput;
   
   data->input = malloc(sizeof(float*) * numInput);
   for (int i = 0; i < numInput; i++) {
      data->input[i] = calloc(sizeof(float), sizeInput);
   }

   return data;
}

void freeTrainingData(TRAINING_DATA** data)
{
   for (int i = 0; i < (*data)->numInput; i++) {
      free((*data)->input[i]);
      (*data)->input[i] = NULL;
   }

   free((*data)->input);
   (*data)->input = NULL;

   free(*data);
   *data = NULL;
}
