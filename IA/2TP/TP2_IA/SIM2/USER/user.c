/*****************************************************************************/
/* File:        user.c (Khepera Simulator)                                   */
/* Author:      Olivier MICHEL <om@alto.unice.fr>                            */
/* Date:        Thu Sep 21 14:39:05 1995                                     */
/* Description: example of user.c file                                       */
/*                                                                           */
/* Copyright (c) 1995                                                        */
/* Olivier MICHEL                                                            */
/* MAGE team, i3S laboratory,                                                */
/* CNRS, University of Nice - Sophia Antipolis, FRANCE                       */
/*                                                                           */
/* Permission is hereby granted to copy this package for free distribution.  */
/* The author's name and this copyright notice must be included in any copy. */
/* Commercial use is forbidden.                                              */
/*****************************************************************************/

#include "../SRC/include.h"
#include "user_info.h"
#include "user.h"
#include "fuzzy_infer.h"

#define FORWARD_SPEED   5                    /* normal (slow) forward speed*/
#define TURN_SPEED      4                    /* normal (slow) turn speed */
#define COLLISION_TH    900                  /* value of IR sensors to be 
                                                  considered as collision */
int pas = 0;
FUZZY_SYS sys1 = {0}, sys2 = {0};

void DrawStep()
{
   char text[256];

   sprintf(text, "step = %d", pas);
   Color(GREY);
   UndrawText(200, 100, "step = 500");
   Color(BLUE);
   DrawText(200, 100, text);
}

void UserInit(struct Robot *robot)
{
   /**
    * Initialisation des systémes floues
    */

   // variables linguistique pour le capteur droite et gauche
   IN_LING_VAR *r_sensor = createInputVar("right_sensor", 0, 1023, 0);
   IN_LING_VAR *l_sensor = createInputVar("left_sensor" , 0, 1023, 0);
   
   // les fonction d'appartenance pour les valeurs capteurs
   FUZZY_SET *close = createTrapezoidSet("close", 300, 350, 1023, 1023, 1);
   FUZZY_SET *half  = createTriangleSet ("half" , 150, 250,        350, 1);
   FUZZY_SET *far   = createTrapezoidSet("far"  , 0  , 0  , 100 , 250 , 1);
   
   // ajout des fonction aux variables linguistiques des capteurs
   addFuzzySet(r_sensor, close);
   addFuzzySet(r_sensor, far);
   addFuzzySet(r_sensor, half);
   addFuzzySet(l_sensor, close);
   addFuzzySet(l_sensor, far);
   addFuzzySet(l_sensor, half);

   // variables linguistiques pour le moteur droite et gauche
   OUT_LING_VAR *l_motor = createOutputVar("left_motor" , -7, 7);
   OUT_LING_VAR *r_motor = createOutputVar("right_motor", -7, 7);

   // les fonctions d'appartenence pour les vitesses de moteurs
   FUZZY_SET *reverse= createTrapezoidSet("reverse"    , -6, -5 , -4 , -3, 1);
   FUZZY_SET *stop   = createTriangleSet ("stop"      , -0.2 , 0, 0.2    , 1);
   FUZZY_SET *high   = createTrapezoidSet("high"       , 3 , 4  , 5  , 6 , 1);

   // ajout des fonction aux variables linguistiques des moteurs
   addFuzzySet(l_motor, high   );
   addFuzzySet(l_motor, stop    );
   addFuzzySet(l_motor, reverse);
   addFuzzySet(r_motor, high   );
   addFuzzySet(r_motor, stop    );
   addFuzzySet(r_motor, reverse);

   // régles pour le systéme de moteur gauche
   FUZZY_RULE* rules1 = NULL;
   rules1 = pushFuzzyRule(rules1, close, far  , stop   );
   rules1 = pushFuzzyRule(rules1, far  , close, high   );
   rules1 = pushFuzzyRule(rules1, far  , far  , high   );
   rules1 = pushFuzzyRule(rules1, close, half , reverse);
   rules1 = pushFuzzyRule(rules1, half , close, high   );

   // régles pour le systéme de moteur droite
   FUZZY_RULE* rules2 = NULL;
   rules2 = pushFuzzyRule(rules2, close, far  , high   );
   rules2 = pushFuzzyRule(rules2, far  , close, stop   );
   rules2 = pushFuzzyRule(rules2, far  , far  , high   );
   rules2 = pushFuzzyRule(rules2, close, half , high   );
   rules2 = pushFuzzyRule(rules2, half , close, reverse);
   
   // ajout des regles et variables linguistiques dans les systémes d'inference

   // systéme 1 utilise les 'rules1' avec les meme valeurs de capteurs pour
   // trouver la sortie pour le moteur gauche
   sys1 = initSystem(l_sensor, r_sensor, l_motor, rules1);

   // systéme 2 fait le meme chose mais avec 'rules2' et pour le moteur droite
   sys2 = initSystem(l_sensor, r_sensor, r_motor, rules2);
}

void UserClose(struct Robot *robot)
{
   // liberation des systémes d'inference
   freeSystem(&sys1);
   freeSystem(&sys2);
}

void NewRobot(struct Robot *robot)
{
   pas = 0;
}

void LoadRobot(struct Robot *robot, FILE *file)
{
}

void SaveRobot(struct Robot *robot, FILE *file)
{
}

void RunRobotStart(struct Robot *robot)
{
   ShowUserInfo(2, 1);
}

void RunRobotStop(struct Robot *robot)
{
   ShowUserInfo(1, 1);
}

boolean StepRobot(struct Robot *robot)
{
   pas++;
   DrawStep();
   
   // on prend le moyenne des 3 capteurs gauche pour la valeur d'entree 'crisp' gauche
   float l_s = (robot->IRSensor[3].DistanceValue +
                robot->IRSensor[4].DistanceValue +
                robot->IRSensor[5].DistanceValue) / 3;
   
   // meme chose pour les 3 capteurs droite
   float r_s = (robot->IRSensor[0].DistanceValue +
                robot->IRSensor[1].DistanceValue +
                robot->IRSensor[2].DistanceValue) / 3;
   
   // les resultats d'inference pour les deux moteurs
   float crisp1 = run(&sys1, l_s, r_s);
   float crisp2 = run(&sys2, l_s, r_s);

   // on utilise les resultats d'inference apres l'arrondissment  
   robot->Motor[LEFT].Value  = round(crisp1);
   robot->Motor[RIGHT].Value = round(crisp2);
   
   return(TRUE);
}

void FastStepRobot(struct Robot *robot)
{
}

void ResetRobot(struct Robot *robot)
{
   pas = 0;
}

void UserCommand(struct Robot *robot, char *text)
{
   WriteComment("unknown command"); /* no commands */
}
void DrawUserInfo(struct Robot *robot, u_char info, u_char page)
{
   char text[256];
   switch(info) {
   case 1:
      switch(page) {
      case 1:
         Color(MAGENTA);
         FillRectangle(0, 0, 40, 40);
         Color(BLUE);
         DrawLine(100, 100, 160, 180);
         Color(WHITE);
         DrawPoint(200, 200);
         Color(YELLOW);
         DrawRectangle(240, 100, 80, 40);
         Color(GREEN);
         DrawText(240, 230, "hello world");
         break;

      case 2:
         Color(RED);
         DrawArc(200, 50, 100, 100, 0, 360 * 64);
         Color(YELLOW);
         FillArc(225, 75, 50, 50, 0, 360 * 64);
         Color(BLACK);
         DrawText(140, 170, "This is the brain of the robot");
      }

      break;

   case 2:
      DrawStep();
   }
}


