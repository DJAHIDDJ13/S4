#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fuzzy_infer.h"
/*
FUZZY_SET* mamdaniInference(FUZZY_RULE* rules, int in1, int in2)
{
   FUZZY_RULE* rule = rules;
   FUZZY_SET* mam_out = NULL;

   while(rule) {
      float val1 = getInputMembership(&rule->A, in1);
      float val2 = getInputMembership(&rule->B, in2);

      FUZZY_SET to_add;

      if(val1 < val2) {
         memcpy(&to_add, &rule->A);
         to_add.height = val1;
         mam_out = pushFuzzySet(mam_out, rule->A);
      } else {
         memcpy(&to_add, &rule->B);
         to_add.height = val2;
         mam_out = pushFuzzySet(mam_out, );
      }

      rule = rule->next;
   }

   return mam_out;
}

int defuzzifyCentroid(OUT_LING_VAR* out)
{
   float sum = 0.0;

   for(int i = out->min_x; i < out->max_x; i++) {
      get_value
   }
}
*/

FUZZY_SYS initSystem() {
   FUZZY_SYS s;
   return s;
}

FUZZY_RULE createFuzzyRule(IN_LING_VAR x, FUZZY_SET A, IN_LING_VAR y, FUZZY_SET B, OUT_LING_VAR z, FUZZY_SET C)
{
   FUZZY_RULE rule;
   rule.x = x;
   rule.y = y;
   rule.z = z;
   rule.A = A;
   rule.B = B;
   rule.C = C;
   rule.next = NULL;

   return rule;
}

/*
FUZZY_RULE create121FuzzyRule(LING_VAR x, FUZZY_SET A, LING_VAR y, FUZZY_SET B)
{
   FUZZY_RULE rule;
   rule.x = rule.y = x;
   rule.A = rule.B = A;
   rule.z = y;
   rule.C = B;
   rule.next = NULL;

   return rule;
}
*/

FUZZY_RULE* pushFuzzyRule(FUZZY_RULE* rules, FUZZY_RULE to_add)
{
   FUZZY_RULE* new_rule = malloc(sizeof(FUZZY_RULE));
   memcpy(new_rule, &to_add, sizeof(FUZZY_RULE));
   new_rule->next = rules;

   return new_rule;
}

void freeFuzzyRule(FUZZY_RULE* rule)
{
   memset(rule, 0, sizeof(FUZZY_RULE));
   free(rule);
}

FUZZY_RULE* popFuzzyRule(FUZZY_RULE* rule)
{
   FUZZY_RULE* to_ret = rule->next;
   freeFuzzyRule(rule);

   return to_ret;
}

FUZZY_SET createTrapezoidSet(const char* name, int x1, int x2, int x3, int x4, int height)
{
   FUZZY_SET set = {0};
   set.trapezoid.type = TRAPEZOID;
   memcpy(&set.trapezoid.set_name, name, sizeof(char) * MAX_NAME_LEN);
   set.trapezoid.x1 = x1;
   set.trapezoid.x2 = x2;
   set.trapezoid.x3 = x3;
   set.trapezoid.x4 = x4;

   return set;
}
FUZZY_SET createTriangleSet(const char* name, int x1, int x2, int x3, int height)
{
   FUZZY_SET set = {0};
   set.triangle.type = TRIANGLE;
   memcpy(&set.triangle.set_name, name, sizeof(char) * MAX_NAME_LEN);
   set.triangle.x1 = x1;
   set.triangle.x2 = x2;
   set.triangle.x3 = x3;
   
   return set;
}
FUZZY_SET createRectangleSet(const char* name, int x1, int x2, int height)
{
   FUZZY_SET set = {0};
   set.rectangle.type = RECTANGLE;
   memcpy(&set.rectangle.set_name, name, sizeof(char) * MAX_NAME_LEN);
   set.rectangle.x1 = x1;
   set.rectangle.x2 = x2;
   
   return set;
}

FUZZY_SET_LIST* pushFuzzySetList(FUZZY_SET_LIST* list, FUZZY_SET set)
{
   FUZZY_SET_LIST* new_list = malloc(sizeof(FUZZY_SET_LIST));
   memcpy(&new_list->current, &set, sizeof(set));
   new_list->next = list;

   return new_list;
}

FUZZY_SET_LIST* popFuzzySetList(FUZZY_SET_LIST* list)
{
   FUZZY_SET_LIST* to_ret = list->next;
   free(list);

   return to_ret;
}

IN_LING_VAR createInputVar(const char* var_name, int min_x, int max_x, int input)
{
   IN_LING_VAR ling  = {0};
   
   if(min_x >= max_x) {
      fprintf(stderr, "min_x has to be smaller than max_x\n");
      return ling;
   }
   if(input < min_x || input >= max_x) {
      fprintf(stderr, "The input is not in the range [min_x, max_x]\n");
      return ling;
   }

   memcpy(&ling.var_name, &var_name, sizeof(char) * MAX_NAME_LEN);
   ling.min_x = min_x;
   ling.max_x = max_x;
   ling.input = input;
   ling.fuzzy_sets = NULL;

   return ling;
}

void addFuzzySet(IN_LING_VAR* ling, FUZZY_SET set)
{
   ling->fuzzy_sets = pushFuzzySetList(ling->fuzzy_sets, set);
}

void freeInputVar(IN_LING_VAR* ling)
{
   FUZZY_SET_LIST* list = ling->fuzzy_sets;
   ling->fuzzy_sets = NULL;

   while(list != NULL) {
      list = popFuzzySetList(list);
   }

}

float getInputMembership(IN_LING_VAR in, const char* set_name)
{
   float out = -1.0;
   for(FUZZY_SET_LIST* l = in.fuzzy_sets; l != NULL; l = l->next) {
      FUZZY_SET set = l->current;
      if(strcmp(set_name, set.trapezoid.set_name) == 0) {
         out = getSetMembership(set, in.input);
         break;
      }
   }

   return out;
}
