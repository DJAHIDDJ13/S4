#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define EPSILON 0.01
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


/**
 * To display the 'actual' i'th neurone when showing the neural network
 */
void show_letter(int i) {
   printf("%c", (char) i + 'A');
}

void AC_letter(int i) {
   printf("%c", (i == 1)?'A':'C');
}

/*
 * repr is a function that displays the value the neurone represents
 * ex: for recognizing the alphabet it should print the letter from its index
 */
void dumpNetwork(NETWORK *network, void (*repr)(int))
{
   printf("Network dump\n");
   int num_layers = network->num_layers;

   int max_size = 0;

   for(int i = 0; i < network->num_layers; i++) {
      int new_size = network->sizes[i];
      max_size = (max_size < new_size) ? new_size : max_size;
   }

   // finding the top 3 winners ie the 3 output neurones with the biggest .out value
   int win = -1;
   int win2 = -1;
   int win3 = -1;

   int out_layer_size = network->sizes[network->num_layers-1];
   NEURONE* out_layer = network->layers[network->num_layers-1];
   for (int i = 0; i < out_layer_size; i++) {
      if(win == -1 || out_layer[i].out >= out_layer[win].out) {
         win3 = win2;
         win2 = win;
         win = i;
      } else if(win2 == -1 || out_layer[i].out >= out_layer[win2].out) {
         win3 = win2;
         win2 = i;
      } else if(win3 == -1 || out_layer[i].out >= out_layer[win3].out) {
         win3 = i;
      }

   }

   // print the output of all the neurones of the network
   for(int i = 0; i < max_size; i++) {
      for(int layer = 0; layer < num_layers; layer++) {
         int translate =  (max_size - network->sizes[layer]) / 2; // translate the display to the center
         
         if(i >= translate && i < max_size-translate) {
            printf("%.3f ", network->layers[layer][i - translate].out);
            if(layer == num_layers-1) {
               repr(i - translate);
               
               // show the top 3
               if(i - translate == win) 
                  printf(" <-- first"); 
               else if(i - translate == win2)
                  printf(" <-- second");
               else if(i - translate == win3)
                  printf(" <-- third");
            }
         }
         printf("\t");
      }

      printf("\n");
   }
   printf("\n");
}


/**
 * Initialize the network with random values [0, 1] for the weights and 0 for
 * the biases
 */
NETWORK* initNetwork(int num_layers, const int layer_sizes[])
{
   NETWORK *network = malloc(sizeof(NETWORK));

   network->num_layers = num_layers;

   network->layers = malloc(sizeof(NEURONE*) * num_layers);
   network->biases = calloc(sizeof(float), num_layers);

   network->sizes = malloc(sizeof(int) * num_layers);

   for(int layer = 0; layer < num_layers; layer++) {
      network->sizes[layer] = layer_sizes[layer];

      network->layers[layer] = malloc(sizeof(NEURONE) * layer_sizes[layer]);

      float (*func)(float);
      int size;

      if(layer == 0) {
         func = identity;
         size = 1;
      } else {
         func = sigmoid; // CHANGE THE ACTIVATION FUNCTION HERE sigmoid or heaviside
         size = layer_sizes[layer - 1];
      }

      for(int i = 0; i < layer_sizes[layer]; i++) {
         network->layers[layer][i].weight = malloc(sizeof(float) * size);
         for (int j = 0; j < size; j++) {
            network->layers[layer][i].weight[j] = rand() / RAND_MAX;  
         }

         network->layers[layer][i].activation = func;
      }
   }

   return network;
}

/**
 * Liberate the memory fo the network
 */
void destroyNetwork(NETWORK** network_ptr)
{
   NETWORK* network = *network_ptr;

   for(int layer = 0; layer < network->num_layers; layer++) {
      int cur_size = network->sizes[layer];

      for(int i = 0; i < cur_size; i++) {
         if(network->layers[layer][i].weight) {
            free(network->layers[layer][i].weight);
         }

         network->layers[layer][i].weight = NULL;
      }

      if(network->layers[layer]) {
         free(network->layers[layer]);
      }

      network->layers[layer] = NULL;
   }

   if(network->layers) {
      free(network->layers);
   }

   network->layers = NULL;

   if(network->sizes) {
      free(network->sizes);
   }

   network->sizes = NULL;

   if(*network_ptr) {
      free(*network_ptr);
   }

   *network_ptr = NULL;
}

/**
 * Forward propagation of the network of all the neurones
 */
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

/**
 * Train the network using the data
 * Only works for two layers
 * */
void trainNetwork(NETWORK *network, TRAINING_DATA *data)
{
   int num_iter_min = 1000; // Minimum number of training steps
   int num_iter_max = 100000; // Maximum number of training steps
   float target_err = 0.01; // target error 
   float err = 1.0; // error init value
   
   for(int i = 0; (i < num_iter_min || err > target_err) && i < num_iter_max; i++) {
      int choice = i % data->size; // select training examples consecutively
      // int choice = rand() % data->size; // select a random training example
      evaluateNetwork(network, data->entries[choice].input);

      err = 0.0;
      for(int layer = network->num_layers - 1; layer >= 1; layer--) {

         // for each neurone in the layer
         for(int k = 0; k < network->sizes[layer]; k++) {
            float sol = data->entries[choice].output[k];
            err += fabs(sol - network->layers[layer][k].out);
            network->biases[layer] -= BIAS_EPSILON * (sol - network->layers[layer][k].out);

            // for each weight of the k th neurone
            for(int j = 0; j < network->sizes[layer - 1]; j++) {
               network->layers[layer][k].weight[j] += EPSILON *
                                                      (sol - network->layers[layer][k].out) *
                                                      network->layers[layer - 1][j].out;
            }
         }
      }

   }

}

/**
 * Read the training data from a file
 */
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

/**
 * Frees up the training data
 */
void destoryTrainingData(TRAINING_DATA **data)
{
   for(int i = 0; i < (*data)->size; i++) {
      if((*data)->entries[i].input) {
         free((*data)->entries[i].input);
      }

      (*data)->entries[i].input = NULL;

      if((*data)->entries[i].output) {
         free((*data)->entries[i].output);
      }

      (*data)->entries[i].output = NULL;
   }

   if((*data)->entries) {
      free((*data)->entries);
   }

   (*data)->entries = NULL;

   if(*data) {
      free(*data);
   }

   *data = NULL;
}

/**
 * Test and display network results on data
 */
void testNetwork(NETWORK* network, TRAINING_DATA* data)
{
   float err = 0.0;
   int num_correct = 0;

   for (int i = 0; i < data->size; i++) {
      evaluateNetwork(network, data->entries[i].input);
      dumpNetwork(network, show_letter);

      float loc_err = 0.0;    
      int arg_max = -1;
      int base = -1;

      for (int j = 0; j < network->sizes[network->num_layers-1]; j++) {
         // update the local error
         loc_err += fabs(network->layers[network->num_layers-1][j].out - data->entries[i].output[j]);
         
         // find the network's guess ie the output neurone with the maximum value
         if(arg_max == -1 || network->layers[network->num_layers-1][j].out > network->layers[network->num_layers-1][arg_max].out) {
            arg_max = j;
         }

         // extract the training data's base truth ie find out which output neurone
         // has the biggest value in the training data
         if(base == -1 || data->entries[i].output[j] > data->entries[i].output[base]) {
            base = j;
         }
      }

      if(arg_max == base) {
         num_correct ++;
         printf("CORRECT\n");
      } else {
         printf("WRONG\n");
      }

      err += loc_err;
   }

   printf("Statistics of the NN on the testing data:\n"
          "Total global error: %g, average = %g\n"
          "Total accuracy: %d/%d = %g\n", 
          err, err / data->size, 
          num_correct, data->size,
          ((float)num_correct) / data->size);

}

int main(int argc, char *argv[])
{
   if(argc != 3){
     fprintf(stderr, "Usage: ./percepton [training_data] [testing_data]\n");
     exit(1);
   }
   TRAINING_DATA *train_data = readTrainingData(argv[1]);
   TRAINING_DATA *test_data = readTrainingData(argv[2]);

   if(train_data->num_in != test_data->num_in || train_data->num_out != test_data->num_out) {
     fprintf(stderr, "Testing data size doesn't match the training data\n");
     exit(1);
   }

   const int layer_sizes[] = {train_data->num_in, train_data->num_out};
   NETWORK *network = initNetwork(2, layer_sizes);

   trainNetwork(network, train_data);

   testNetwork(network, test_data);

   destroyNetwork(&network);
   destoryTrainingData(&train_data);
   destoryTrainingData(&test_data);

   return 0;
}
