#ifndef FUZZY_INFER_H
#define FUZZY_INFER_H

typedef struct input_linguistic_variable IN_LING_VAR;
typedef struct output_linguistic_variable OUT_LING_VAR;

typedef struct fuzzy_set FUZZY_SET;
/*
typedef struct trapezoidal_fuzzy_set TRAP_FUZZY_SET;
typedef struct triangular_fuzzy_set TRI_FUZZY_SET;
typedef struct rectangular_fuzzy_set RECT_FUZZY_SET;
typedef struct polygonal_fuzzy_set POLY_FUZZY_SET;
*/
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
   char *var_name; // variable name
   float min_x;                   // start of the x range
   float max_x;                   // end of the x range

   FUZZY_SET_LIST* fuzzy_sets;  // list of fuzzy sets
};

IN_LING_VAR *createInputVar(const char* var_name, float min_x, float max_x, float input);
OUT_LING_VAR *createOutputVar(const char* var_name, float min_x, float max_x);

void addFuzzySet_IN(IN_LING_VAR* ling, FUZZY_SET *set);
void addFuzzySet_OUT(OUT_LING_VAR* ling, FUZZY_SET *set);
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

// typedef enum fuzzy_set_type {TRAPEZOID, TRIANGLE, RECTANGLE, POLYGON} FUZZY_SET_TYPE;

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
 * Triangle fuzzy set
 * h |      
 *   |     /\
 *   |    /  \
 * 0 |___/____\____
 *      x1 x2 x3
 */
/*struct triangular_fuzzy_set {
   FUZZY_SET_TYPE type;

   char *set_name; // the set's linguistic name eg: 'hot', 'far' etc..

   float x1;
   float x2;
   float x3;
   
   float height;
};
*/
/*
 * Rectangular fuzzy set
 * h |   ______   
 *   |  |      |
 *   |  |      | 
 * 0 |__|______|__
 *     x1      x2 
 */
/*struct rectangular_fuzzy_set {
   FUZZY_SET_TYPE type;

   char *set_name; // the set's linguistic name eg: 'hot', 'far' etc..

   float x1;
   float x2;
   
   float height;
};

*/
/*
 * List of polygon pofloats
 */
/*typedef struct polygon_list {
   float x, y;

   struct poly_list* next;
} POLY_LIST;
*/
/*
 * Polygonal fuzzy set
 *
 *y3 |     __ 
 *y2 |    /  \    ___
 *y1 |   /    \__/   \    ...
 * 0 |__/_____________\_
 *     x1 x2 x3 x4 x5  x6 ... 
 * pofloats = [x1, y2]->[x2,y2]-> ...
 */
/*struct polygonal_fuzzy_set {
   FUZZY_SET_TYPE type;

   char *set_name; // the set's linguistic name eg: 'hot', 'far' etc..

   POLY_LIST points;
};

union fuzzy_set {
   TRAP_FUZZY_SET trapezoid;
   TRI_FUZZY_SET triangle;
   RECT_FUZZY_SET rectangle;
   POLY_FUZZY_SET polygon;
};
*/
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
 * where x, y, z are linguistic variables and
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
// TRAP_FUZZY_SET transformToTrapezoid(FUZZY_SET set);
void clipTrapezoid(FUZZY_SET set, float new_h, float* new_x2, float* new_x3);
#endif

