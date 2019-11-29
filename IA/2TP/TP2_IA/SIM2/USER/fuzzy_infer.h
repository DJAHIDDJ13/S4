#ifndef FUZZY_INFER_H
#define FUZZY_INFER_H

typedef struct input_linguistic_variable IN_LING_VAR;
typedef struct output_linguistic_variable OUT_LING_VAR;

typedef struct fuzzy_set FUZZY_SET;
typedef struct fuzzy_set_list FUZZY_SET_LIST;
typedef struct fuzzy_rule FUZZY_RULE;

/*********************************************
 *             inference system              *
 *********************************************/

typedef struct fuzzy_logic_system {
   IN_LING_VAR *x, *y;
   OUT_LING_VAR *z;

   FUZZY_SET_LIST* out;

   FUZZY_RULE* rules;
} FUZZY_SYS;

FUZZY_SYS initSystem(IN_LING_VAR*, IN_LING_VAR*, OUT_LING_VAR*, FUZZY_RULE*);
void freeSystem(FUZZY_SYS* sys);
void fuzzify(FUZZY_SYS *sys, const char* var_name, float crisp);
void inference(FUZZY_SYS *sys);
float defuzzify(FUZZY_SYS* sys, int sample_size);
float run(FUZZY_SYS *sys, float crisp1, float crisp2);

/*********************************************
 *             Linguistic variable           *
 *********************************************/

/*
 * min_x and max_x define the range of the linguistic variables
 */
struct input_linguistic_variable {
   char *var_name; // variable name
   float min_x;                   // start of the x range
   float max_x;                   // end of the x range

   float input;

   FUZZY_SET_LIST* fuzzy_sets;  // list of fuzzy sets
};


struct output_linguistic_variable {
   char *var_name;                // variable name
   float min_x;                   // start of the x range
   float max_x;                   // end of the x range

   FUZZY_SET_LIST* fuzzy_sets;  // list of fuzzy sets
};

IN_LING_VAR *createInputVar(const char* var_name, float min_x, float max_x, float input);
OUT_LING_VAR *createOutputVar(const char* var_name, float min_x, float max_x);

void addFuzzySet_IN(IN_LING_VAR* ling, FUZZY_SET *set);
void addFuzzySet_OUT(OUT_LING_VAR* ling, FUZZY_SET *set);
// pour faire un 'overload' des fonctions
#define addFuzzySet(_1, _2) _Generic((_1),                         \
                                    IN_LING_VAR*: addFuzzySet_IN,  \
                                    OUT_LING_VAR*: addFuzzySet_OUT \
                                    )(_1, _2)

void freeVar_IN(IN_LING_VAR* ling);
void freeVar_OUT(OUT_LING_VAR* ling);
#define freeVar(_1)         _Generic((_1),                        \
                                    IN_LING_VAR*: freeVar_IN,     \
                                    OUT_LING_VAR*: freeVar_OUT    \
                                    )(_1)

float getInputMembership(IN_LING_VAR in, const char* set_name);

/*********************************************
 *                 Fuzzy sets                *
 *********************************************/


/*
 * Trapezoid fuzzy set
 * h |      _______
 *   |     /       \
 *   |    /         \
 * 0 |___/___________\____
 *      x1 x2     x3  x4
 */
struct fuzzy_set {
   // FUZZY_SET_TYPE type;

   char *set_name; // the set's linguistic name eg: 'hot', 'far' etc..

   float x1;
   float x2;
   float x3;
   float x4;

   float height; // h
};

/*
 * Linked list of fuzzy sets
 */
struct fuzzy_set_list {
   FUZZY_SET *current;
   FUZZY_SET_LIST* next;
};

FUZZY_SET_LIST* pushFuzzySetList(FUZZY_SET_LIST* list, FUZZY_SET *set);
void freeFuzzySetList(FUZZY_SET_LIST* list);
FUZZY_SET *createTrapezoidSet(const char* name, float x1, float x2, float x3, float x4, float height);
FUZZY_SET *createTriangleSet(const char* name, float x1, float x2, float x3, float height);
FUZZY_SET *createRectangleSet(const char* name, float x1, float x2, float height);

/**********************************************
 *                 Fuzzy rules                *
 **********************************************/

/*
 * Mamdani fuzzy rule of this form:
 * IF    x IS A    AND    y IS B    THEN   z IS C
 * where x, y, z are the two input and output variables respectively
 * A, B, C are fuzzy_sets
 */
typedef struct fuzzy_rule {
   FUZZY_SET *A, *B, *C; // fuzzy sets of rule definition

   FUZZY_RULE* next;  // next fuzzy rule
} FUZZY_RULE;


FUZZY_RULE* pushFuzzyRule(FUZZY_RULE* rules, FUZZY_SET* A, FUZZY_SET* B, FUZZY_SET* C);
FUZZY_RULE* popFuzzyRule(FUZZY_RULE* rules);
void freeFuzzyRules(FUZZY_RULE* rulse);

/* Utility functions */
float getLinearIntersection(float x1, float y1, float x2, float y2, float x);
float getTrapezoidArea(float b1, float b2, float height);
float getTrapezoidMembership(FUZZY_SET set, float in);
float getSetMembership(FUZZY_SET set, float input);
float getLinearIntersectionX(float x1, float y1, float x2, float y2, float y);
void dumpTrapSet(FUZZY_SET s);
void clipTrapezoid(FUZZY_SET set, float new_h, float* new_x2, float* new_x3);
#endif

