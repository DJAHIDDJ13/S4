#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define EPSILON 0.001
#define BIAS_EPSILON 0.1 

typedef struct neurone {
   float *weight;
   float (*activation)(float);
   float out;
} NEURONE;

typedef struct network {
   int num_layers;
   NEURONE **layers;
   float *biases;
   int *sizes;
} NETWORK;

typedef struct training_data_entry {
   float *input;
   float *output;
} DATA_ENTRY;

typedef struct training_data {
   int size;
   int num_in, num_out;
   DATA_ENTRY *entries;
} TRAINING_DATA;

float heaviside(float x)
{
   return (x > 0) ? 1 : 0;
}

float identity(float x)
{
   return x;
}

float sigmoid(float x)
{
   return 1 / (1 + exp(-x));
}

void dumpNetwork(NETWORK *network)
{
   int num_layers = network->num_layers;

   int max_size = 0;

   for(int i = 0; i < network->num_layers; i++) {
      int new_size = network->sizes[i];
      max_size = (max_size < new_size) ? new_size : max_size;
   }

   for(int i = 0; i < max_size; i++) {
      for(int layer = 0; layer < num_layers; layer++) {
         if(i < network->sizes[layer]) {
            printf("%g", network->layers[layer][i].out);
         }

         printf("\t");
      }

      printf("\n");
   }
}

NETWORK* initNetwork(int num_layers, const int layer_sizes[])
{
   NETWORK *network = malloc(sizeof(NETWORK));

   network->num_layers = num_layers;

   network->layers = malloc(sizeof(NEURONE*) * num_layers);
   network->biases = calloc(sizeof(float), num_layers); 

   network->sizes = malloc(sizeof(int) * num_layers);

   for(int layer = 0; layer < num_layers; layer++) {
      network->sizes[layer] = layer_sizes[layer];

      network->layers[layer] = calloc(sizeof(NEURONE), layer_sizes[layer]);
      float (*func)(float);
      int size;

      if(layer == 0) {
         func = identity;
         size = 1;
      } else {
         func = sigmoid;
         size = layer_sizes[layer - 1];
      }

      for(int i = 0; i < layer_sizes[layer]; i++) {
         network->layers[layer][i].weight = calloc(sizeof(float), size);
         network->layers[layer][i].activation = func;
      }
   }

   return network;
}

void destroyNetwork(NETWORK** network_ptr)
{
   NETWORK* network = *network_ptr;

   for(int layer = 0; layer < network->num_layers; layer++) {
      int cur_size = network->sizes[layer];

      for(int i = 0; i < cur_size; i++) {
         if(network->layers[layer][i].weight)
            free(network->layers[layer][i].weight);
         network->layers[layer][i].weight = NULL;
      }

      if(network->layers[layer])
         free(network->layers[layer]);
      network->layers[layer] = NULL;
   }
   
   if(network->layers)
      free(network->layers);
   network->layers = NULL;

   if(network->sizes)
      free(network->sizes);
   network->sizes = NULL;

   if(*network_ptr)
      free(*network_ptr);
   *network_ptr = NULL;
}

void evaluateNetwork(NETWORK* network, float* data)
{
   // input layer
   for(int j = 0; j < network->sizes[0]; j++) {
      network->layers[0][j].out = data[j];
   }

   int num_layers = network->num_layers;

   for(int layer = 1; layer < num_layers; layer++) {

      int cur_layer_size = network->sizes[layer];
      int prev_layer_size = network->sizes[layer - 1];

      for(int i = 0; i < cur_layer_size; i++) {
         NEURONE *cur_neurone = &network->layers[layer][i];
         cur_neurone->out = 0.0;

         for(int j = 0; j < prev_layer_size; j++) {
            NEURONE *prev_neurone = &network->layers[layer - 1][j];
            cur_neurone->out += prev_neurone->out * cur_neurone->weight[j];
         }

         cur_neurone->out = cur_neurone->activation(cur_neurone->out - network->biases[layer]);
      }
   }
}

/** Only works for two layers
 * */
void trainNetwork(NETWORK *network, TRAINING_DATA *data)
{
   int num_iter = 10000;

   for(int i = 0; i < num_iter; i++) {
      int choice = rand() % data->size;
      evaluateNetwork(network, data->entries[choice].input);

      for(int layer = network->num_layers-1; layer >= 1; layer--) {

         for(int k = 0; k < network->sizes[layer]; k++) {
            float sol = data->entries[choice].output[k];
            network->biases[layer] -= BIAS_EPSILON * (sol - network->layers[layer][k].out); 
            
            // for each weight of the k th neurone
            for(int j = 0; j < network->sizes[layer-1]; j++) {
               network->layers[layer][k].weight[j] += EPSILON *
                                                      (sol - network->layers[layer][k].out) * 
                                                      network->layers[layer-1][j].out; 
            }
         }
      }

   }

}

TRAINING_DATA *readTrainingData(const char *filename)
{
   FILE* f = fopen(filename, "r");

   if(!f) {
      fprintf(stderr, "Cannot open %s\n", filename);
      exit(1);
   }

   TRAINING_DATA *data = malloc(sizeof(TRAINING_DATA));

   if(fscanf(f, "%d\n", &data->size) < 1) {
      fprintf(stderr, "Cannot read number of data entries\n");
      exit(1);
   }
   data->entries = malloc(sizeof(DATA_ENTRY) * data->size);

   if(data->entries == NULL) {
      fprintf(stderr, "Cannot allocate memory\n");
      exit(1);
   }

   if(fscanf(f, "%d %d\n", &data->num_in, &data->num_out) < 2) {
      fprintf(stderr, "Cannot read input or output sizes\n");
      exit(1);
   }

   for(int i = 0; i < data->size; i++) {
      data->entries[i].input  = malloc(sizeof(float) * data->num_in);
      data->entries[i].output = malloc(sizeof(float) * data->num_out);

      if(data->entries[i].input == NULL || data->entries[i].output == NULL) {
         fprintf(stderr, "Cannot allocate memory for input or output entry=%d\n", i);
         exit(1);
      }

      for(int j = 0; j < data->num_in; j++) {
         if(fscanf(f, "%f", &data->entries[i].input[j]) < 1) {
            fprintf(stderr, "Cannot read input value entry=%d, value=%d\n", i, j);
            exit(1);
         }
      }

      for(int j = 0; j < data->num_out; j++) {
         if(fscanf(f, "%f", &data->entries[i].output[j]) < 1) {
            fprintf(stderr, "Cannot read input value entry=%d, value=%d\n", i, j);
            exit(1);
         }
      }

   }

   fclose(f);

   return data;
}

void destoryTrainingData(TRAINING_DATA **data)
{
   for(int i = 0; i < (*data)->size; i++) {
      if((*data)->entries[i].input)
         free((*data)->entries[i].input);

      (*data)->entries[i].input = NULL;

      if((*data)->entries[i].output)
         free((*data)->entries[i].output);

      (*data)->entries[i].output = NULL;
   }
   
   if((*data)->entries)
      free((*data)->entries);
   (*data)->entries = NULL;

   if(*data)
      free(*data);
   *data = NULL;
}

int main(int argc, char *argv[])
{
   TRAINING_DATA *data = readTrainingData("training_data.txt");

   const int layer_sizes[] = {data->num_in, data->num_out};
   NETWORK *network = initNetwork(2, layer_sizes);
   trainNetwork(network, data);

   float test_im[] = {0, 0, 0, 0,
                      0, 0, 1, 0,
                      0, 1, 0, 1,
                      0, 1, 1, 1,
                      0, 1, 0, 1};

   evaluateNetwork(network, test_im);
   dumpNetwork(network);

   destroyNetwork(&network);
   destoryTrainingData(&data);
   return 0;
}

