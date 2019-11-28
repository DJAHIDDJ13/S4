#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fuzzy_infer.h"

/* Calculates the y value of line at a specific *x*
 *
 *   ^
 * y2|_________
 *   |       /|
 *   |      / |
 * i |____ /__|________
 *   |    /|  |
 * y1|___/_|__|________
 *     x1  x 0 x2
 *
 * where 'i' is the desired value
 */
float getLinearIntersection(float x1, float y1, float x2, float y2, float x)
{
   if (x1 - x2 == 0) {
      fprintf(stderr, "Division by zero while calculating intersection\n");
      return -1;
   }

   float a = (y1 - y2) / (x1 - x2);
   float b = (x1 * y2 - x2 * y1) / (x1 - x2);

   return a * x + b;
}

float getLinearIntersectionX(float x1, float y1, float x2, float y2, float y)
{
   if (x1 - x2 == 0) {
      fprintf(stderr, "Division by zero while calculating intersection\n");
      return -1;
   }

   float a = (y1 - y2) / (x1 - x2);
   float b = (x1 * y2 - x2 * y1) / (x1 - x2);

   return (y - b) / a;
}

/*
 * Calculates the area of a trapezoid
 */
float getTrapezoidArea(float b1, float b2, float height)
{
   return height * ((b1 + b2) / 2);
}

/*
 * Returns either 0 if ling does not belong to set
 */
float getTrapezoidMembership(FUZZY_SET set, float in)
{
   if (in < set.x1) {
      return 0.0;
   }

   else if (in <= set.x2 && set.x1 < set.x2) {
      return getLinearIntersection(set.x1, 0, set.x2, set.height, in);
   }

   else if (in <= set.x3) {
      return set.height;
   }

   else if (in <= set.x4 && set.x3 < set.x4) {
      return getLinearIntersection(set.x3, 0, set.x4, set.height, in);
   }

   else {
      return 0.0;
   }

   return -1;
}
/*
TRAP_FUZZY_SET transformToTrapezoid(FUZZY_SET set)
{
   TRAP_FUZZY_SET temp = {0};
   temp.set_name = set.trapezoid.set_name;
   temp.type = TRAPEZOID;
   temp.height = set.trapezoid.height;

   switch (set.trapezoid.type) {
   case TRAPEZOID:
      memcpy(&temp, &set, sizeof(temp));
      break;

   case TRIANGLE:
      temp.x1 = set.triangle.x1;
      temp.x2 = set.triangle.x2;
      temp.x3 = set.triangle.x2;
      temp.x4 = set.triangle.x3;
      break;

   case RECTANGLE:
      temp.x1 = set.rectangle.x1;
      temp.x2 = set.rectangle.x1;
      temp.x3 = set.rectangle.x2;
      temp.x4 = set.rectangle.x2;
      break;

   case POLYGON:
      return temp;
   }

   return temp;
}
*/
float getSetMembership(FUZZY_SET set, float input)
{
   return getTrapezoidMembership(set, input);
}

/*
 * Finds the center of mass of a right triangle
 * a, b, c are the x coordinates of the triangle points
 */
float getTriangleCentroid(float a, float b, float c)
{
   return (a + b + c) / 3;
}

/*
 * Finds the center of mass of a rectangle
 * a, b are the x coordinates of the base
 */
float getRectangleCentroid(float a, float b)
{
   return (a + b) / 2;
}

void clipTrapezoid(FUZZY_SET set, float new_h, float* new_x2, float* new_x3)
{
   if(set.x1 != set.x2) {
      *new_x2 = getLinearIntersectionX(set.x1, 0, set.x2, set.height, new_h);
   } else {
      *new_x2 = set.x1;
   }

   if(set.x3 != set.x4) {
      *new_x3 = getLinearIntersectionX(set.x3, set.height, set.x4, 0, new_h);
   } else {
      *new_x3 = set.x3;
   }
}
