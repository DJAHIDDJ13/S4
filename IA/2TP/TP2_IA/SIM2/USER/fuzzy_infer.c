#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

#include "fuzzy_infer.h"

void dumpTrapSet(FUZZY_SET s)
{
   printf("[%s] (%g, %g, %g, %g) %g\n", s.set_name, s.x1, s.x2, s.x3, s.x4, s.height);
}

float run(FUZZY_SYS *sys, float crisp1, float crisp2)
{
   // fuzzify(sys, sys->x->var_name, crisp1);
   // fuzzify(sys, sys->y->var_name, crisp2);
   sys->x->input = crisp1;
   sys->y->input = crisp2;

   inference(sys);
   float crisp_out = defuzzify(sys, 100);
   return crisp_out;
}

void fuzzify(FUZZY_SYS *sys, const char* var_name, float crisp)
{
   IN_LING_VAR* ling = NULL;

   if (strcmp(sys->x->var_name, var_name) == 0) {
      ling = sys->x;
   } else if (strcmp(sys->y->var_name, var_name) == 0) {
      ling = sys->y;
   }

   ling->input = crisp;
}

void inference(FUZZY_SYS *sys)
{
   if(sys->out) {
      freeFuzzySetList(sys->out);
   }

   sys->out = NULL;
   int in1 = sys->x->input;
   int in2 = sys->y->input;

   for (FUZZY_RULE* rule = sys->rules; rule != NULL; rule = rule->next) {
      float val1 = getSetMembership(*rule->A, in1);
      float val2 = getSetMembership(*rule->B, in2);

      FUZZY_SET C = *rule->C;
      
      float and_val = (val1 < val2) ? val1 : val2;
      
      float clipped_x2, clipped_x3;
      clipTrapezoid(C, and_val, &clipped_x2, &clipped_x3);
      FUZZY_SET* to_add = createTrapezoidSet("", C.x1, clipped_x2, clipped_x3, C.x4, and_val);

      sys->out = pushFuzzySetList(sys->out, to_add);
   }
}

float defuzzify(FUZZY_SYS* sys, int sample_size)
{
   float rng_len = sys->z->max_x - sys->z->min_x;
   float delta = rng_len / sample_size;

   FUZZY_SET_LIST* lst = sys->out;
   float center = 0.0;
   float prev_or = 0.0;
   float area = 0.0;

   for(int i = 0; i <= sample_size; i++) {
      float fi = sys->z->min_x + delta * i;
      // float fi = (float)i;
      float or = -FLT_MAX;

      for(FUZZY_SET_LIST* l = lst; l != NULL; l = l->next) {
         FUZZY_SET s = *l->current;
         float val = getSetMembership(s, fi);

         if(val > or ) {
            or = val;
         }
      }

      if(i > 0) {
         float loc_area = delta * (prev_or + or ) / 2.0;
         area += loc_area;
         center += (fi - delta / 2) * loc_area;
      }

      prev_or = or;
   }
   return (area == 0)? 0: center / area;
}


/*
int defuzzifyCentroid(OUT_LING_VAR* out)
{
   float sum = 0.0;

   for(int i = out->min_x; i < out->max_x; i++) {
      get_value
   }
}
*/

FUZZY_SYS initSystem(IN_LING_VAR* x, IN_LING_VAR* y, OUT_LING_VAR* z, FUZZY_RULE* rules)
{
   FUZZY_SYS sys = {
      x, y, z,
      NULL,
      rules
   };

   return sys;
}

void freeSystem(FUZZY_SYS* sys)
{
   freeFuzzyRules(sys->rules);
   sys->rules = NULL;
   freeVar(sys->x);
   sys->x = NULL;
   freeVar(sys->y);
   sys->y = NULL;
   freeVar(sys->z);
   sys->z = NULL;
   freeFuzzySetList(sys->out);
   sys->out = NULL;
}

FUZZY_RULE* pushFuzzyRule(FUZZY_RULE* rules, FUZZY_SET* A, FUZZY_SET* B, FUZZY_SET* C)
{
   FUZZY_RULE* rule = malloc(sizeof(FUZZY_RULE));
   rule->A = A;
   rule->B = B;
   rule->C = C;
   rule->next = rules;

   return rule;
}

FUZZY_RULE* popFuzzyRule(FUZZY_RULE* rule)
{
   FUZZY_RULE* to_ret = rule->next;
   free(rule);

   return to_ret;
}
void freeFuzzyRules(FUZZY_RULE* rules)
{
   while (rules) {
      rules = popFuzzyRule(rules);
   }
}

FUZZY_SET *createTrapezoidSet(const char* name, float x1, float x2, float x3, float x4, float height)
{
   FUZZY_SET *set = malloc(sizeof(FUZZY_SET));
   set->set_name = strdup(name);
   set->x1 = x1;
   set->x2 = x2;
   set->x3 = x3;
   set->x4 = x4;
   set->height = height;

   return set;
}
FUZZY_SET *createTriangleSet(const char* name, float x1, float x2, float x3, float height)
{
  return createTrapezoidSet(name, x1, x2, x2, x3, height);
}
FUZZY_SET *createRectangleSet(const char* name, float x1, float x2, float height)
{
   return createTrapezoidSet(name, x1, x1, x2, x2, height);
}

FUZZY_SET_LIST* pushFuzzySetList(FUZZY_SET_LIST* list, FUZZY_SET *set)
{
   FUZZY_SET_LIST* new_list = malloc(sizeof(FUZZY_SET_LIST));
   memcpy(&new_list->current, &set, sizeof(set));
   new_list->next = list;

   return new_list;
}

FUZZY_SET_LIST* popFuzzySetList(FUZZY_SET_LIST* list)
{
   FUZZY_SET_LIST* to_ret = list->next;
   if(list->current) {
      free(list->current->set_name);
      free(list->current);
   }
   list->current = NULL;
   free(list);

   return to_ret;
}

void freeFuzzySetList(FUZZY_SET_LIST* list)
{
   while(list) {
      list = popFuzzySetList(list);
   }
}

IN_LING_VAR* createInputVar(const char* var_name, float min_x, float max_x, float input)
{
   IN_LING_VAR* ling  = malloc(sizeof(IN_LING_VAR));

   if (min_x >= max_x) {
      fprintf(stderr, "min_x has to be smaller than max_x\n");
      return ling;
   }

   if (input < min_x || input >= max_x) {
      fprintf(stderr, "The input is not in the range [min_x, max_x]\n");
      return ling;
   }

   ling->var_name = strdup(var_name);
   ling->min_x = min_x;
   ling->max_x = max_x;
   ling->input = input;
   ling->fuzzy_sets = NULL;

   return ling;
}

OUT_LING_VAR* createOutputVar(const char* var_name, float min_x, float max_x)
{
   OUT_LING_VAR* ling  = malloc(sizeof(OUT_LING_VAR));

   if (min_x >= max_x) {
      fprintf(stderr, "min_x has to be smaller than max_x\n");
      return ling;
   }

   ling->var_name = strdup(var_name);
   ling->min_x = min_x;
   ling->max_x = max_x;
   ling->fuzzy_sets = NULL;

   return ling;
}

void addFuzzySet_OUT(OUT_LING_VAR* ling, FUZZY_SET *set)
{
   ling->fuzzy_sets = pushFuzzySetList(ling->fuzzy_sets, set);
}

void addFuzzySet_IN(IN_LING_VAR* ling, FUZZY_SET *set)
{
   ling->fuzzy_sets = pushFuzzySetList(ling->fuzzy_sets, set);
}

void freeVar_IN(IN_LING_VAR* ling)
{
   if(ling == NULL) {
      return;
   }

   if(ling->var_name) {
      free(ling->var_name);
      ling->var_name = NULL;
   }


   FUZZY_SET_LIST* list = ling->fuzzy_sets;
   ling->fuzzy_sets = NULL;

   while (list != NULL) {
      list = popFuzzySetList(list);
   }

}

void freeVar_OUT(OUT_LING_VAR* ling)
{
   if(ling == NULL) {
      return;
   }

   if(ling->var_name) {
      free(ling->var_name);
      ling->var_name = NULL;
   }
   FUZZY_SET_LIST* list = ling->fuzzy_sets;
   ling->fuzzy_sets = NULL;
   
   while (list != NULL) {
      list = popFuzzySetList(list);
   }

}

float getInputMembership(IN_LING_VAR in, const char* set_name)
{
   float out = -1.0;

   for (FUZZY_SET_LIST* l = in.fuzzy_sets; l != NULL; l = l->next) {
      FUZZY_SET set = *l->current;

      if (strcmp(set_name, set.set_name) == 0) {
         out = getSetMembership(set, in.input);
         break;
      }
   }

   return out;
}
