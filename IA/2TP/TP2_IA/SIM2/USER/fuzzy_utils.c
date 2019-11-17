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
float getLinearIntersection(int x1, int y1, int x2, int y2, int x)
{
   if(x1 - x2 == 0) {
      fprintf(stderr, "Division by zero while calculating intersection\n");
      return -1;
   }
   float a = (float) (y1 - y2) / (x1 - x2);
   float b = (float) (x1 * y2 - x2 * y1) / (x1 - x2);

   return a * x + b;
}

/*
 * Calculates the area of a trapezoid
 */
float getTrapezoidArea(int a, int b, int c, int d, int height)
{
   int b1 = c - b;  // upper base
   int b2 = d - a;  // lower base

   return height * ((b1 + b2) / 2);
}

/*
 * Returns either 0 if ling does not belong to set
 */
float getTrapezoidMembership(TRAP_FUZZY_SET set, int in)
{
   if(in < set.x1 || in >= set.x4) {
      return 0.0;
   }

   else if(in < set.x2) {
      return getLinearIntersection(set.x1, 0, set.x2, 1, in);
   }

   else if(in < set.x3) {
      return 1.0;
   }

   else if(in < set.x4) {
      return getLinearIntersection(set.x3, 0, set.x4, 1, in);
   }

   return -1;
}

TRAP_FUZZY_SET transformToTrapezoid(FUZZY_SET set) {
   TRAP_FUZZY_SET temp = {0};
   memcpy(&temp.set_name, &set.trapezoid.set_name, sizeof(char) * MAX_NAME_LEN);
   temp.type = TRAPEZOID;
   temp.height = set.trapezoid.height;

   switch(set.trapezoid.type) {
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

float getSetMembership(FUZZY_SET set, int input) {
   return getTrapezoidMembership(transformToTrapezoid(set), input);
} 

/*
 * Finds the center of mass of a right triangle
 * a, b, c are the x coordinates of the triangle points
 */
float getTriangleCentroid(int a, int b, int c)
{
   return (float) (a + b + c) / 3;
}

/*
 * Finds the center of mass of a rectangle
 * a, b are the x coordinates of the base
 */
float getRectangleCentroid(int a, int b)
{
   return (float) (a + b) / 2;
}


