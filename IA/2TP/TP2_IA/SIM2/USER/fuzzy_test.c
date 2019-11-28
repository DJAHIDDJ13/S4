#include <stdio.h>
#include "fuzzy_infer.h"

int main(int argc, const char *argv[])
{ 
   IN_LING_VAR *r_sensor = createInputVar("r_sensor", 0, 1023, 0);
   IN_LING_VAR *l_sensor = createInputVar("l_sensor", 0, 1023, 0);
   FUZZY_SET *close = createTrapezoidSet("close", 400, 550, 1023, 1023,  1);
   FUZZY_SET *far = createTrapezoidSet("far", 0, 0, 450, 600, 1);
   
   addFuzzySet(r_sensor, close);
   addFuzzySet(r_sensor, far);
   addFuzzySet(l_sensor, close);
   addFuzzySet(l_sensor, far);

   OUT_LING_VAR *steer = createOutputVar("steering", -5, 5);
   FUZZY_SET *left = createTrapezoidSet("left", -5, -5, -2, -1, 1);
   FUZZY_SET *forward = createTriangleSet("forward", -2, 0, 2, 1);
   FUZZY_SET *right = createTrapezoidSet("right", 1, 2, 5, 5, 1);

   addFuzzySet(steer, right);
   addFuzzySet(steer, forward);
   addFuzzySet(steer, left);

   /*
   addFuzzyRule("IF left_sensor = close AND right_sensor = far THEN steering = right");
   addFuzzyRule("IF left_sensor = far AND right_sensor = far THEN steering = forward");
   addFuzzyRule("IF left_sensor = far AND right_sensor = close THEN steering = left");
   */

   FUZZY_RULE* rules = NULL;
   rules = pushFuzzyRule(rules, close, far  , right);
   rules = pushFuzzyRule(rules, far  , far  , forward);
   rules = pushFuzzyRule(rules, far  , close, left);
   
   FUZZY_SYS sys = initSystem(l_sensor, r_sensor, steer, rules);
   float crisp = run(&sys, 1023, 1023);
   printf("crisp = %g\n", crisp);
   // printf("Membership of distance to close = %g\n", getInputMembership(l_sensor, "close"));
   // printf("Membership of distance to far = %g\n", getInputMembership(r_sensor, "far"));

   freeSystem(&sys);
   return 0;
}
