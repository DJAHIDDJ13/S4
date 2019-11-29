#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct neurone {
   float *weight;
   float (*squish_func)(float);
   float out;
} NEURONE;

struct network {
   NEURONE **layers;
   int *sizes;
} NETWORK;

void error_calc() {

}

void train(NETWORK *network) {
   
}

float identity(float x)
{
   return x;
}

float sigmoid(float )
{
   return 1 / (1 + fexp(-x));
}

int main(int argc, const char *argv[])
{
   float C_im[] = {1, 1, 1, 1,
                   1, 0, 0, 0,
                   1, 0, 0, 0,
                   1, 0, 0, 0,
                   1, 1, 1, 1
                  };

   float A_im[] = {0, 1, 1, 0,
                   1, 0, 0, 1,
                   1, 1, 1, 1,
                   1, 0, 0, 1,
                   1, 0, 0, 1
                  };
   int num_in = sizeof(A_im) / sizeof(float);

   NETWORK network;
   network.layers = malloc(sizeof(NEURONE*) * 2); // two layers input and output
   network.sizes = malloc(sizeof(int) * 2);
   network.sizes[0] = num_in;
   network.sizes[1] = 2;

   NEURONE * in_neurones = malloc(sizeof(NEURONE) * num_in); // input neurones

   for(int i = 0; i < num_in; i++) {
      inputs[i].weights = malloc(sizeof(float)); // one input for the 'in' neurones
      inputs[i].squish_func = identity;
   }


   NEURONE *out_neurones = malloc(sizeof(NEURONE) * 2); // two output neurones, either A or C
   out_neurones[0].squish_func = sigmoid;
   out_neurones[1].squish_func = sigmoid;
   out_neurones[0].weights = malloc(sizeof(float) * num_in); // every pixel from the input neurones has a weight
   out_neurones[1].weights = malloc(sizeof(float) * num_in); // every pixel from the input neurones has a weight

   for(int i = 0; i < num_in; i++) {
      out_neurones[0].weights[i] = rand() / RAND_MAX; // initialize weights randomly between 0-1
      out_neurones[1].weights[i] = rand() / RAND_MAX;
   }

   network.layers[0] = in_neurones;
   network.layers[1] = out_neurones;
   return 0;
}
