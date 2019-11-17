#include <stdio.h>
#include "fuzzy_infer.h"

int main(int argc, const char *argv[])
{
   IN_LING_VAR ling = createInputVar("distance", 0, 10, 5);

   FUZZY_SET set1 = createTriangleSet("close", 0, 4, 6, 1);
   FUZZY_SET set2 = createTriangleSet("far", 4, 7, 9, 1);

   addFuzzySet(&ling, set1);
   addFuzzySet(&ling, set2);

   /*
   addfuzzyrule("IF left_sensor = close AND right_sensor = far THEN steering = right");
   addfuzzyrule("IF left_sensor = far AND right_sensor = far THEN steering = forward");
   addfuzzyrule("IF left_sensor = far AND right_sensor = close THEN steering = left");
   */

   printf("Membershipt of distance to close = %g\n", getInputMembership(ling, "close"));
   printf("Membershipt of distance to far = %g\n", getInputMembership(ling, "far"));

   freeInputVar(&ling);

   return 0;
}
