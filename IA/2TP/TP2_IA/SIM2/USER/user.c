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
   printf("INITIALIZING FUZZY SYSTEMS\n");

   IN_LING_VAR *r_sensor = createInputVar("right_sensor", 0, 1023, 0);
   IN_LING_VAR *l_sensor = createInputVar("left_sensor" , 0, 1023, 0);
   
   FUZZY_SET *close = createTrapezoidSet("close", 200, 300, 1023, 1023, 1);
   FUZZY_SET *far   = createTrapezoidSet("far"  , 0  , 0  , 100 , 250 , 1);

   addFuzzySet(r_sensor, close);
   addFuzzySet(r_sensor, far);
   addFuzzySet(l_sensor, close);
   addFuzzySet(l_sensor, far);

   OUT_LING_VAR *l_motor = createOutputVar("left_motor" , -5, 5);
   OUT_LING_VAR *r_motor = createOutputVar("right_motor", -5, 5);

   FUZZY_SET *reverse= createTrapezoidSet("reverse", -4, -3 , -2 , -1, 1);
   FUZZY_SET *low    = createTriangleSet ("low"    , 0 , 0.2, 0.5    , 1);
   FUZZY_SET *high   = createTrapezoidSet("high"   , 3 , 4  , 5  , 5 , 1);

   addFuzzySet(l_motor, high   );
   addFuzzySet(l_motor, low    );
   addFuzzySet(l_motor, reverse);
   addFuzzySet(r_motor, high   );
   addFuzzySet(r_motor, low    );
   addFuzzySet(r_motor, reverse);

/*
   FUZZY_SET l_motor_rules[][3]  = {
      {close, far  , low},
      {far, close ,high},
      {far, far, high},
      {close, close, low}
   };
   
   FUZZY_SET r_motor_rules[][3]  = {
      {close, far  , high},
      {far, close ,low},
      {far, far, high},
      {close, close, reverse}
   };
  */ 
   FUZZY_RULE* rules1 = NULL; // rules for the left motor
   rules1 = pushFuzzyRule(rules1, close, far  , low );
   rules1 = pushFuzzyRule(rules1, far  , close, high);
   rules1 = pushFuzzyRule(rules1, far  , far  , high);
   rules1 = pushFuzzyRule(rules1, close, close, low );

   FUZZY_RULE* rules2 = NULL; // rules for the right motor
   rules2 = pushFuzzyRule(rules2, close, far  , high   );
   rules2 = pushFuzzyRule(rules2, far  , close, low    );
   rules2 = pushFuzzyRule(rules2, far  , far  , high   );
   rules2 = pushFuzzyRule(rules2, close, close, reverse);
   
   sys1 = initSystem(l_sensor, r_sensor, l_motor, rules1); 
   sys2 = initSystem(l_sensor, r_sensor, r_motor, rules2);
}

void UserClose(struct Robot *robot)
{
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
   
  
   float l_s = (robot->IRSensor[3].DistanceValue + robot->IRSensor[4].DistanceValue + robot->IRSensor[5].DistanceValue) / 3;
   float r_s = (robot->IRSensor[0].DistanceValue + robot->IRSensor[1].DistanceValue + robot->IRSensor[2].DistanceValue) / 3;
   //Besoin de rajouter les règles de logique floues ici

   float crisp1 = run(&sys1, l_s, r_s);
   float crisp2 = run(&sys2, l_s, r_s);
  
   robot->Motor[LEFT].Value  = crisp1;
   robot->Motor[RIGHT].Value = crisp2;
   
   printf("[%g %g] %g %g\n",l_s, r_s, crisp1, crisp2);
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


