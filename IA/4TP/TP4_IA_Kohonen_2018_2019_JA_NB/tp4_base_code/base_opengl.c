/*
#########################
Installation des packages
sudo apt install libglu1-mesa-dev freeglut3-dev mesa-common-dev
#########################
Simple programme d'affichage de points et de segments en opengl
utilise GL et glut
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <GL/glut.h>
#include <time.h>
#include <string.h>
#include <limits.h>

#include "base_opengl.h"
#include "ppm.h"
#include "kohonen.h"

#define DEFAULT_WIDTH  500
#define DEFAULT_HEIGHT 500
#define NB_VILLE 22
#define MODE 0 // 1 FOR TRAVELING SALESMAN PROBLEM, 0 FOR IMAGE COMPRESSION

int cpt = 0;
int calc = 0;
char presse;
Point ville[NB_VILLE];
int anglex = 0;
int angley = 0;
int x, y, xold, yold;
GLuint textureID;

int width  = DEFAULT_WIDTH;
int height = DEFAULT_HEIGHT;

unsigned char * img = NULL;

/* Global variables for the network */
float EPSILON = 0.1;
KOHONEN* map;
TRAINING_DATA* DataSet;

/* Exercice 1: initilize the data with random values between 0, 200 */
TRAINING_DATA* initialiseSet()
{
   TRAINING_DATA* DataSet = initTrainingData(20, 2);

   for(int i = 0; i < 20; i++) {
      DataSet->input[i][0] = rand() % 200;
      DataSet->input[i][1] = rand() % 200;
   }

   return DataSet;
}

/* Reset the weights of the maps to random values between minval and maxval*/
void resetMap(KOHONEN* m, int minval, int maxval)
{
   for (int i = 0; i < m->sizeX * m->sizeY; i++) {
      for (int j = 0; j < m->sizeInput; j++) {
         m->weight[i][j] = ((float)rand()) / ((float)RAND_MAX) *  (minval + maxval) - minval;
      }
   }
}


/**
 * Snap the neurones' weights to the closest data entry (with smallest
 * euclidian distance) in order to find the solution to the travelling salesman
 * problem (finding the result path)
 */
void snapToData(KOHONEN* m, TRAINING_DATA* data)
{
   int total_size = m->sizeX * m->sizeY;
   int *seen = calloc(total_size, sizeof(float));

   // for each city
   for(int i = 0; i < data->numInput; i++) {
      int closestArg = -1;
      float closest = 0;

      // find the neurone that is closest to that city
      for(int j = 0; j < total_size; j++) {
         float dist = euclidianDistance(data->input[i], m->weight[j], data->sizeInput);

         // while making sure that it was not taken by another city
         if((closestArg == -1 || dist < closest) && seen[j] == 0) {
            closestArg = j;
            closest = dist;
         }
      }

      // mark the neurone as taken and snap its coordinates to the city's
      seen[closestArg] = 1;
      memcpy(m->weight[closestArg], data->input[i], sizeof(float) * data->sizeInput);
   }

   // handling the excess neurones (because usually we use more neurones than
   // there are data entries to find the best path
   int last_snap = -1; // variable to store the index of the last neurone that has been snapped to a city

   for (int i = 0; i < total_size; i++) {
      // update the index of the last neurone that had been snapped to a city
      // and skip because we don't want to change its coordinates anymore
      if(last_snap == -1 || seen[i] == 1) {
         last_snap = i;
         continue;
      }

      // if the neurone is an excess (meaning it wasn't the closest to any
      // city), assign the coordinates of the last neurone (in order of the
      // network's topology ie from 0 to sizeX) that has been snapped to it.
      memcpy(m->weight[i], m->weight[last_snap], sizeof(float) * data->sizeInput);
   }

   // The result path can be extract from the values of last_snap

   free(seen);
}


/* Exercice 2
 * Initialize the training data to the cities' locations */
TRAINING_DATA* initialiseCitiesData()
{
   TRAINING_DATA* DataSet = initTrainingData(NB_VILLE, 2);

   for(int i = 0; i < NB_VILLE; i++) {
      DataSet->input[i][0] = ville[i].x;
      DataSet->input[i][1] = ville[i].y;
   }

   return DataSet;
}

/** Exercice 3
 * Initialize the training data to be the colors of the original image pixels
 */
TRAINING_DATA* initialiseImageData()
{
   // dividing the pixels of the image into blocks of size block_width x block_width 
   // so it works for larger images with much more pixels
   int block_width = 1;
   TRAINING_DATA* DataSet = initTrainingData(width * height / (block_width * block_width), 3);
   
   int entryNum = 0;
   for (int i = 0; i < width; i += block_width) {
      for (int j = 0; j < height; j += block_width) {
         int sR = 0,
             sG = 0,
             sB = 0;
         int total = 0;
         int limx = (width < i + block_width)? width: i + block_width;
         int limy = (height < j + block_width)? height: j + block_width;

         for (int x = i; x < limx; x++) {
            for (int y = j; y < limy; y++) {
               int pix = x * width + y;
               sR += img[3 * pix + 0]; 
               sG += img[3 * pix + 1]; 
               sB += img[3 * pix + 2];
               total ++;
            }
         }

         DataSet->input[entryNum][0] = ((float)sR) / total;
         DataSet->input[entryNum][1] = ((float)sG) / total;
         DataSet->input[entryNum][2] = ((float)sB) / total;
         entryNum++;
      }
   }
   for(int i = 0; i < DataSet->numInput; i++) {
      int j = 3 * i;
      for (int x = 0; x < block_width; x++) {
         for (int y = 0; y < block_width; y++) {
            DataSet->input[i][0] = img[j    ] ;
            DataSet->input[i][1] = img[j + 1];
            DataSet->input[i][2] = img[i + 2];
         }
      }
   }

   return DataSet;
}

float phi(float x)
{
   float lambda = 0.4;
   float beta = 0.05;

   if(x < 1) {
      return 1;
   } else if(x < 2) {
      return lambda;
   } else if(x < 3) {
      return 0.2;
   } else if(x < 4) {
      return beta;
   }

   return 0;
}

float phi3(float x)
{
   if(x < 1) {
      return 1;
   } else if(x < 2) {
      return 0.5;
   } else if(x < 3) {
      return 0.2;
   }

   return 0;
}

/* affiche la chaine fmt a partir des coordonnées x,y*/
void draw_text(float x, float y, const char *fmt, ...)
{
   char    buf[1024];      //Holds Our String
   char    *text = buf;
   va_list   ap;       // Pointer To List Of Arguments

   if (fmt == NULL) {      // If There's No Text
      return;   // Do Nothing
   }

   va_start(ap, fmt);        // Parses The String For Variables
   vsprintf(text, fmt, ap);      // And Converts Symbols To Actual Numbers
   va_end(ap);         // Results Are Stored In Text

   glDisable(GL_TEXTURE_2D);
   glRasterPos2i( x, y );

   while (*text) {
      glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *text++);
   }

   glEnable(GL_TEXTURE_2D);
}

GLuint charger_texture(unsigned char * data)
{
   GLuint textureBidule;
   glGenTextures(1, &textureBidule); /* Texture name generation */
   glBindTexture(GL_TEXTURE_2D, textureBidule); /* Binding of texture name */
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); /* We will use linear interpolation for magnification filter */
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); /* We will use linear interpolation for minifying filter */
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);;

   return textureBidule;
}

unsigned char * transform_img_to_vector(const char * filename, int *width, int *height)
{
   Image* image = NULL;
   image = readPPM(filename);

   if (image == NULL) {
      EXIT_ON_ERROR("error loading img");
   }

   int i, j;
   unsigned char* data = NULL;
   *width = image->x;
   *height = image->y;
   data = (unsigned char*)malloc(3 * image->x * image->y * sizeof(unsigned char));

   for (i = 0; i < image->x * image->y; i++) {
      j = i * 3;
      data[j] = image->data[i].r;
      data[j + 1] = image->data[i].g;
      data[j + 2] = image->data[i].b;
   }

   if (image != NULL) {
      free(image->data);
      image->data = NULL;
      free(image);
      image = NULL ;
   }

   return data;
}

// le code original n'as pas marché sur debian
void load_cities()
{
   FILE * file = NULL;

   if ((file = fopen("Villes_et_positions_dans_image.txt", "r")) == NULL) {
      EXIT_ON_ERROR("error while loading cities txt file");
   }

   int x, y;
   char *buffer = NULL;

   // %ms lis et alloc jusqu'a il trouve espace
   for(int i = 0; (fscanf(file, "%ms %d %d\n", &buffer, &x, &y)) == 3; i++) {
      strcpy(ville[i].name, buffer);
      ville[i].x = x - 5; // shift du au resize de l'image
      ville[i].y = y - 5;

      free(buffer);
   }

   fclose(file);
}

/* Initialize OpenGL Graphics */
void initGL(int w, int h)
{

#if MODE
   int taille_point = 15;
   glViewport(0, 0, w, h); // use a screen size of WIDTH x HEIGHT
#else
   int taille_point = 5;
   glViewport(0, 0, 256, 256); // use a screen size of WIDTH x HEIGHT
#endif

   glEnable(GL_TEXTURE_2D);     // Enable 2D texturing
   glMatrixMode(GL_PROJECTION);     // Make a simple 2D projection on the entire window
   glLoadIdentity();

#if MODE
   glOrtho(0.0, w, h, 0.0, -1, 1);
#else
   glOrtho(0.0, 256, 256, 0.0, -10, 10);
#endif

   glPointSize(taille_point);
   glMatrixMode(GL_MODELVIEW);    // Set the matrix mode to object modeling
   
   glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
   glClearDepth(0.0f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the window
}

/* *************************************************** */
/* main */
int main(int argc, char **argv)
{
   if (argc != 2) {
      EXIT_ON_ERROR("You must specified a .ppm file");
   }

   img = transform_img_to_vector(argv[1], &width, &height);
   printf("Usage:\n"
         "Press P to start learning \n"
         "Press R to reset the neurones randomly\n");


#if MODE
   load_cities();

   // INITIALIZING THE NETWORK
   map = initKohonen(50, 1, 2, phi, loopTopologicalDistance); // takes the sizeX, sizeY, size of input vector, function callback for the neighborhood function phi
   resetMap(map, 50, 750); // randomize the values of the weights
   DataSet = initialiseCitiesData(); // load the location of the cities into the training data
   printf("Press S to snap the neurones to the data points\n");
#else

   // INITIALIZING THE NETWORK
   int numCol = 256; // can be 32, 256 for more colors <<<<<<<<<<<<<<<<<<
   int networkSize = log(numCol) / log(2.0);
   map = initKohonen(networkSize, networkSize, 3, phi3, topologicalDistance); // takes the sizeX, sizeY, size of input vector, function callback for the neighborhood function phi
   resetMap(map, 0, 256); // randomize the values of the weights
   DataSet = initialiseImageData(); // load the location of the cities into the training data
   printf("Press S to save the compressed image\n");
#endif

   /* GLUT init */
   glutInit(&argc, argv);            // Initialize GLUT
   glutInitDisplayMode(GLUT_DOUBLE); // Enable double buffered mode
#if MODE
   glutInitWindowSize(width, height);   // Set the window's initial width & height
#else
   glutInitWindowSize(512, 512);   // Set the window's initial width & height
#endif
   glutCreateWindow("Kohonen");      // Create window with the name of the executable

   /* enregistrement des fonctions de rappel */
   glutDisplayFunc(affichage);
   glutKeyboardFunc(clavier);
   glutSpecialFunc(clavierSpecial);
   glutReshapeFunc(reshape);
   glutIdleFunc(idle);
   glutMouseFunc(mouse);
   glutMotionFunc(mousemotion);

   /* OpenGL 2D generic init */
   initGL(width, height);

#if MODE
   textureID = charger_texture(img);
#endif

   /* Main loop */
   glutMainLoop();

#if MODE

   if (img != NULL) {
      free(img);
      img = NULL;
   }

   /* Delete used resources and quit */
   glDeleteTextures(1, &textureID);
   freeTrainingData(&DataSet);
   freeKohonen(&map);
#endif

   return 0;
}

/* *************************************************** */

/* Helper for HSLtoRGB */
float HueToRgb(float p, float q, float t)
{
   if (t < 0.0f) {
      t += 1.0f;
   }

   if (t > 1.0f) {
      t -= 1.0f;
   }

   if (t < 1.0f / 6.0f) {
      return p + (q - p) * 6.0f * t;
   }

   if (t < 1.0f / 2.0f) {
      return q;
   }

   if (t < 2.0f / 3.0f) {
      return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
   }

   return p;
}
/* Convert HSL (Hue, Saturation, Lightness) color to RGB
 * Used to make the sliding color effect on the neurones display */
void HSLtoRGB(float H, float S, float L, float *R, float *G, float *B)
{
   if (S == 0.0f) {
      *R = *G = *B = L;
   } else {
      float q = L < 0.5f ? L * (1.0f + S) : L + S - L * S;
      float p = 2.0f * L - q;
      *R = HueToRgb(p, q, H + 1.0f / 3.0f);
      *G = HueToRgb(p, q, H);
      *B = HueToRgb(p, q, H - 1.0f / 3.0f);
   }
}

/**
 * Draw the network
 */
void drawKohonen2D(KOHONEN* m)
{
   int total_size = m->sizeX * m->sizeY;

   // draw neurones
   float H = 0;
   float S = 1, L = 0.5; // saturation 100% and Lightness 50%
   float R, G, B;

   for (int i = 0; i < total_size; i++) {
      HSLtoRGB(H, S, L, &R, &G, &B);

      glBegin(GL_POINTS);
      glColor3f(R, G, B);
      glVertex2f(m->weight[i][0], m->weight[i][1]);
      glEnd();
      glColor3f(0, 0, 0);

      H = ((float)i) / (total_size - 1); // go through all hue values from 0 to 1 (to get all the colors)
   }

   glLineWidth(2);

   // draw synapses
   for (int i = 0; i < m->sizeY; i++) {
      for (int j = 0; j < m->sizeX; j++) {
         int cur_point    = i       * m->sizeX +  j,
             bottom_point = i       * m->sizeX + (j + 1),
             right_point  = (i + 1) * m->sizeX +  j;

         // Draw a line into the right neighbor
         if(right_point < total_size) {
            glBegin(GL_LINES);
            glColor3f(0.0, 0.0, 1.0);
            glVertex2f(m->weight[cur_point][0], m->weight[cur_point][1]);
            glVertex2f(m->weight[right_point][0], m->weight[right_point][1]);
            glEnd();
         }

         // Draw the line into the bottom neighbor
         if(bottom_point < total_size) {
            glBegin(GL_LINES);
            glColor3f(0.0, 0.0, 1.0);
            glVertex2f(m->weight[cur_point][0], m->weight[cur_point][1]);
            glVertex2f(m->weight[bottom_point][0], m->weight[bottom_point][1]);
            glEnd();
         }

         glColor3f(0.0, 0.0, 0.0); // reset color
      }
   }

}

void drawKohonenRGB(KOHONEN* m)
{
   int wx = 256 / m->sizeX,
       wy = 256 / m->sizeY;

   for (int i = 0; i < m->sizeX; i++) {
      for (int j = 0; j < m->sizeY; j++) {
         int x = i * 256 / m->sizeX,
             y = j * 256 / m->sizeY;
         
         glPushMatrix();
         glTranslatef(x, y, 0.0f);
         
         glBegin(GL_QUADS);
            glColor3f(m->weight[i * m->sizeX + j][0]/256.0, 
                      m->weight[i * m->sizeX + j][1]/256.0, 
                      m->weight[i * m->sizeX + j][2]/256.0);
            glVertex2f(0 , 0 );
            glVertex2f(wx, 0 );
            glVertex2f(wx, wy);
            glVertex2f(0 , wy);
         glEnd();
         glPopMatrix();
      }
   }
}

void writeCompressed()
{
   Image* image = malloc(sizeof(Image));
   image->x = width;
   image->y = height;

   image->data = malloc(sizeof(unsigned char) * 3 * width * height);
   
   for (int p = 0; p < width * height; p++) {
      int c = 3 * p;
      
      int minval = INT_MAX;
      int argmin = -1;
      for(int i = 0; i < map->sizeX * map->sizeY; i++) {
         float dist = (img[c + 0] - map->weight[i][0]) * (img[c + 0] - map->weight[i][0])
                    + (img[c + 1] - map->weight[i][1]) * (img[c + 1] - map->weight[i][1]) 
                    + (img[c + 2] - map->weight[i][2]) * (img[c + 2] - map->weight[i][2]);
         if(argmin == -1 || minval > dist) {
            minval = dist;
            argmin = i;
         }
      }
      image->data[p].r = map->weight[argmin][0];
      image->data[p].g = map->weight[argmin][1];
      image->data[p].b = map->weight[argmin][2];
   }

   writePPM("compressed.ppm", image);
}

/* fonction d'affichage appelée a chaque refresh*/
void affichage()
{
   // Clear color and depth buffers
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glMatrixMode(GL_MODELVIEW);     // Operate on model-view matrix
   glLoadIdentity();

#if MODE
   int i ;
   /* Draw a quad */
   glColor3f(1.0, 1.0, 1.0);
   glBegin(GL_QUADS);
      glTexCoord2i(0, 0);
      glVertex2i(0,   0);
      glTexCoord2i(0, 1);
      glVertex2i(0,   height);
      glTexCoord2i(1, 1);
      glVertex2i(width, height);
      glTexCoord2i(1, 0);
      glVertex2i(width, 0);
   glEnd();

   for (i = 0; i < NB_VILLE; i++) {
      glBegin(GL_POINTS);
      glColor3f(1.0, 0.0, 0.0);
      glVertex2f(ville[i].x, ville[i].y);
      glEnd();
      glColor3f(0, 0, 0);
      draw_text(ville[i].x - 20, ville[i].y + 20, "%s", ville[i].name);
   }

   /* Draw the network */
   drawKohonen2D(map);

   glColor3f(0.0, 0.0, 0.0);
   draw_text(60, 70, "nb iter: %d", cpt);
   draw_text(60, 85, "EPSILON: %.2f", EPSILON);
#else
   drawKohonenRGB(map);

   glColor3f(0.0, 0.0, 0.0);
   draw_text(150, 20, "nb iter: %d", cpt);
   draw_text(150, 15, "EPSILON: %.2f", EPSILON);
#endif


   glFlush();
   glutSwapBuffers();
}

// VOTRE CODE DE KOHONEN ICI
void idle()
{
   if (calc) { // calc est modifié si on presse "p" (voir la fonction "clavier" ci dessous)
      cpt++; // un simple compteur

      int choice  = rand() % DataSet->numInput;
      updateKohonen(map, DataSet->input[choice], EPSILON);

      /*for(int i = 0; i < 20; i++) {
         printf("%g %g\n", map->weight[i][0], map->weight[i][1]);
      }*/


      glutPostRedisplay();
   }
}
void clavierSpecial(int touche, int x, int y)
{
   switch(touche) {
   case GLUT_KEY_UP:
      EPSILON = fmin(EPSILON + 0.02, 2);
      break;

   case GLUT_KEY_DOWN:
      EPSILON = fmax(EPSILON - 0.02, 0.01);
      break;

   case GLUT_KEY_LEFT:
   case GLUT_KEY_RIGHT:
      EPSILON = 0.1;
      break;
   }

   glutPostRedisplay();
}
void clavier(unsigned char touche, int x, int y)
{
   switch (touche) {
   case 'p':
      calc = !calc;
      break;

   case 's': // Snap the kohonen map to the data
#if MODE
      snapToData(map, DataSet);
      printf("Snapping neuones to the data\n");
#else
      if(calc) {
         printf("Please stop the learning to save, press p\n");
      } else {
         printf("Saving compressed image to ./compressed.ppm\n");
         writeCompressed(); 
      }
#endif
      break;

   case 'r':
#if MODE
      resetMap(map, 50, 750);
#else
      resetMap(map, 0, 256);
#endif
      printf("Randomizing the neurones' weights..\n");
      break;

   case 'q': /* la touche 'q' permet de quitter le programme */
      exit(0);
   } /* switch */

   glutPostRedisplay();
} /* clavier */


void reshape(GLsizei newwidth, GLsizei newheight)
{
   // On ecrase pas width et height dans le cas image car il s'agira de la taille de l'image
#if MODE
   width = newwidth;
   height = newheight;
#else
#endif
   // Set the viewport to cover the new window
   glViewport(0, 0, newwidth, newheight );
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

#if MODE
   glOrtho(0.0, width, height, 0.0, -1, 1);
#else
   glOrtho(0.0, 256, 256, 0.0, -10, 10);
#endif

   glMatrixMode(GL_MODELVIEW);

   glutPostRedisplay();
}


/* getion des boutons de  la souris*/
void mouse(int bouton, int etat, int x, int y)
{
   /* si on appuie sur la bouton de gauche */
   if (bouton == GLUT_LEFT_BUTTON && etat == GLUT_DOWN) {
      presse = 1; // vrai
      xold = x; // sauvegarde de la position de la souris
      yold = y;
   }

   /* si on relache la souris */
   if (bouton == GLUT_LEFT_BUTTON && etat == GLUT_UP) {
      presse = 0; // faux
   }
} /* mouse */



/*gestion des mouvements de la souris */
void mousemotion(int x, int y)
{
   if (presse) { /* si le bouton gauche est presse */
      /* on mofifie les angles de rotation de l'objet en fonction de la position actuelle de la souris et de la derniere position sauvegard?e */
      anglex = anglex + (x - xold);
      angley = angley + (y - yold);
      glutPostRedisplay();
   }

   xold = x; /* sauvegarde des valeurs courante des positions de la souris */
   yold = y;
} /* mousemotion */
