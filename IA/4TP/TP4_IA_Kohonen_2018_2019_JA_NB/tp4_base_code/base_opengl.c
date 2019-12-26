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

#include "base_opengl.h"
#include "ppm.h"
#include "kohonen.h"

#define DEFAULT_WIDTH  500
#define DEFAULT_HEIGHT 500
#define NB_VILLE 21
#define MODE 1

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

KOHONEN* map;
TRAINING_DATA* DataSet;

TRAINING_DATA* initialiseSet1()
{
   TRAINING_DATA* DataSet = initTrainingData(20, 2);

   for(int i = 0; i < 20; i++) {
      DataSet->input[i][0] = rand() % 200;
      DataSet->input[i][1] = rand() % 200;
   }

   return DataSet;
}

void resetMap(KOHONEN* m)
{
   int minval = 50;
   int maxval = 750;
   for (int i = 0; i < m->sizeX * m->sizeY; i++) {
      m->weight[i][0] = rand() % (minval + maxval) - minval;
      m->weight[i][1] = rand() % (minval + maxval) - minval;
   }
}

void snapToData(KOHONEN* m, TRAINING_DATA* data) 
{
   for (int i = 0; i < m->sizeX * m->sizeY; i++) {
      int closestArg = -1;
      float closest = 0;
      for(int j = 0; j < data->numInput; j++) {
         float dist = euclidianDistance(data->input[j], m->weight[i], data->sizeInput);
         if(dist < closest || closestArg == -1) {
            closestArg = j; closest = dist;
         }
      }

      memcpy(m->weight[i], data->input[closestArg], sizeof(float) * data->sizeInput);
   }
}

TRAINING_DATA* initialiseSet2()
{
   TRAINING_DATA* DataSet = initTrainingData(NB_VILLE, 2);

   for(int i = 0; i < NB_VILLE; i++) {
      DataSet->input[i][0] = ville[i].x;
      DataSet->input[i][1] = ville[i].y;
   }

   return DataSet;
}

float phi1(float x)
{
   float lambda = 0.5;
   float beta = 0.1;

   if(x < 1) {
      return 1;
   } else if(x < 2) {
      return 0.4;
   } else if(x < 3) {
      return 0.2;
   } else if(x < 4) {
      return 0.05;
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
#else
   int taille_point = 5;
#endif

   glViewport(0, 0, w, h); // use a screen size of WIDTH x HEIGHT
   glEnable(GL_TEXTURE_2D);     // Enable 2D texturing

   glMatrixMode(GL_PROJECTION);     // Make a simple 2D projection on the entire window
   glLoadIdentity();

#if MODE
   glOrtho(0.0, w, h, 0.0, -1, 1);
#else
   glOrtho(0.0, 200, 200, 0.0, -1, 1);
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

#if MODE
   unsigned char * data = NULL;

   if (argc != 2) {
      EXIT_ON_ERROR("You must specified a .ppm file");
   }

   data = transform_img_to_vector(argv[1], &width, &height);
   load_cities();

   map = initKohonen(50, 1, 2, phi1);
   resetMap(map);
   DataSet = initialiseSet2();
#endif

   /* GLUT init */
   glutInit(&argc, argv);            // Initialize GLUT
   glutInitDisplayMode(GLUT_DOUBLE); // Enable double buffered mode
   glutInitWindowSize(width, height);   // Set the window's initial width & height
   glutCreateWindow("Kohonen");      // Create window with the name of the executable

   /* enregistrement des fonctions de rappel */
   glutDisplayFunc(affichage);
   glutKeyboardFunc(clavier);
   glutReshapeFunc(reshape);
   glutIdleFunc(idle);
   glutMouseFunc(mouse);
   glutMotionFunc(mousemotion);

   /* OpenGL 2D generic init */
   initGL(width, height);

#if MODE
   textureID = charger_texture(data);

   if (data != NULL) {
      free(data);
      data = NULL;
   }

#endif

   /* Main loop */
   glutMainLoop();

#if MODE
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
    if (t < 0.0f) t += 1.0f;
    if (t > 1.0f) t -= 1.0f;
    if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
    if (t < 1.0f / 2.0f) return q;
    if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
    return p;
}
/* Convert HSL color to RGB */
void HSLtoRGB(float H, float S, float L, float *R, float *G, float *B)
{
    if (S == 0.0f)
        *R = *G = *B = L;
    else
    {
        float q = L < 0.5f ? L * (1.0f + S) : L + S - L * S;
        float p = 2.0f * L - q;
        *R = HueToRgb(p, q, H + 1.0f / 3.0f);
        *G = HueToRgb(p, q, H);
        *B = HueToRgb(p, q, H - 1.0f / 3.0f);
    }
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

   int total_size = map->sizeX * map->sizeY;

   // draw neurones
   float H = 0;
   float S = 1, L = 0.5;
   float R, G, B;
   for (i = 0; i < total_size; i++) {
      HSLtoRGB(H,S,L, &R,&G,&B);
      
      glBegin(GL_POINTS);
      glColor3f(R, G, B);
      glVertex2f(map->weight[i][0], map->weight[i][1]);
      glEnd();
      glColor3f(0, 0, 0);

      H = ((float)i) / (total_size-1);
   }
   
   // draw synapses
   for (i = 0; i < map->sizeY; i++) {
      for (int j = 0; j < map->sizeX; j++) {
         int cur_point    = i     * map->sizeX + j,
             bottom_point = i     * map->sizeX + (j + 1),
             right_point  = (i + 1) * map->sizeX + j;
         
         
         if(right_point < total_size) {
            glBegin(GL_LINES);
            glColor3f(0.0, 0.0, 1.0);
            glVertex2f(map->weight[cur_point][0], map->weight[cur_point][1]);
            glVertex2f(map->weight[right_point][0], map->weight[right_point][1]);
            glEnd();
         }

         if(bottom_point < total_size) {
            glBegin(GL_LINES);
            glColor3f(0.0, 0.0, 1.0);
            glVertex2f(map->weight[cur_point][0], map->weight[cur_point][1]);
            glVertex2f(map->weight[bottom_point][0], map->weight[bottom_point][1]);
            glEnd();
         }

         glColor3f(0.0, 0.0, 0.0);
      }
   }

// glColor3f(1.0, 1.0, 1.0);

   draw_text(60, 70, "nb iter: %d", cpt);
#else

// VOTRE CODE D'AFFICHAGE ICI, voir l'exemple ci-dessous

// ceci est un exemple pour tracer des points et des lignes
   glBegin(GL_POINTS);
   glColor3f(1.0, 0.0, 0.0);
   glVertex2f(50, 25);
   glEnd();

   glBegin(GL_POINTS);
   glColor3f(1.0, 0.0, 0.0);
   glVertex2f(75, 10);
   glEnd();

   glBegin(GL_LINE_LOOP);
   glColor3f(1.0, 0.0, 0.0);
   glVertex2f(50, 25);
   glVertex2f(75, 10);
   glEnd();

   glColor3f(1.0, 1.0, 1.0);
   draw_text(150, 20, "nb iter: %d", cpt);

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
      updateKohonen(map, DataSet->input[choice]);

      /*for(int i = 0; i < 20; i++) {
         printf("%g %g\n", map->weight[i][0], map->weight[i][1]);
      }*/


      glutPostRedisplay();
   }
}


void clavier(unsigned char touche, int x, int y)
{
   switch (touche) {
   case 'p':
      calc = !calc;
      break;
   case 's': // Snap the kohonen map to the data
      snapToData(map, DataSet);
      glutPostRedisplay();
      break;
   case 'r':
      resetMap(map);
      glutPostRedisplay();
      break;
   case 'q': /* la touche 'q' permet de quitter le programme */
      exit(0);
   } /* switch */

} /* clavier */


void reshape(GLsizei newwidth, GLsizei newheight)
{
   // On ecrase pas width et height dans le cas image car il s'agira de la taille de l'image
#if MODE
#else
   width = newwidth;
   height = newheight;
#endif
   // Set the viewport to cover the new window
   glViewport(0, 0, newwidth, newheight );
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

#if MODE
   glOrtho(0.0, width, height, 0.0, -1, 1);
#else
   glOrtho(0.0, 200, 200, 0.0, -1, 1);
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
