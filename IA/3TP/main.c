#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define EPSILON 0.01
#define ARRAY_LEN(a) (sizeof((a))/sizeof((a[0])))

typedef struct neurone {
   float *weight;
   float bias;
   float (*activation)(float);
   float out;
} NEURONE;

typedef struct network {
   int num_layers;
   NEURONE **layers;
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
         func = heaviside;
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
         free(network->layers[layer][i].weight);
         network->layers[layer][i].weight = NULL;
      }

      free(network->layers[layer]);
      network->layers[layer] = NULL;
   }

   free(network->layers);
   network->layers = NULL;
   free(network->sizes);
   network->sizes = NULL;

   *network_ptr = NULL;
}

void evaluate(NETWORK* network, float* data)
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

         cur_neurone->out = cur_neurone->activation(cur_neurone->out - cur_neurone->bias);
      }
   }
}

void train(NETWORK *network, TRAINING_DATA *data)
{
   int num_iter = 10000;

   for(int i = 0; i < num_iter; i++) {
      for(int k = 0; k < network->sizes[1]; k++) {
         int choice = rand() % data->size;

         float sol = data->entries[choice].output[k];
         evaluate(network, data->entries[choice].input);

         for(int j = 0; j < network->sizes[0]; j++) {
            network->layers[1][k].weight[j] += EPSILON * (sol - network->layers[1][k].out) * data->entries[choice].input[j];
         }
      }
   }
}

TRAINING_DATA *read_training_data(const char *filename)
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
   printf("%d\n", data->size);
   data->entries = malloc(sizeof(DATA_ENTRY) * data->size);

   if(data->entries == NULL) {
      fprintf(stderr, "Cannot allocate memory\n");
      exit(1);
   }

   if(fscanf(f, "%d %d\n", &data->num_in, &data->num_out) < 2) {
      fprintf(stderr, "Cannot read input or output sizes\n");
      exit(1);
   }
   printf("%d %d\n", data->num_in, data->num_out);

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
         printf("%g ", data->entries[i].input[j]);
      }
      printf("\n");

      for(int j = 0; j < data->num_out; j++) {
         if(fscanf(f, "%f", &data->entries[i].output[j]) < 1) {
            fprintf(stderr, "Cannot read input value entry=%d, value=%d\n", i, j);
            exit(1);
         }
         printf("%g ", data->entries[i].output[j]);
      }
      printf("\n");

   }

   fclose(f);

   return data;
}

void free_training_data(TRAINING_DATA **data)
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
   TRAINING_DATA *data = read_training_data("training_data.txt");

   const int layer_sizes[] = {data->num_in, data->num_out};
   NETWORK *network = initNetwork(2, layer_sizes);
   train(network, data);

   float At_im[] = {1, 1, 1, 1,
                    1, 0, 0, 1,
                    1, 1, 1, 1,
                    1, 0, 0, 1,
                    1, 0, 0, 1};

   evaluate(network, At_im);
   dumpNetwork(network);

   destroyNetwork(&network);
   free_training_data(&data);
   return 0;
}
