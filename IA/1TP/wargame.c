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
   int val_diff = 0;

   for (int i = 0; i < NB_LIGNES; i++) {
      for (int j = 0; j < NB_COLONNES; j++) {
         int col = jeu[i * NB_COLONNES + j].couleur;
         int val = jeu[i * NB_COLONNES + j].valeur;

         // ignore empty cells
         if(col != 0) {
            int goal = (col == -1) ? 9 : 0; // goal line that the pawns must seek

            // distance is reversed because it is inversly correlated with evaluation
            // ie: if the distance is big the evaluation should be small and if
            // the distance is small the evaluation should be big
            int inv_dist = (9 - abs(goal - i));
            int is_player = (col == joueur) ? 1 : -1;

            dist_diff += is_player * inv_dist;

            // difference of the sum of the players pawn values and the sum of the
            // opponents pawn values
            val_diff += is_player * val;
         }
      }
   }

   int mult_factor = 75; // this value should represent how much more important the value
   // difference is from the distance difference

   // to randomize played games
   // expected to either add or substract 1 (uniformly) from the evaluation every (rand_step) calls
   int rand_step = 3;
   int rand_add = ((rand() % 2) ? 1 : -1) * ((rand() % rand_step == 0) ? 1 : 0);

   return val_diff * mult_factor + dist_diff + rand_add;
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
int fromX, fromY;
int toX, toY;

// to store the collected stats
struct {
   long num_tested_moves; // number of all node visits from all calls
   int num_AI_calls; // number of all f_IA calls
   double total_elapsed_time; // in seconds (only execution of f_negamax or f_negamax_ab is counted)
   int num_searched_nodes;
} stats;

void f_affiche_stats()
{
   FILE* output_file = fopen("output.csv", "a");
   double average_elapsed_time = 1000.0 * stats.total_elapsed_time / stats.num_AI_calls;
   double average_moves_per_turn = (double)stats.num_tested_moves / stats.num_searched_nodes;
   double average_nodes_per_call = (double)stats.num_searched_nodes / stats.num_AI_calls;

   fprintf(output_file, "%s:%d:%g:%g:%g\n", (ALPHA_BETA) ? "True" : "False", PROF_MAX, average_moves_per_turn, average_elapsed_time, average_nodes_per_call);

   printf("****** STATISTIQUES ******\n");
   printf("WITH ALPHA BETA = %s\n", (ALPHA_BETA) ? "yes" : "no");
   printf("MAX DEPTH = %d\n", PROF_MAX);
   printf("NUM OF AI CALLS = %d function calls\n", stats.num_AI_calls);
   printf("AVERAGE MOVES PER TURN= %g nodes\n", average_moves_per_turn) ;
   printf("AVERAGE NODES PER CALL = %g nodes\n", average_nodes_per_call) ;
   printf("AVERAGE ELPASED TIME = %.2fms\n", average_elapsed_time);
   printf("**************************\n");
}

int f_win_check(int joueur, int* player_counter, int* opponent_counter,
                int voisinX, int old_dest, int new_dest)
{

   int goal = (joueur == 1) ? 0 : 9; // goal line

   if(voisinX == goal) { // if we moved to the goal line the node is winning and therefore terminal
      return 1;
   }

   // update pawn counters only if the destination
   // coordinated had an opponent's pawn
   if(old_dest == -joueur) {
      // check the destination coordinate for who won the battle
      if(new_dest == joueur) { // if the player pawn won
         (*opponent_counter)--; // the opponent loses a pawn
      } else {
         (*player_counter)--; // otherwise we lost one
      }

      // check if either pawn counters are 0
      if((*player_counter) <= 0) {
         return -1; // if we lost our last pawn in a failed attack
      }

      if((*opponent_counter) <= 0) {
         return 1; // if we successfully finished all opponent pawns
      }
   }

   return 0; // neither won
}


// algo negamax sans alpha beta
int f_negamax(Pion* plateau_courant, int profondeur, int joueur, int player_counter, int opponent_counter)
{
   if(profondeur <= 0) {
      return f_eval(plateau_courant, joueur);
   }

   // update stats; adding a node search call to the total
   stats.num_searched_nodes ++;

   int maxval = -INFINI;

   int has_next = 1; // flag to signal a terminal node (no possible moves)

   Pion* plateau_suivant = (Pion *) malloc(NB_LIGNES * NB_COLONNES * sizeof (Pion));
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
                     // win check
                     int new_dest = plateau_suivant[i * NB_COLONNES + j].couleur; // new destination pawn
                     int old_dest = plateau_courant[i * NB_COLONNES + j].couleur; // old destination pawn

                     int win_check = f_win_check(joueur, &player_counter, &opponent_counter, voisinX, old_dest, new_dest);

                     // if any of the two won
                     if(win_check) {
                        return win_check * INFINI;
                     }

                     has_next = 0; // if there is at least one move we unset the flag

                     // update stats; adding a move to the total
                     stats.num_tested_moves++;

                     int newval = -f_negamax(plateau_suivant, profondeur - 1, -joueur, player_counter, opponent_counter);

                     // update the maximum value
                     if(newval > maxval) {
                        maxval = newval;

                        // set the move
                        if(profondeur == PROF_MAX) {
                           fromX = i,     fromY = j;
                           toX = voisinX, toY = voisinY;
                        }
                     }

                     // undo the move:
                     // put the origin and destination cell pieces back
                     // (in case there was an attack; ie: neighboring
                     // cell had an enemy pawn)

                     plateau_suivant[i       * NB_COLONNES + j      ] = plateau_courant[i       * NB_COLONNES + j      ];
                     plateau_suivant[voisinX * NB_COLONNES + voisinY] = plateau_courant[voisinX * NB_COLONNES + voisinY];

                     // f_bouge_piece(plateau_suivant, voisinX, voisinY, i, j, joueur);
                  }

               }
            }
      }
   }


   free(plateau_suivant);

   // if the node is terminal return the evaluation of the current node
   if(!has_next) {
      return f_eval(plateau_courant, joueur);
   }

   return maxval;
}

// algo negamax avec alpha beta maximize toujours pour le joueur donné
int f_negamax_ab(Pion* plateau_courant, int profondeur, int joueur, int alpha, int beta, int player_counter, int opponent_counter)
{
   if(profondeur <= 0) {
      return f_eval(plateau_courant, joueur);
   }

   // update stats; adding a node search call to the total
   stats.num_searched_nodes ++;

   int maxval = -INFINI;

   int has_next = 1; // in case the node is terminal

   Pion* plateau_suivant = (Pion *) malloc(NB_LIGNES * NB_COLONNES * sizeof (Pion));
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
                     // win check
                     int new_dest = plateau_suivant[i * NB_COLONNES + j].couleur; // new destination pawn
                     int old_dest = plateau_courant[i * NB_COLONNES + j].couleur; // old destination pawn

                     int win_check = f_win_check(joueur, &player_counter, &opponent_counter, voisinX, old_dest, new_dest);

                     // if any of the two won
                     if(win_check) {
                        return win_check * INFINI;
                     }

                     has_next = 0; // if theres at least one possible move the node is not terminal

                     // update stats
                     stats.num_tested_moves++;

                     int newval = -f_negamax_ab(plateau_suivant, profondeur - 1, -joueur, -beta, -alpha, player_counter, opponent_counter); // note that the alpha and beta are reversed

                     // update the maximum value
                     if(newval > maxval) {
                        maxval = newval;

                        // set the move
                        if(profondeur == PROF_MAX) {
                           fromX = i, fromY = j      ;
                           toX   = voisinX, toY   = voisinY;
                        }
                     }

                     // trim the alpha value
                     if(alpha < maxval) {
                        alpha = maxval;
                     }

                     // cut off if the [alpha, beta] range is empty
                     if(alpha >= beta) {
                        goto CLEAN_EXIT; // goto to exit all four loops
                     }

                     // undo the move:
                     // put the origin and destination cell piece back
                     // (in case there was an attack; ie: neighboring
                     // cell had an enemy pawn)

                     plateau_suivant[i       * NB_COLONNES + j      ] = plateau_courant[i       * NB_COLONNES + j      ];
                     plateau_suivant[voisinX * NB_COLONNES + voisinY] = plateau_courant[voisinX * NB_COLONNES + voisinY];

                     // f_bouge_piece(plateau_suivant, voisinX, voisinY, i, j, joueur);
                  }

               }
            }

         }

      }
   }


// to perform clean exits when cutting branches off
CLEAN_EXIT:
   free(plateau_suivant);

   // if the node is terminal return the evaluation of the current node
   if(!has_next) {
      return f_eval(plateau_courant, joueur);
   }

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

   int player_counter = f_nbPions(plateauDeJeu, joueur);
   int opponent_counter = f_nbPions(plateauDeJeu, -joueur);

#if ALPHA_BETA==1
   eval = f_negamax_ab(plateauDeJeu, PROF_MAX, joueur, -INFINI, INFINI, player_counter, opponent_counter);
#else
   eval = f_negamax(plateauDeJeu, PROF_MAX, joueur, player_counter, opponent_counter);
#endif

   end = clock();

   //update stats
   stats.num_AI_calls ++;
   stats.total_elapsed_time += ((double) end - strt) / CLOCKS_PER_SEC;

   f_bouge_piece(plateauDeJeu, fromX, fromY, toX, toY, joueur);

   printf("\n IA move for %c with eval %d: %d%c%d%c\n", (joueur == 1) ? 'x' : 'o', eval,
          fromX,      f_convert_int2char(fromY),
          toX,        f_convert_int2char(toY));


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
