#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NB_LIGNES 10
#define NB_COLONNES 10

// changed this to accomodate largest f_eval
#define INFINI 1000000

#define PROF_MAX 4

#define ALPHA_BETA 1
/*#define DEBUG*/

typedef struct pion_s {
   int couleur;
   int valeur;
} Pion;

Pion *plateauDeJeu;

void f_affiche_plateau(Pion *plateau);
int f_convert_char2int(char c);
char f_convert_int2char(int i);



int f_convert_char2int(char c)
{
#ifdef DEBUG
   printf("dbg: entering %s %d\n", __FUNCTION__, __LINE__);
#endif

   if(c >= 'A' && c <= 'Z') {
      return (int)(c - 'A');
   }

   if(c >= 'a' && c <= 'z') {
      return (int)(c - 'a');
   }

   return -1;
}

char f_convert_int2char(int i)
{
#ifdef DEBUG
   printf("dbg: entering %s %d\n", __FUNCTION__, __LINE__);
#endif

   return (char)i + 'A';
}

Pion *f_init_plateau()
{
   int i, j;
   Pion *plateau = NULL;


#ifdef DEBUG
   printf("dbg: entering %s %d\n", __FUNCTION__, __LINE__);
#endif

   plateau = (Pion *)malloc(NB_LIGNES * NB_COLONNES * sizeof(Pion));

   if(plateau == NULL) {
      printf("error: unable to allocate memory\n");
      exit(EXIT_FAILURE);
   }

   for(i = 0; i < NB_LIGNES; i++) {
      for(j = 0; j < NB_COLONNES; j++) {
         plateau[i * NB_COLONNES + j].couleur = 0;
         plateau[i * NB_COLONNES + j].valeur = 0;
      }
   }

   plateau[9 * NB_COLONNES + 5].couleur = 1;
   plateau[9 * NB_COLONNES + 5].valeur = 1;

   plateau[9 * NB_COLONNES + 6].couleur = 1;
   plateau[9 * NB_COLONNES + 6].valeur = 2;

   plateau[9 * NB_COLONNES + 7].couleur = 1;
   plateau[9 * NB_COLONNES + 7].valeur = 3;

   plateau[9 * NB_COLONNES + 8].couleur = 1;
   plateau[9 * NB_COLONNES + 8].valeur = 2;

   plateau[9 * NB_COLONNES + 9].couleur = 1;
   plateau[9 * NB_COLONNES + 9].valeur = 1;

   plateau[8 * NB_COLONNES + 0].couleur = 1;
   plateau[8 * NB_COLONNES + 0].valeur = 1;

   plateau[8 * NB_COLONNES + 1].couleur = 1;
   plateau[8 * NB_COLONNES + 1].valeur = 3;

   plateau[8 * NB_COLONNES + 2].couleur = 1;
   plateau[8 * NB_COLONNES + 2].valeur = 3;

   plateau[8 * NB_COLONNES + 3].couleur = 1;
   plateau[8 * NB_COLONNES + 3].valeur = 1;

   plateau[8 * NB_COLONNES + 6].couleur = 1;
   plateau[8 * NB_COLONNES + 6].valeur = 1;

   plateau[8 * NB_COLONNES + 7].couleur = 1;
   plateau[8 * NB_COLONNES + 7].valeur = 1;

   plateau[8 * NB_COLONNES + 8].couleur = 1;
   plateau[8 * NB_COLONNES + 8].valeur = 1;

   plateau[7 * NB_COLONNES + 1].couleur = 1;
   plateau[7 * NB_COLONNES + 1].valeur = 1;

   plateau[7 * NB_COLONNES + 2].couleur = 1;
   plateau[7 * NB_COLONNES + 2].valeur = 1;

   plateau[2 * NB_COLONNES + 7].couleur = -1;
   plateau[2 * NB_COLONNES + 7].valeur = 1;

   plateau[2 * NB_COLONNES + 8].couleur = -1;
   plateau[2 * NB_COLONNES + 8].valeur = 1;

   plateau[1 * NB_COLONNES + 1].couleur = -1;
   plateau[1 * NB_COLONNES + 1].valeur = 1;

   plateau[1 * NB_COLONNES + 2].couleur = -1;
   plateau[1 * NB_COLONNES + 2].valeur = 1;

   plateau[1 * NB_COLONNES + 3].couleur = -1;
   plateau[1 * NB_COLONNES + 3].valeur = 1;

   plateau[1 * NB_COLONNES + 6].couleur = -1;
   plateau[1 * NB_COLONNES + 6].valeur = 1;

   plateau[1 * NB_COLONNES + 7].couleur = -1;
   plateau[1 * NB_COLONNES + 7].valeur = 3;

   plateau[1 * NB_COLONNES + 8].couleur = -1;
   plateau[1 * NB_COLONNES + 8].valeur = 3;

   plateau[1 * NB_COLONNES + 9].couleur = -1;
   plateau[1 * NB_COLONNES + 9].valeur = 1;

   plateau[0 * NB_COLONNES + 0].couleur = -1;
   plateau[0 * NB_COLONNES + 0].valeur = 1;

   plateau[0 * NB_COLONNES + 1].couleur = -1;
   plateau[0 * NB_COLONNES + 1].valeur = 2;

   plateau[0 * NB_COLONNES + 2].couleur = -1;
   plateau[0 * NB_COLONNES + 2].valeur = 3;

   plateau[0 * NB_COLONNES + 3].couleur = -1;
   plateau[0 * NB_COLONNES + 3].valeur = 2;

   plateau[0 * NB_COLONNES + 4].couleur = -1;
   plateau[0 * NB_COLONNES + 4].valeur = 1;

#ifdef DEBUG
   printf("dbg: exiting %s %d\n", __FUNCTION__, __LINE__);
#endif

   return plateau;
}

void f_affiche_plateau(Pion *plateau)
{
   int i, j, k;


#ifdef DEBUG
   printf("dbg: entering %s %d\n", __FUNCTION__, __LINE__);
#endif

   printf("\n    ");

   for(k = 0; k < NB_COLONNES; k++) {
      printf("%2c ", f_convert_int2char(k));
   }

   printf("\n    ");

   for(k = 0; k < NB_COLONNES; k++) {
      printf("-- ");
   }

   printf("\n");

   for(i = NB_LIGNES - 1; i >= 0; i--) {
      printf("%2d ", i);

      for(j = 0; j < NB_COLONNES; j++) {
         printf("|");

         switch(plateau[i * NB_COLONNES + j].couleur) {
         case -1:
            printf("%do", plateau[i * NB_COLONNES + j].valeur);
            break;

         case 1:
            printf("%dx", plateau[i * NB_COLONNES + j].valeur);
            break;

         default:
            printf("  ");
         }
      }

      printf("|\n    ");

      for(k = 0; k < NB_COLONNES; k++) {
         printf("-- ");
      }

      printf("\n");
   }

   printf("    ");

#ifdef DEBUG
   printf("dbg: exiting %s %d\n", __FUNCTION__, __LINE__);
#endif
}

// **** added function **** same as f_gagnant but for any board (not just
// plateau de jeu)
int f_gagnant_test(Pion* jeu)
{
   int i, j, somme1 = 0, somme2 = 0;


#ifdef DEBUG
   printf("dbg: entering %s %d\n", __FUNCTION__, __LINE__);
#endif

   //Quelqu'un est-il arrive sur la ligne de l'autre
   for(i = 0; i < NB_COLONNES; i++) {
      if(jeu[i].couleur == 1) {
         return 1;
      }

      if(jeu[(NB_LIGNES - 1)*NB_COLONNES + i].couleur == -1) {
         return -1;
      }
   }

   //taille des armees
   for(i = 0; i < NB_LIGNES; i++) {
      for(j = 0; j < NB_COLONNES; j++) {
         if(jeu[i * NB_COLONNES + j].couleur == 1) {
            somme1++;
         }

         if(jeu[i * NB_COLONNES + j].couleur == -1) {
            somme2++;
         }
      }
   }

   if(somme1 == 0) {
      return -1;
   }

   if(somme2 == 0) {
      return 1;
   }

#ifdef DEBUG
   printf("dbg: exiting %s %d\n", __FUNCTION__, __LINE__);
#endif
   return 0;
}
int f_gagnant()
{
   int i, j, somme1 = 0, somme2 = 0;


#ifdef DEBUG
   printf("dbg: entering %s %d\n", __FUNCTION__, __LINE__);
#endif

   //Quelqu'un est-il arrive sur la ligne de l'autre
   for(i = 0; i < NB_COLONNES; i++) {
      if(plateauDeJeu[i].couleur == 1) {
         return 1;
      }

      if(plateauDeJeu[(NB_LIGNES - 1)*NB_COLONNES + i].couleur == -1) {
         return -1;
      }
   }

   //taille des armees
   for(i = 0; i < NB_LIGNES; i++) {
      for(j = 0; j < NB_COLONNES; j++) {
         if(plateauDeJeu[i * NB_COLONNES + j].couleur == 1) {
            somme1++;
         }

         if(plateauDeJeu[i * NB_COLONNES + j].couleur == -1) {
            somme2++;
         }
      }
   }

   if(somme1 == 0) {
      return -1;
   }

   if(somme2 == 0) {
      return 1;
   }

#ifdef DEBUG
   printf("dbg: exiting %s %d\n", __FUNCTION__, __LINE__);
#endif
   return 0;
}


/**
 * Prend comme argument la ligne et la colonne de la case
 * 	pour laquelle la bataille a lieu
 * Renvoie le couleur du gagnant
 * */
int f_bataille(int l, int c)
{
   int i, j, mini, maxi, minj, maxj;
   int somme = 0;

#ifdef DEBUG
   printf("dbg: entering %s %d\n", __FUNCTION__, __LINE__);
#endif
   mini = l - 1 < 0 ? 0 : l - 1;
   maxi = l + 1 > NB_LIGNES - 1 ? NB_LIGNES - 1 : l + 1;
   minj = c - 1 < 0 ? 0 : c - 1;
   maxj = c + 1 > NB_COLONNES - 1 ? NB_COLONNES - 1 : c + 1;

   for(i = mini; i <= maxi; i++) {
      for(j = minj; j <= maxj; j++) {
         somme += plateauDeJeu[i * NB_COLONNES + j].couleur * plateauDeJeu[i * NB_COLONNES + j].valeur;
      }
   }

   somme -= plateauDeJeu[l * NB_COLONNES + c].couleur * plateauDeJeu[l * NB_COLONNES + c].valeur;

#ifdef DEBUG
   printf("dbg: exiting %s %d\n", __FUNCTION__, __LINE__);
#endif

   if(somme < 0) {
      return -1;
   }

   if(somme > 0) {
      return 1;
   }

   return plateauDeJeu[l * NB_COLONNES + c].couleur;
}


/**
 * Prend la ligne et colonne de la case d'origine
 * 	et la ligne et colonne de la case de destination
 * Renvoie 1 en cas d'erreur
 * Renvoie 0 sinon
 * */
int f_test_mouvement(Pion *plateau, int l1, int c1, int l2, int c2, int couleur)
{
#ifdef DEBUG
   printf("dbg: entering %s %d\n", __FUNCTION__, __LINE__);
   printf("de (%d,%d) vers (%d,%d)\n", l1, c1, l2, c2);
#endif

   /* Erreur, hors du plateau */
   if(l1 < 0 || l1 >= NB_LIGNES || l2 < 0 || l2 >= NB_LIGNES ||
         c1 < 0 || c1 >= NB_COLONNES || c2 < 0 || c2 >= NB_COLONNES) {
      return 1;
   }

   /* Erreur, il n'y a pas de pion a deplacer ou le pion n'appartient pas au joueur*/
   if(plateau[l1 * NB_COLONNES + c1].valeur == 0 || plateau[l1 * NB_COLONNES + c1].couleur != couleur) {
      return 1;
   }

   /* Erreur, tentative de tir fratricide */
   if(plateau[l2 * NB_COLONNES + c2].couleur == plateau[l1 * NB_COLONNES + c1].couleur) {
      return 1;
   }

   if(l1 - l2 > 1 || l2 - l1 > 1 || c1 - c2 > 1 || c2 - c1 > 1 || (l1 == l2 && c1 == c2)) {
      return 1;
   }

#ifdef DEBUG
   printf("dbg: exiting %s %d\n", __FUNCTION__, __LINE__);
#endif
   return 0;
}


/**
 * Prend la ligne et colonne de la case d'origine
 * 	et la ligne et colonne de la case de destination
 *  et effectue le trantement de l'operation demandée
 * Renvoie 1 en cas d'erreur
 * Renvoie 0 sinon
 * */
int f_bouge_piece(Pion *plateau, int l1, int c1, int l2, int c2, int couleur)
{
   int gagnant = 0;


#ifdef DEBUG
   printf("dbg: entering %s %d\n", __FUNCTION__, __LINE__);
#endif

   if(f_test_mouvement(plateau, l1, c1, l2, c2, couleur) != 0) {
      return 1;
   }


   /* Cas ou il n'y a personne a l'arrivee */
   if(plateau[l2 * NB_COLONNES + c2].valeur == 0) {
      plateau[l2 * NB_COLONNES + c2].couleur = plateau[l1 * NB_COLONNES + c1].couleur;
      plateau[l2 * NB_COLONNES + c2].valeur = plateau[l1 * NB_COLONNES + c1].valeur;
      plateau[l1 * NB_COLONNES + c1].couleur = 0;
      plateau[l1 * NB_COLONNES + c1].valeur = 0;
   } else {
      gagnant = f_bataille(l2, c2);

      /* victoire */
      if(gagnant == couleur) {
         plateau[l2 * NB_COLONNES + c2].couleur = plateau[l1 * NB_COLONNES + c1].couleur;
         plateau[l2 * NB_COLONNES + c2].valeur = plateau[l1 * NB_COLONNES + c1].valeur;
         plateau[l1 * NB_COLONNES + c1].couleur = 0;
         plateau[l1 * NB_COLONNES + c1].valeur = 0;
      }
      /* defaite */
      else if(gagnant != 0) {
         plateau[l1 * NB_COLONNES + c1].couleur = 0;
         plateau[l1 * NB_COLONNES + c1].valeur = 0;
      }
   }

#ifdef DEBUG
   printf("dbg: exiting %s %d\n", __FUNCTION__, __LINE__);
#endif
   return 0;
}

//Calcul du nombre de pions sur le plateau du joueur
int f_nbPions(Pion* jeu, int joueur)
{
   int nbPion = 0;
   int i, j;

   for (i = 0; i < NB_COLONNES; ++i) {
      for (j = 0; j < NB_LIGNES; ++j) {
         if (jeu[i * NB_COLONNES + j].couleur == joueur) {
            ++nbPion;
         }
      }
   }

   return nbPion;
}

//Calcul de la valeur de tous les pions du joueur
int f_valeur(Pion* jeu, int joueur)
{
   int i, j;
   int valeur = 0;

   for (i = 0; i < NB_COLONNES; ++i) {
      for (j = 0; j < NB_LIGNES; ++j) {
         if (jeu[i * NB_COLONNES + j].couleur == joueur) {
            valeur += jeu[i * NB_COLONNES + j].valeur;
         }
      }
   }

   return valeur;
}

//fonction d'évaluation
int f_eval(Pion* jeu, int joueur)
{
   int dist_diff = 0; // sum of players distances from the opposite player's goal line

   for (int i = 0; i < NB_LIGNES; i++) {
      for (int j = 0; j < NB_COLONNES; j++) {
         int col = jeu[i * NB_COLONNES + j].couleur;

         // ignore empty cells
         if(col != 0) {
            int goal = (col == -1) ? 9 : 0; // goal line that the pawns must seek

            // distance is reversed because it is inversly correlated with evaluation
            // ie: if the distance is big the evaluation should be small etc..
            int inv_dist = (9 - abs(goal - i));
            int is_player = (col == joueur) ? 1 : -1;

            dist_diff += is_player * inv_dist;
         }
      }
   }

   int val_diff = f_valeur(jeu, joueur) - f_valeur(jeu, -joueur); // player pawn values difference
   int mult_factor = 5; // this value should represent how much more important the value
   // difference is from the distance difference

   // to randomize played games
   // expected to either add or substract 1 (uniformly) from the evaluation every (rand_step/2) calls
   int rand_step = 50;
   int rand_val = rand();
   int rand_add = (rand_val % rand_step == 0) ? 1 : ((rand_val % rand_step == 1) ? -1 : 0);

   return val_diff * mult_factor + dist_diff;// + rand_add;
}

//copie du plateau
void f_copie_plateau(Pion* source, Pion* destination)
{
   int i, j;

   for (i = 0; i < NB_LIGNES; i++) {
      for (j = 0; j < NB_COLONNES; j++) {
         destination[i * NB_COLONNES + j].couleur = source[i * NB_COLONNES + j].couleur;
         destination[i * NB_COLONNES + j].valeur = source[i * NB_COLONNES + j].valeur;
      }
   }
}

//mise a zero du plateau
Pion* f_raz_plateau()
{
   Pion* jeu = NULL;
   int i, j;
   jeu = (Pion *) malloc(NB_LIGNES * NB_COLONNES * sizeof (Pion));

   for (i = 0; i < NB_LIGNES; i++) {
      for (j = 0; j < NB_COLONNES; j++) {
         jeu[i * NB_COLONNES + j].couleur = 0;
         jeu[i * NB_COLONNES + j].valeur = 0;
      }
   }

   return jeu;
}

// global variable to store the ai move
struct move {
   struct {
      int x, y;
   } from;
   struct {
      int x, y;
   } to;
} move;

// to store the collected stats
struct {
   long total_visited_nodes; // number of all node visits from all calls
   int num_calls; // number of all ai calls
   double total_elapsed_time; // in seconds (only execution of f_negamax or f_negamax_ab is counted)
} stats;

void f_affiche_stats()
{
   printf("****** STATISTIQUES ******\n");
   printf("WITH ALPHA BETA = %s\n", (ALPHA_BETA) ? "yes" : "no");
   printf("MAX DEPTH = %d\n", PROF_MAX);
   printf("NUM OF AI CALLS = %d function calls\n", stats.num_calls);
   printf("AVERAGE NODE VISITS = %g nodes\n", (float)stats.total_visited_nodes / stats.num_calls);
   printf("AVERAGE ELPASED TIME = %gms\n", 1000 * stats.total_elapsed_time);
   printf("**************************\n");
}

// algo negamax sans alpha beta
int f_negamax(Pion* plateau_courant, int profondeur, int joueur)
{
   // update stats
   stats.total_visited_nodes++;

   int winner = f_gagnant_test(plateau_courant);

   if(winner) {
      return joueur * winner * INFINI;
   }

   if(profondeur <= 0) {
      return f_eval(plateau_courant, joueur);
   }

   int maxval = -INFINI;

   Pion* plateau_suivant = f_raz_plateau();
   f_copie_plateau(plateau_courant, plateau_suivant);

   for (int i = 0; i < NB_LIGNES; i++) {
      for (int j = 0; j < NB_COLONNES; j++) {

         // move either the player or the opponent's pawns depending on
         // current color
         if(plateau_courant[i * NB_COLONNES + j].couleur == joueur)

            for(int x = -1; x <= 1; x++) {
               for(int y = -1; y <= 1; y++) {
                  int voisinX = i + x, voisinY = j + y;

                  // try moving the pawn
                  if(f_bouge_piece(plateau_suivant, i, j, voisinX, voisinY, joueur) == 0) {
                     // TODO  move win check here

                     int newval = -f_negamax(plateau_suivant, profondeur - 1, -joueur);

                     // update the maximum value
                     if(newval >= maxval) {
                        maxval = newval;

                        // set the move
                        if(profondeur == PROF_MAX) {
                           move.from.x = i,     move.from.y = j;
                           move.to.x = voisinX, move.to.y = voisinY;
                        }
                     }

                     // undo the move:
                     // put the origin and destination cell pieces back
                     // (in case there was an attack; ie: neighboring
                     // cell had an enemy pawn)
                     /*
                     plateau_suivant[i       * NB_COLONNES + j      ] = plateau_courant[i      * NB_COLONNES + j      ];
                     plateau_suivant[voisinX * NB_COLONNES + voisinY] = plateau_courant[voisinX * NB_COLONNES + voisinY];
                     */
                     // memcpy(&plateau_suivant[i * NB_COLONNES + j],             &plateau_courant[i * NB_COLONNES + j], sizeof(Pion));
                     // memcpy(&plateau_suivant[voisinX * NB_COLONNES + voisinY], &plateau_courant[voisinX * NB_COLONNES + voisinY], sizeof(Pion));
                     f_bouge_piece(plateau_suivant, voisinX, voisinY, i, j, joueur);
                  }

               }
            }

      }
   }

   free(plateau_suivant);
   return maxval;
}

// algo negamax avec alpha beta maximize toujours pour le joueur donné
int f_negamax_ab(Pion* plateau_courant, int profondeur, int joueur, int alpha, int beta)
{
   // update stats
   stats.total_visited_nodes++;

   // win check
   /*int winner = f_gagnant_test(plateau_courant);

   if(winner) {
      return joueur * winner * INFINI;
   }*/

   if(profondeur <= 0) {
      return f_eval(plateau_courant, joueur);
   }

   int maxval = -INFINI;

   Pion* plateau_suivant = f_raz_plateau();
   f_copie_plateau(plateau_courant, plateau_suivant);

   for (int i = 0; i < NB_LIGNES; i++) {
      for (int j = 0; j < NB_COLONNES; j++) {

         // only move the player's pawns
         if(plateau_courant[i * NB_COLONNES + j].couleur == joueur) {

            for(int x = -1; x <= 1; x++) {
               for(int y = -1; y <= 1; y++) {
                  int voisinX = i + x, voisinY = j + y;

                  // try moving the pawn
                  if(f_bouge_piece(plateau_suivant, i, j, voisinX, voisinY, joueur) == 0) {
                     // TODO  move win check here

                     int newval = -f_negamax_ab(plateau_suivant, profondeur - 1, -joueur, -beta, -alpha); // note that the alpha and beta are reversed

                     // update the maximum value
                     if(newval >= maxval) {
                        maxval = newval;

                        // set the move
                        if(profondeur == PROF_MAX) {
                           move.from.x = i, move.from.y = j      ;
                           move.to.x   = voisinX, move.to.y   = voisinY;
                        }
                     }

                     // trim the alpha value
                     if(alpha < maxval) {
                        alpha = maxval;
                     }

                     // cut off if the [alpha, beta] range is empty
                     if(alpha >= beta) {
                        goto CLEAN_EXIT;
                     }

                     // undo the move:
                     // put the origin and destination cell piece back
                     // (in case there was an attack; ie: neighboring
                     // cell had an enemy pawn)

                     // plateau_suivant[i *      NB_COLONNES + j      ] = plateau_courant[i      * NB_COLONNES + j      ];
                     // plateau_suivant[voisinX * NB_COLONNES + voisinY] = plateau_courant[voisinX * NB_COLONNES + voisinY];

                     // plateau_suivant[voisinX * NB_COLONNES + voisinY] = plateau_courant[voisinX * NB_COLONNES + voisinY];

                     f_bouge_piece(plateau_suivant, voisinX, voisinY, i, j, joueur);
                     // memcpy(&plateau_suivant[i*NB_COLONNES + j],             &plateau_courant[i*NB_COLONNES + j], sizeof(Pion));
                     // plateau_suivant[voisinX * NB_COLONNES + voisinY].couleur = 0;
                     // plateau_suivant[voisinX * NB_COLONNES + voisinY].valeur = 0;

                     // memcpy(&plateau_suivant[voisinX*NB_COLONNES + voisinY], &plateau_courant[voisinX*NB_COLONNES + voisinY], sizeof(Pion));

                     // f_copie_plateau(plateau_courant, plateau_suivant);
                  }

               }
            }

         }

      }
   }

// to perform clean exits when cutting branches off
CLEAN_EXIT:
   free(plateau_suivant);
   return maxval;
}

/**
 * Calcule et joue le meilleur cout
 * */
void f_IA(int joueur)
{
#ifdef DEBUG
   printf("dbg: entering %s %d\n", __FUNCTION__, __LINE__);
#endif

   int eval; // returned value for minimax evaluation

   // benchmarking
   clock_t strt, end;
   strt = clock();

#if ALPHA_BETA==1
   eval = f_negamax_ab(plateauDeJeu, PROF_MAX, joueur, -INFINI, INFINI);
#else
   eval = f_negamax(plateauDeJeu, PROF_MAX, joueur);
#endif

   end = clock();

   //update stats
   stats.num_calls ++;
   stats.total_elapsed_time += ((double) end - strt) / CLOCKS_PER_SEC;

   f_bouge_piece(plateauDeJeu, move.from.x, move.from.y, move.to.x, move.to.y, joueur);

   printf("\n IA move for %c with eval %d: %d%c%d%c\n", (joueur == 1) ? 'x' : 'o', eval,
          move.from.x,      f_convert_int2char(move.from.y),
          move.to.x,        f_convert_int2char(move.to.y));


#ifdef DEBUG
   printf("dbg: exiting %s %d\n", __FUNCTION__, __LINE__);
#endif
}


/**
 * Demande le choix du joueur humain et calcule le coup demande
 * */
void f_humain(int joueur)
{
   char c1, c2;
   char buffer[32];
   int l1, l2;


#ifdef DEBUG
   printf("dbg: entering %s %d\n", __FUNCTION__, __LINE__);
#endif

   printf("joueur ");

   switch(joueur) {
   case -1:
      printf("o ");
      break;

   case 1:
      printf("x ");
      break;

   default:
      printf("inconnu ");
   }

   printf("joue:\n");

   while(1) {
      fgets(buffer, 32, stdin);

      if(sscanf(buffer, "%c%i%c%i\n", &c1, &l1, &c2, &l2) == 4) {
         if(f_bouge_piece(plateauDeJeu, l1, f_convert_char2int(c1), l2, f_convert_char2int(c2), joueur) == 0) {
            break;
         }
      }

      fflush(stdin);
      printf("mauvais choix\n");
   }

#ifdef DEBUG
   printf("dbg: exiting %s %d\n", __FUNCTION__, __LINE__);
#endif
}

int main(int argv, char *argc[])
{
   // random seed
   srand(time(0));

   int fin = 0, mode = 0, ret, joueur = 1;
   printf("1 humain vs IA\n2 humain vs humain\n3 IA vs IA\n");
   scanf("%d", &mode);

   plateauDeJeu = f_init_plateau();

   while (!fin) {
      f_affiche_plateau(plateauDeJeu);

      if(mode == 1) {
         if(joueur > 0) {
            f_humain(joueur);
         } else {
            f_IA(joueur);
         }
      } else if(mode == 2) {
         f_humain(joueur);
      } else {
         f_IA(joueur);
      }

      if ((ret = f_gagnant()) != 0) {
         switch (ret) {
         case 1:
            f_affiche_plateau(plateauDeJeu);
            printf("joueur x gagne!\n");
            fin = 1;
            break;

         case -1:
            f_affiche_plateau(plateauDeJeu);
            printf("joueur o gagne!\n");
            fin = 1;
            break;
         }
      }

      joueur = -joueur;
   }

   // write statistics
   f_affiche_stats();

#ifdef DEBUG
   printf("dbg: exiting %s %d\n", __FUNCTION__, __LINE__);
#endif

   return 0;
}
