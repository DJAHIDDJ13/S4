#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "kohonen.h"

/**
 * Initialisation de réseau
 */
KOHONEN *initKohonen(int sizeX, int sizeY, int sizeInput, float (*distfunc)(float), float (*topDist)(int, int, int, int))
{
   KOHONEN *map = malloc(sizeof(KOHONEN));

   map->sizeX = sizeX;
   map->sizeY = sizeY;
   map->sizeInput = sizeInput;
   map->phi = distfunc;
   map->topDist = topDist;


   map->weight = malloc(sizeof(float*) * sizeX * sizeY);

   for (int i = 0; i < sizeX * sizeY; i++) {
      map->weight[i] = calloc(sizeInput, sizeof(float));
   }

   map->input = calloc(sizeof(float), sizeInput);

   return map;
}

/**
 * Libération de mémoire de réseau
 */
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

/** Pour un carte torique
 * A ring topology for the neurones ie the 0th and sizeX-1th neurones are neighbors
 * to find a loop (end of the path is at the start) for the traveling salesman problem
 */
float loopTopologicalDistance(int row_size, int column_size, int a, int b)
{
   int aX =  a % row_size;
   int aY = ((int) a / row_size);

   int bX = b % row_size;
   int bY = ((int) b / row_size);

   // --------o--------------o----------- // the two o's are the indices of the two we want to compare
   // ________o--------------o___________ // the reverse distance
   // --------o______________o----------- // the normal distance
   // we take the minimum between these two distances to get the 
   return fmin(
            fabs(aX - bX),                        // normal distance between the two  
            row_size - fmax(aX, bX) + fmin(aX,bX) // distance if we go the other direction ie through the 0
          ) +

          fmin(
            fabs(aY - bY), 
            column_size - fmax(aY, bY) + fmin(aY,bY)
         );
}

/**
 * Distance topologique entre deux neurones
 */
float topologicalDistance(int row_size, int column_size, int a, int b)
{
   int aX =  a % row_size;
   int aY = ((int) a / row_size);

   int bX = b % row_size;
   int bY = ((int) b / row_size);
   (void) column_size;

   return fabs(aX - bX) + fabs(aY - bY);
}

/**
 * Fonction d'activation de potentielle
 */
float activation(float x)
{
   return 1 / (1 + x);
}

/**
 * Distance manhattan (norme ordre 0)
 */
float manhattanDistance(float *v1, float *v2, int size)
{
   float total_dist = 0.0;
   for (int i = 0; i < size; i++) {
      total_dist += fabs(v1[i] - v2[i]);
   }
   return total_dist;
}

/**
 * Distance euclidienne (norme ordre 1)
 */
float euclidianDistance(float *v1, float *v2, int size)
{
   float total_dist = 0.0;
   for (int i = 0; i < size; i++) {
      total_dist += (v1[i] - v2[i]) * (v1[i] - v2[i]);
   }
   return sqrt(total_dist);
}

/**
 * Fonction de potentielle
 */
float potential(float *v1, float *v2, int size)
{
   return activation(euclidianDistance(v1, v2, size)); // You can change the type of distance here
   // either euclidianDistance or manhattanDistance
}

/**
 * Chercher le neurones gagnant
 */
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

/**
 * Mise a jour de carte (le gagnant est stocké dans le structure)
 */
void updateKohonen(KOHONEN* map, float* input, float EPSILON)
{
   findWinner(map, input);

   int total_size = map->sizeX * map->sizeY;

   for (int i = 0; i < total_size; i++) {
      //float dist = loopTopologicalDistance(map->sizeX, map->sizeY, i, map->winner); 
      // uncomment this for a loop neurone topology (view the loopTopologicalDistance function for more) <<< 
      float dist = map->topDist(map->sizeX, map->sizeY, i, map->winner);

      for (int j = 0; j < map->sizeInput; j++) {
         map->weight[i][j] += EPSILON * (map->input[j] - map->weight[i][j]) * map->phi(dist);
      }
   }
}

/**
 * Apprentissage de réseau
 */
void trainKohonen(KOHONEN* map, TRAINING_DATA* data, int num_iter, float EPSILON)
{
   for (int i = 0; i < num_iter; i++) {
      // choose an input randomly
      int choice = rand() % data->numInput;

      // find the winner then update the neurones
      updateKohonen(map, data->input[choice], EPSILON);
   }   
}

/**
 * Initialisation de données d'entrainage
 */
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

/**
 * Libération mémoire de données d'entrainage
 */
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
