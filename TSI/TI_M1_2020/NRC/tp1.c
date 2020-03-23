#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <dirent.h>
#include <limits.h>
#include <fcntl.h>

#include "def.h"
#include "nrio.h"
#include "nrarith.h"
#include "nralloc.h"


/* 2D convolution with a mask
 * Retuns an imatrix to avoid overflows
 */
int** conv2(byte** f, long nrl, long nrh, long ncl, long nch,
            float** mask, long maskw, long maskh)
{
   int** out = imatrix(nrl, nrh, ncl, nch);

   for (int x = nrl; x < nrh; x++) {
      for (int y = ncl; y < nch; y++) {
         double acc = 0.0;
         int n = 0;

         for (int u = 0; u < maskw; u++) {
            for (int v = 0; v < maskh; v++) {
               int nx = x + u - ((int) (maskw / 2));
               int ny = y + v - ((int) (maskh / 2));

               if(nx >= nrl && nx < nrh && ny >= ncl && ny < nch) {
                  acc += f[nx][ny] * mask[u][v];
                  n++;
               }

            }
         }

         out[x][y] = acc / n;
      }
   }

   return out;
}

/* map a function to every element of an imatrix
 */
void map(int** m, long nrl, long nrh, long ncl, long nch, int (*func)(int))
{

   for (int x = nrl; x < nrh; x++) {
      for (int y = ncl; y < nch; y++) {
         m[x][y] = func(m[x][y]);
      }
   }
}

/* Add two imatrices
 */
int** add(int **m1, int **m2, long nrl, long nrh, long ncl, long nch)
{
   int** out = imatrix(nrl, nrh, ncl, nch);

   for (int x = nrl; x < nrh; x++) {
      for (int y = ncl; y < nch; y++) {
         out[x][y] = m1[x][y] + m2[x][y];
      }
   }

   return out;
}

/* Convert an imatrix to a bmatrix
 * PS: casting to smaller type can be lossy
 */
byte** convert_imatrix_bmatrix(int** m, long nrl, long nrh, long ncl, long nch)
{

   byte** out = bmatrix(nrl, nrh, ncl, nch);

   for (int x = nrl; x < nrh; x++) {
      for (int y = ncl; y < nch; y++) {
         out[x][y] = (byte) m[x][y];
      }
   }

   free_imatrix(m, nrl, nrh, ncl, nch);

   return out;
}

int pow2(int x)
{
   return x * x;
}

int sqrt2(int x)
{
   return (int) sqrt((double) x);
}

int ceil2(int x)
{
   return 255 * (x > 5) ;
}

byte** bgradient(byte** I, long nrl, long nrh, long ncl, long nch)
{
   float maskd1[3][3] = {{-1, -2, -1},
      {0, 0, 0},
      {1, 2, 1}
   };
   float maskd2[3][3] = {{-1, 0, 1},
      {-2, 0, 2},
      {-1, 0, 1}
   };

   float** mask1 = malloc(sizeof(float*) * 3);
   float** mask2 = malloc(sizeof(float*) * 3);

   for (int i = 0; i < 3; i++) {
      mask1[i] = malloc(sizeof(float) * 3);
      mask2[i] = malloc(sizeof(float) * 3);

      for (int j = 0; j < 3; j++) {
         mask1[i][j] = maskd1[i][j];
         mask2[i][j] = maskd2[i][j];
      }
   }

   /* Calculate convolutions for the two masks */
   int** im1 = conv2(I, nrl, nrh, ncl, nch, mask1, 3, 3); // horizontal
   int** im2 = conv2(I, nrl, nrh, ncl, nch, mask2, 3, 3); // vertical

   /* Normalize */
   /* map square */
   map(im1, nrl, nrh, ncl, nch, pow2);
   map(im2, nrl, nrh, ncl, nch, pow2);

   /* add two matrices */
   int** sum = add(im1, im2, nrl, nrh, ncl, nch);
   /* map square root */
   map(sum, nrl, nrh, ncl, nch, sqrt2);

   /* map ceil */
   map(sum, nrl, nrh, ncl, nch, ceil2);

   /* Free the dynamically allocated masks */
   for (int i = 0; i < 3; i++) {
      free(mask1[i]);
      free(mask2[i]);
   }

   free(mask1);
   free(mask2);

   /* Free the convolution results*/
   free_imatrix(im1, nrl, nrh, ncl, nch);
   free_imatrix(im2, nrl, nrh, ncl, nch);

   /* convert to byte matrix and save (and frees the sum matrix) */
   return convert_imatrix_bmatrix(sum, nrl, nrh, ncl, nch);
}

rgb8** rgb8gradient(rgb8** m, long nrl, long nrh, long ncl, long nch)
{
   byte **R = bmatrix(nrl, nrh, ncl, nch),
        **G = bmatrix(nrl, nrh, ncl, nch),
        **B = bmatrix(nrl, nrh, ncl, nch);

   for (int x = nrl; x < nrh; x++) {
      for (int y = ncl; y < nch; y++) {
         R[x][y] = m[x][y].r;
         G[x][y] = m[x][y].g;
         B[x][y] = m[x][y].b;
      }
   }

   byte** R_Grad = bgradient(R, nrl, nrh, ncl, nch);
   byte** G_Grad = bgradient(G, nrl, nrh, ncl, nch);
   byte** B_Grad = bgradient(B, nrl, nrh, ncl, nch);
   free_bmatrix(R, nrl, nrh, ncl, nch);
   free_bmatrix(G, nrl, nrh, ncl, nch);
   free_bmatrix(B, nrl, nrh, ncl, nch);


   rgb8** out = rgb8matrix(nrl, nrh, ncl, nch);
   
   // number of components that has to be over the threshold to be considered
   // ex R and G or G and B
   int threshold = 2;
   for (int x = nrl; x < nrh; x++) {
      for (int y = ncl; y < nch; y++) {
         out[x][y].r = out[x][y].g = out[x][y].b = 255 * ((R_Grad[x][y] == 255) + (G_Grad[x][y] == 255) + (B_Grad[x][y] == 255) > threshold);
      }
   }

   free_bmatrix(R_Grad, nrl, nrh, ncl, nch);
   free_bmatrix(G_Grad, nrl, nrh, ncl, nch);
   free_bmatrix(B_Grad, nrl, nrh, ncl, nch);

   return out;
}

int min(int a, int b) {
   return (a < b)?a:b;
}
int max(int a, int b) {
   return (a > b)?a:b;
}

int** label(byte** I, long nrl, long nrh, long ncl, long nch, int* num_labels)
{
   int** labels = imatrix0(nrl, nrh, ncl, nch);
   int init_size = 10; 
   int cur_size = 10;
   int cur = 0;
   int* labelMap = malloc(sizeof(int) * init_size);
   
   // init labels and map
   for (int i = 0; i < init_size; i++) {
      labelMap[i] = i;
   }
   
   for (int y = nrl; y < nrh; y++) {
      for (int x = ncl; x < nch; x++) {
         if(I[y][x] != 0) {
            int C = labels[y][x];

            int A = (x-1 >= 0)? labels[y][x-1]: 0, PA = (x-1 >= 0)? I[y][x-1]: 0;
            int B = (y-1 >= 0)? labels[y-1][x]: 0, PB = (y-1 >= 0)? I[y-1][x]: 0;
            
            if(PA != 0 && PB == 0) {
               labels[y][x] = A;
            } else if(PB != 0 && PA == 0) {
               labels[y][x] = labelMap[B];
            } else if(PA == 0 && PB == 0) {
               cur ++;
               
               // if we need to realloc the labelMap
               if(cur >= cur_size) {
                  int new_size = cur_size * 2;
                  labelMap = realloc(labelMap, sizeof(int) * new_size);
                  for(int i = cur_size; i < new_size; i++) {
                     labelMap[i] = i;
                  }
                  cur_size = new_size;
               }
               
               labels[y][x] = cur;
            } else if(PA != 0 && PB != 0 && A == B) {
               labels[y][x] = A;
            } else if(PA != 0 && PB != 0 && A != B) {
               labels[y][x] = min(labelMap[B], A);
               C = labels[y][x];
               labelMap[C] = C;
               labelMap[A] = C;
               labelMap[max(labelMap[B], A)] = C;
            }
         }
      }  
   }

   // update label map
   int label = 0;
   for (int i = 1; i <cur; i++) {
      if(labelMap[i] == i) {
         label ++;
         labelMap[i] = label;
      } else {
         labelMap[i] = labelMap[labelMap[i]];
      }
   }
   *num_labels = label;

   for (int y = nrl; y < nrh; y++) {
      for (int x = ncl; x < nch; x++) {
         labels[y][x] = labelMap[labels[y][x]];    
      }
   }

   printf("\n");
   free(labelMap);
   return labels;   
}

rgb8** colorLabels(int** labels, long nrl, long nrh, long ncl, long nch, int num_labels) {
   rgb8** res = rgb8matrix(nrl, nrh, ncl, nch);
   rgb8* colorMap = rgb8vector(1, num_labels);
   for (int i = 0; i < num_labels; i++) {
      colorMap[i].r = rand() % 256;
      colorMap[i].g = rand() % 256;
      colorMap[i].b = rand() % 256;
   }

   for (int i = nrl; i < nrh; i++) {
      for (int j = ncl; j < nch; j++) {
         if(labels[i][j] != 0)
            res[i][j] = colorMap[labels[i][j]];
         else {
            res[i][j].r = 0; res[i][j].g = 0; res[i][j].b = 0;
         }
      }
   }
   return res;
}

byte** getLabel(int** labels, int nrl, int nrh, int ncl, int nch, int label)
{
   byte** res = bmatrix(nrl, nrh, ncl, nch);
   for (int x = ncl; x < nch; x++) {
      for (int y = nrl; y < nrh; y++) {
         res[y][x] = (label == labels[y][x])? 255: 0;
      }
   }
   return res;
}

byte** erosion(byte** f, long nrl, long nrh, long ncl, long nch, float** mask, long maskw, long maskh, int repeats) {
   float mask_sum = 0;

   for (int u = 0; u < maskw; u++) {
      for (int v = 0; v < maskh; v++) {
         mask_sum += mask[u][v];
      }
   }
   byte ** out = bmatrix(nrl, nrh, ncl, nch);
   for (int x = nrl; x < nrh; x++) {
      for (int y = ncl; y < nch; y++) {
         out[x][y] = f[x][y];
      }
   }

   byte ** out2 = bmatrix(nrl, nrh, ncl, nch);

   for (int i = 0; i < repeats; i++) {
      for (int x = nrl; x < nrh; x++) {
         for (int y = ncl; y < nch; y++) {
            float acc = 0.0;

            for (int u = 0; u < maskw; u++) {
               for (int v = 0; v < maskh; v++) {
                  int nx = x + u - ((int) (maskw / 2));
                  int ny = y + v - ((int) (maskh / 2));

                  if(nx >= nrl && nx < nrh && ny >= ncl && ny < nch) {
                     acc += (out[nx][ny] / 255) * mask[u][v];
                  }

               }
            }

            out2[x][y] = (acc == mask_sum) * 255;
         }
      }

      byte** temp = out2;
      out2 = out;
      out = temp;
   }
   
   free_bmatrix(out2, nrl, nrh, ncl, nch);
   
   return out;
}

byte** dilation(byte** f, long nrl, long nrh, long ncl, long nch, float** mask, long maskw, long maskh, int repeats) {
   byte ** out = bmatrix(nrl, nrh, ncl, nch);
   for (int x = nrl; x < nrh; x++) {
      for (int y = ncl; y < nch; y++) {
         out[x][y] = f[x][y];
      }
   }

   byte ** out2 = bmatrix(nrl, nrh, ncl, nch);

   for (int i = 0; i < repeats; i++) {
      for (int x = nrl; x < nrh; x++) {
         for (int y = ncl; y < nch; y++) {
            float acc = 0.0;

            for (int u = 0; u < maskw; u++) {
               for (int v = 0; v < maskh; v++) {
                  int nx = x + u - ((int) (maskw / 2));
                  int ny = y + v - ((int) (maskh / 2));

                  if(nx >= nrl && nx < nrh && ny >= ncl && ny < nch) {
                     acc += (out[nx][ny] / 255) * mask[u][v];
                  }

               }
            }

            out2[x][y] = (acc > 0) * 255;
         }
      }

      byte** temp = out2;
      out2 = out;
      out = temp;
   }
   
   free_bmatrix(out2, nrl, nrh, ncl, nch);
   
   return out;
}

byte** openingMorph(byte** f, long nrl, long nrh, long ncl, long nch, float** mask, long maskw, long maskh, int repeats) {
   byte** E = erosion(f, nrl, nrh, ncl, nch, mask, maskw, maskh, repeats);
   byte** D = dilation(E, nrl, nrh, ncl, nch, mask, maskw, maskh, repeats);
   free_bmatrix(E, nrl, nrh, ncl, nch);

   return D; 
}

byte** closingMorph(byte** f, long nrl, long nrh, long ncl, long nch, float** mask, long maskw, long maskh, int repeats) {
   byte** D = dilation(f, nrl, nrh, ncl, nch, mask, maskw, maskh, repeats);
   byte** E = erosion(D, nrl, nrh, ncl, nch, mask, maskw, maskh, repeats);
   free_bmatrix(D, nrl, nrh, ncl, nch);

   return E; 
}

int reverse(int x) {
   return 255 - x;
}

// chacks for the .ppm extension
int imageNameFormat(const struct dirent* d) {
   int len = strlen(d->d_name);
   if(len > 4 && strcmp(d->d_name + len - 4, ".ppm") == 0) {
      return 1;
   }
   return 0;
}

void imageDiff(char* dirp, char* dirout) {
   printf("Detecting movement\n");

   struct dirent **namelist;
   int n = scandir(dirp, &namelist, imageNameFormat, alphasort);
   if(n < 0) {
      fprintf(stderr, "Error scandir\n");
      exit(-1);
   }
   if(n < 2) {
      fprintf(stderr, "Not enough images\n");
      exit(-1);
   }


   int dirp_len = strlen(dirp);
   int dirout_len = strlen(dirout);
  
   // format check (needs a / at the end of the directory name)
   if(dirp[dirp_len-1] != '/') {
      dirp = strdup(dirp);
      dirp = realloc(dirp, dirp_len+2);
      dirp[dirp_len] = '/';
      dirp[dirp_len+1] = '\0';
      dirp_len++;
   }

   if(dirout[dirout_len-1] != '/') {
      dirout = strdup(dirout);
      dirout = realloc(dirout, dirout_len+2);
      dirout[dirout_len] = '/';
      dirout[dirout_len+1] = '\0';
      dirout_len++;
   }

   char* curpath = malloc(sizeof(char) * (dirp_len + FILENAME_MAX));
   char* outpath = malloc(sizeof(char) * (dirout_len + FILENAME_MAX));
   strcpy(curpath, dirp);
   strcpy(outpath, dirout);

   long nrl, nrh, ncl, nch;
   // load the first image
   memcpy(curpath + dirp_len, namelist[0]->d_name, FILENAME_MAX);
   rgb8** A = LoadPPM_rgb8matrix(curpath, &nrl, &nrh, &ncl, &nch), 
      **B = NULL;
   rgb8** out = rgb8matrix(nrl, nrh, ncl, nch);

   for(int i = 1; i < n; i++) {
      // read new image
      if(B != NULL) {
         free_rgb8matrix(B, nrl, nrh, ncl, nch);
      }
      B = A;
      memcpy(curpath + dirp_len, namelist[i]->d_name, FILENAME_MAX);
      A = LoadPPM_rgb8matrix(curpath, &nrl, &nrh, &ncl, &nch);

      // calc A - B
      for (int x = nrl; x < nrh; x++) {
         for (int y = ncl; y < nch; y++) {
            out[x][y].r = abs(A[x][y].r - B[x][y].r);
            out[x][y].g = abs(A[x][y].g - B[x][y].g);
            out[x][y].b = abs(A[x][y].b - B[x][y].b);
         }
      }

      // write out
      char* outfilename = malloc(sizeof(char) * FILENAME_MAX);
      sprintf(outfilename, "diff%03d.ppm", i);
      memcpy(outpath + dirout_len, outfilename, FILENAME_MAX);
      SavePPM_rgb8matrix(out, nrl, nrh, ncl, nch, outpath);
   }
   free_rgb8matrix(A, nrl, nrh, ncl, nch);
   free_rgb8matrix(B, nrl, nrh, ncl, nch);
   free_rgb8matrix(out, nrl, nrh, ncl, nch);
}

rgb8** imageAvg(char* dirp, long* nrl, long* nrh, long* ncl, long* nch) {
   struct dirent **namelist;
   int n = scandir(dirp, &namelist, imageNameFormat, alphasort);
   if(n < 0) {
      fprintf(stderr, "Error scandir\n");
      exit(-1);
   }
   if(n < 2) {
      fprintf(stderr, "Not enough images\n");
      exit(-1);
   }


   int dirp_len = strlen(dirp);
  
   // format check (needs a / at the end of the directory name)
   if(dirp[dirp_len-1] != '/') {
      dirp = strdup(dirp);
      dirp = realloc(dirp, dirp_len+2);
      dirp[dirp_len] = '/';
      dirp[dirp_len+1] = '\0';
      dirp_len++;
   }

   char* curpath = malloc(sizeof(char) * (dirp_len + FILENAME_MAX));
   strcpy(curpath, dirp);

   rgb8** A;
   
   // read the first image to get the size
   memcpy(curpath + dirp_len, namelist[0]->d_name, FILENAME_MAX);
   A = LoadPPM_rgb8matrix(curpath, nrl, nrh, ncl, nch);
   free_rgb8matrix(A, *nrl, *nrh, *ncl, *nch);

   int ***out = malloc(sizeof(int**) * (*nrh+1));
   for (int i = 0; i < *nrh; i++) {
      out[i] = malloc(sizeof(int*) * (*nch+1));
      for (int j = 0; j < *nch; j++) {
         out[i][j] = calloc(3, sizeof(int));
      }
   }

   for(int i = 0; i < n; i++) {
      // read new image
      memcpy(curpath + dirp_len, namelist[i]->d_name, FILENAME_MAX);
      A = LoadPPM_rgb8matrix(curpath, nrl, nrh, ncl, nch);

      // add A to out
      for (int x = *nrl; x < *nrh; x++) {
         for (int y = *ncl; y < *nch; y++) {
            out[x][y][0] += A[x][y].r;
            out[x][y][1] += A[x][y].g;
            out[x][y][2] += A[x][y].b;
         }
      }

      free_rgb8matrix(A, *nrl, *nrh, *ncl, *nch);
   }
   
   rgb8** rgb_out = rgb8matrix(*nrl, *nrh, *ncl, *nch);
   for (int x = *nrl; x < *nrh; x++) {
      for (int y = *ncl; y < *nch; y++) {
         rgb_out[x][y].r = (byte)(((float)out[x][y][0]) / n);
         rgb_out[x][y].g = (byte)(((float)out[x][y][1]) / n);
         rgb_out[x][y].b = (byte)(((float)out[x][y][2]) / n);
      }
   }
   for (int i = 0; i < *nrh; i++) {
      for (int j = 0; j < *nch; j++) {
         free(out[i][j]);
      }
      free(out[i]);
   }
   free(out);

   return rgb_out;
}

void imageDiffAvg(char* dirp, char* dirout, ) {
   printf("Detecting movement\n");

   struct dirent **namelist;
   int n = scandir(dirp, &namelist, imageNameFormat, alphasort);
   if(n < 0) {
      fprintf(stderr, "Error scandir\n");
      exit(-1);
   }
   if(n < 2) {
      fprintf(stderr, "Not enough images\n");
      exit(-1);
   }

   int dirp_len = strlen(dirp);
   int dirout_len = strlen(dirout);
  
   // format check (needs a / at the end of the directory name)
   if(dirp[dirp_len-1] != '/') {
      dirp = strdup(dirp);
      dirp = realloc(dirp, dirp_len+2);
      dirp[dirp_len] = '/';
      dirp[dirp_len+1] = '\0';
      dirp_len++;
   }

   if(dirout[dirout_len-1] != '/') {
      dirout = strdup(dirout);
      dirout = realloc(dirout, dirout_len+2);
      dirout[dirout_len] = '/';
      dirout[dirout_len+1] = '\0';
      dirout_len++;
   }

   char* curpath = malloc(sizeof(char) * (dirp_len + FILENAME_MAX));
   char* outpath = malloc(sizeof(char) * (dirout_len + FILENAME_MAX));
   strcpy(curpath, dirp);
   strcpy(outpath, dirout);

   long nrl, nrh, ncl, nch;
   // load the first image
   memcpy(curpath + dirp_len, namelist[0]->d_name, FILENAME_MAX);
   rgb8** A = LoadPPM_rgb8matrix(curpath, &nrl, &nrh, &ncl, &nch), 
       **Iref = imageAvg(dirp, &nrl, &nrh, &ncl, &nch); // CALCULATE AVG OF VIDEO

   for(int i = 0; i < n; i++) {
      // read new image
      memcpy(curpath + dirp_len, namelist[i]->d_name, FILENAME_MAX);
      A = LoadPPM_rgb8matrix(curpath, &nrl, &nrh, &ncl, &nch);

      // add A to out
      for (int x = nrl; x < nrh; x++) {
         for (int y = ncl; y < nch; y++) {
            A[x][y].r = abs(A[x][y].r - Iref[x][y].r);
            A[x][y].g = abs(A[x][y].g - Iref[x][y].g);
            A[x][y].b = abs(A[x][y].b - Iref[x][y].b);
         }
      }

      // write out
      char* outfilename = malloc(sizeof(char) * FILENAME_MAX);
      sprintf(outfilename, "diff%03d.ppm", i);
      memcpy(outpath + dirout_len, outfilename, FILENAME_MAX);
      SavePPM_rgb8matrix(A, nrl, nrh, ncl, nch, outpath);
 
      free_rgb8matrix(A, nrl, nrh, ncl, nch);
   }
 
   free_rgb8matrix(Iref, nrl, nrh, ncl, nch);
}

rgb8** imageMedian(char* dirp, long *nrl, long *nrh, long *ncl, long *nch) {
   struct dirent **namelist;
   int n = scandir(dirp, &namelist, imageNameFormat, alphasort);
   if(n < 0) {
      fprintf(stderr, "Error scandir\n");
      exit(-1);
   }
   if(n < 2) {
      fprintf(stderr, "Not enough images\n");
      exit(-1);
   }


   int dirp_len = strlen(dirp);
  
   // format check (needs a / at the end of the directory name)
   if(dirp[dirp_len-1] != '/') {
      dirp = strdup(dirp);
      dirp = realloc(dirp, dirp_len+2);
      dirp[dirp_len] = '/';
      dirp[dirp_len+1] = '\0';
      dirp_len++;
   }

   char* curpath = malloc(sizeof(char) * (dirp_len + FILENAME_MAX));
   strcpy(curpath, dirp);

   rgb8** A;
   
   // read the first image to get the size
   memcpy(curpath + dirp_len, namelist[0]->d_name, FILENAME_MAX);
   A = LoadPPM_rgb8matrix(curpath, nrl, nrh, ncl, nch);
   free_rgb8matrix(A, *nrl, *nrh, *ncl, *nch);

   int ***out = malloc(sizeof(int**) * (*nrh+1));
   for (int i = 0; i < *nrh; i++) {
      out[i] = malloc(sizeof(int*) * (*nch+1));
      for (int j = 0; j < *nch; j++) {
         out[i][j] = calloc(256*3, sizeof(int));
      }
   }

   printf("Calculating median image\n");
   for(int i = 0; i < n; i++) {
      // read new image
      memcpy(curpath + dirp_len, namelist[i]->d_name, FILENAME_MAX);
      A = LoadPPM_rgb8matrix(curpath, nrl, nrh, ncl, nch);

      // add A to out
      for (int x = *nrl; x < *nrh; x++) {
         for (int y = *ncl; y < *nch; y++) {
            out[x][y][0  +A[x][y].r] ++;
            out[x][y][256+A[x][y].g] ++;
            out[x][y][512+A[x][y].b] ++;
         }
      }
      
      free_rgb8matrix(A, *nrl, *nrh, *ncl, *nch);
      printf("[%d/%d] %s\r", i+1, n, curpath);
   }
   printf("\n");
   
   rgb8** rgb_out = rgb8matrix(*nrl, *nrh, *ncl, *nch);
   for (int x = *nrl; x < *nrh; x++) {
      for (int y = *ncl; y < *nch; y++) {
         int rf = 0, gf = 0, bf = 0;
         int rc = 0, gc = 0, bc = 0;
         int middle = n / 2;
         for (int i = 0; i <= 255; i++) {
            rc += out[x][y][i]; gc += out[x][y][256+i]; bc += out[x][y][512+i];

            if(rf == 0 && rc > middle) {
               rgb_out[x][y].r = i;
               rf = 1;
            }

            if(gf == 0 && gc > middle) {
               rgb_out[x][y].g = i;
               gf = 1;
            }

            if(bf == 0 && bc > middle) {
               rgb_out[x][y].b = i;
               bf = 1;
            }
         }
      }
   }

   for (int i = 0; i < *nrh; i++) {
      for (int j = 0; j < *nch; j++) {
         free(out[i][j]);
      }
      free(out[i]);
   }
   free(out);

   return rgb_out;
}

void imageDiffMedian(char* dirp, char* dirout) {
   printf("Detecting movement\n");

   struct dirent **namelist;
   int n = scandir(dirp, &namelist, imageNameFormat, alphasort);
   if(n < 0) {
      fprintf(stderr, "Error scandir\n");
      exit(-1);
   }
   if(n < 2) {
      fprintf(stderr, "Not enough images\n");
      exit(-1);
   }

   int dirp_len = strlen(dirp);
   int dirout_len = strlen(dirout);
  
   // format check (needs a / at the end of the directory name)
   if(dirp[dirp_len-1] != '/') {
      dirp = strdup(dirp);
      dirp = realloc(dirp, dirp_len+2);
      dirp[dirp_len] = '/';
      dirp[dirp_len+1] = '\0';
      dirp_len++;
   }

   if(dirout[dirout_len-1] != '/') {
      dirout = strdup(dirout);
      dirout = realloc(dirout, dirout_len+2);
      dirout[dirout_len] = '/';
      dirout[dirout_len+1] = '\0';
      dirout_len++;
   }

   char* curpath = malloc(sizeof(char) * (dirp_len + FILENAME_MAX));
   char* outpath = malloc(sizeof(char) * (dirout_len + FILENAME_MAX));
   strcpy(curpath, dirp);
   strcpy(outpath, dirout);

   long nrl, nrh, ncl, nch;
   // load the first image
   memcpy(curpath + dirp_len, namelist[0]->d_name, FILENAME_MAX);
   rgb8** A = LoadPPM_rgb8matrix(curpath, &nrl, &nrh, &ncl, &nch), 
       **Iref = imageMedian(dirp, &nrl, &nrh, &ncl, &nch); // CALCULATE AVG OF VIDEO

   for(int i = 0; i < n; i++) {
      // read new image
      memcpy(curpath + dirp_len, namelist[i]->d_name, FILENAME_MAX);
      A = LoadPPM_rgb8matrix(curpath, &nrl, &nrh, &ncl, &nch);

      // add A to out
      for (int x = nrl; x < nrh; x++) {
         for (int y = ncl; y < nch; y++) {
            A[x][y].r = abs(A[x][y].r - Iref[x][y].r);
            A[x][y].g = abs(A[x][y].g - Iref[x][y].g);
            A[x][y].b = abs(A[x][y].b - Iref[x][y].b);
         }
      }

      // write out
      char* outfilename = malloc(sizeof(char) * FILENAME_MAX);
      sprintf(outfilename, "diff%03d.ppm", i);
      memcpy(outpath + dirout_len, outfilename, FILENAME_MAX);
      SavePPM_rgb8matrix(A, nrl, nrh, ncl, nch, outpath);
 
      free_rgb8matrix(A, nrl, nrh, ncl, nch);
   }
 
   free_rgb8matrix(Iref, nrl, nrh, ncl, nch);
}

void imageCeil(char* dirp, int ceilval) {
   struct dirent **namelist;
   int n = scandir(dirp, &namelist, imageNameFormat, alphasort);
   if(n < 0) {
      fprintf(stderr, "Error scandir\n");
      exit(-1);
   }
   if(n < 2) {
      fprintf(stderr, "Not enough images\n");
      exit(-1);
   }

   int dirp_len = strlen(dirp);
   int dirout_len = strlen(dirout);
  
   // format check (needs a / at the end of the directory name)
   if(dirp[dirp_len-1] != '/') {
      dirp = strdup(dirp);
      dirp = realloc(dirp, dirp_len+2);
      dirp[dirp_len] = '/';
      dirp[dirp_len+1] = '\0';
      dirp_len++;
   }

   if(dirout[dirout_len-1] != '/') {
      dirout = strdup(dirout);
      dirout = realloc(dirout, dirout_len+2);
      dirout[dirout_len] = '/';
      dirout[dirout_len+1] = '\0';
      dirout_len++;
   }

   char* curpath = malloc(sizeof(char) * (dirp_len + FILENAME_MAX));
   char* outpath = malloc(sizeof(char) * (dirout_len + FILENAME_MAX));
   strcpy(curpath, dirp);
   strcpy(outpath, dirout);

   rgb8** A;
   
   // read the first image to get the size
   memcpy(curpath + dirp_len, namelist[0]->d_name, FILENAME_MAX);
   A = LoadPPM_rgb8matrix(curpath, nrl, nrh, ncl, nch);
   free_rgb8matrix(A, *nrl, *nrh, *ncl, *nch);

   for(int i = 0; i < n; i++) {
      // read new image
      memcpy(curpath + dirp_len, namelist[i]->d_name, FILENAME_MAX);
      A = LoadPPM_rgb8matrix(curpath, nrl, nrh, ncl, nch);

      // add A to out
      for (int x = *nrl; x < *nrh; x++) {
         for (int y = *ncl; y < *nch; y++) {
            A[x][y].r = A[x][y].r > ceilval * 255;
            A[x][y].g = A[x][y].g > ceilval * 255;
            A[x][y].b = A[x][y].b > ceilval * 255;
         }
      }

      // write out
      char* outfilename = malloc(sizeof(char) * FILENAME_MAX);
      sprintf(outfilename, "ceil%03d.ppm", i);
      memcpy(outpath + dirout_len, outfilename, FILENAME_MAX);
      SavePPM_rgb8matrix(A, nrl, nrh, ncl, nch, outpath);
 
      free_rgb8matrix(A, *nrl, *nrh, *ncl, *nch);
   }
   
   return rgb_out;
}

int main(int* argc, char** argv)
{
   /*
   long nrh, nrl,
        nch, ncl;
   byte **I;

   I = LoadPGM_bmatrix("../Images/Test/carreBruit.pgm", &nrl, &nrh, &ncl, &nch);
   
   int r = 3, l = 3;
   float** mask = malloc(r * sizeof(float*));
   for (int i = 0; i < r; i++) {
      mask[i] = malloc(l * sizeof(float));
      for (int j = 0; j < l; j++) {
         mask[i][j] = 1;
      }
   }

   int repeats = 8;
   byte** closed = openingMorph(I, nrl, nrh, ncl, nch, mask, r, l, repeats);

   SavePGM_bmatrix(closed, nrl, nrh, ncl, nch, "./opened.pgm");
   */
   // SavePGM_bmatrix(Grad, nrl, nrh, ncl, nch, "./cubesx3_gredient.pgm");
   
   // int num_labels = 0;
   //int** labels = label(Grad, nrl, nrh, ncl, nch, &num_labels);
   //byte** l = convert_imatrix_bmatrix(labels, nrl, nrh, ncl, nch);
   //map(l, nrl, nrh, ncl, nch, reverse);
   //byte** l = getLabel(labels, nrl, nrh, ncl, nch, 300);
   //SavePGM_bmatrix(l, nrl, nrh, ncl, nch, "./cubesx3_gredient.pgm");
   //
   //rgb8** colLabels = colorLabels(labels, nrl, nrh, ncl, nch, num_labels);
   //SavePPM_rgb8matrix(colLabels, nrl, nrh, ncl, nch, "./color_labels.ppm");
   
  /* 
   long nrl, nrh, ncl, nch;
   rgb8** avg = imageMedian(argv[1], &nrl, &nrh, &ncl, &nch);
   printf("%d %d %d %d\n", nrl, nrh, ncl,nch);
   SavePPM_rgb8matrix(avg, nrl, nrh, ncl, nch, "./median.ppm");
    */  

   //long nrl, nrh, ncl, nch;
   imageDiffMedian(argv[1], argv[2]);
         
   return 0;
}
